/**************************************************************************\
 * Copyright (c) 2020 Zheng, Lei (realthunder) <realthunder.dev@gmail.com>
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\**************************************************************************/

#include "PreCompiled.h"

#include <QtOpenGL.h>
#if defined(HAVE_QT5_OPENGL)
# include <QApplication>
#endif

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <Inventor/misc/SoContextHandler.h>
#include <Inventor/misc/SoGLDriverDatabase.h>
#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/elements/SoGLCacheContextElement.h>
#include <Inventor/elements/SoShapeStyleElement.h>
#include <Inventor/C/tidbits.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/errors/SoDebugError.h>

#include "SoFCVBO.h"
#include "../SoFCInteractiveElement.h"
#include "SoFCVertexArrayIndexer.h"

using namespace Gui;

/**************************************************************************/

static int vbo_vertex_count_min_limit = -1;
static int vbo_vertex_count_max_limit = -1;
static int vbo_render_as_vertex_arrays = -1;
static int vbo_debug = -1;
static int vbo_context_shared = -1;
static long vbo_mem;
static long vbo_buf_mem;

// VBO rendering seems to be faster than other rendering, even for
// large VBOs. Just set the default limit very high
static const int DEFAULT_MAX_LIMIT = 1000000000;
static const int DEFAULT_MIN_LIMIT = 0;

static SbFCUniqueId _VboId;

/*!
  Constructor
*/
SoFCVBO::SoFCVBO(const GLenum target, const GLenum usage)
  : target(target),
    usage(usage),
    data(NULL),
    datasize(0),
    id(++_VboId)
{
  SoContextHandler::addContextDestructionCallback(context_destruction_cb, this);
}


//
// Callback from SoGLCacheContextElement
//
void
SoFCVBO::vbo_delete(void * closure, uint32_t contextid)
{
  const cc_glglue * glue = cc_glglue_instance((int) contextid);
  GLuint id = (GLuint) ((uintptr_t) closure);
  cc_glglue_glDeleteBuffers(glue, 1, &id);
}

/*!
  Destructor
*/
SoFCVBO::~SoFCVBO()
{
  SoContextHandler::removeContextDestructionCallback(context_destruction_cb, this);
  discard();
}

void
SoFCVBO::init(void)
{
  // use COIN_VBO_MAX_LIMIT to set the largest VBO we create
  if (vbo_vertex_count_max_limit < 0) {
    const char * env = coin_getenv("COIN_VBO_MAX_LIMIT");
    if (env) {
      vbo_vertex_count_max_limit = atoi(env);
    }
    else {
      vbo_vertex_count_max_limit = DEFAULT_MAX_LIMIT;
    }
  }

  // use COIN_VBO_MIN_LIMIT to set the smallest VBO we create
  if (vbo_vertex_count_min_limit < 0) {
    const char * env = coin_getenv("COIN_VBO_MIN_LIMIT");
    if (env) {
      vbo_vertex_count_min_limit = atoi(env);
    }
    else {
      vbo_vertex_count_min_limit = DEFAULT_MIN_LIMIT;
    }
  }

  // use COIN_VERTEX_ARRAYS to globally disable vertex array rendering
  if (vbo_render_as_vertex_arrays < 0) {
    const char * env = coin_getenv("COIN_VERTEX_ARRAYS");
    if (env) {
      vbo_render_as_vertex_arrays = atoi(env);
    }
    else {
      vbo_render_as_vertex_arrays = 1;
    }
  }

  if (vbo_debug < 0) {
    const char * env = coin_getenv("COIN_DEBUG_VBO");
    if (env) {
      vbo_debug = atoi(env);
    }
    else {
      vbo_debug = 0;
    }
  }
  if (vbo_context_shared < 0) {
#if defined(HAVE_QT5_OPENGL)
    vbo_context_shared = qApp->testAttribute(Qt::AA_ShareOpenGLContexts) ? 1 : 0;
#else
    vbo_context_shared = 0;
#endif
  }
}

void
SoFCVBO::onSetBuffer()
{
  vbo_buf_mem += (this->datasize<<20);
}

void
SoFCVBO::discard()
{
  auto size = this->datasize;
  vbo_buf_mem -= (size<<20);

  this->data = nullptr;
  this->datasize = 0;

  // schedule delete for all allocated GL resources
  for (auto & v : this->vbohash) {
    void * ptr = (void*) ((uintptr_t) v.handle);
    SoGLCacheContextElement::scheduleDeleteCallback(v.context, SoFCVBO::vbo_delete, ptr);
    vbo_mem -= (size << 20);
  }

  // clear hash table
  this->vbohash.clear();
}

/*!
  Binds the buffer for the context \a contextid.
*/
void
SoFCVBO::bindBuffer(SoState *state, uint32_t contextid)
{
  if ((this->data == NULL) ||
      (this->datasize == 0)) {
    assert(0 && "no data in buffer");
    return;
  }

  const cc_glglue * glue = cc_glglue_instance((int) contextid);

  if (vbo_context_shared && this->vbohash.size()) {
    // TODO: add explicit context sharing group checking. See qt5 implementation
    // in QOpenGLBuffer::bind()
    cc_glglue_glBindBuffer(glue, this->target, this->vbohash.begin()->handle);
    return;
  }

  GLuint buffer;
  auto it = std::lower_bound(this->vbohash.begin(), this->vbohash.end(), contextid);
  if (it == this->vbohash.end() || it->context != contextid) {
    if (state) {
      SoCacheElement::invalidate(state);
      SoGLCacheContextElement::shouldAutoCache(state,
                                               SoGLCacheContextElement::DONT_AUTO_CACHE);
    }
    // need to create a new buffer for this context
    cc_glglue_glGenBuffers(glue, 1, &buffer);
    cc_glglue_glBindBuffer(glue, this->target, buffer);
    cc_glglue_glBufferData(glue, this->target,
                           this->datasize,
                           this->data,
                           this->usage);
    this->vbohash.emplace(it, contextid, buffer);
    vbo_mem += (this->datasize<<20);
  }
  else {
    // buffer already exists, bind it
    cc_glglue_glBindBuffer(glue, this->target, it->handle);
  }
}

//
// Callback from SoContextHandler
//
void
SoFCVBO::context_destruction_cb(uint32_t context, void * userdata)
{
  SoFCVBO * thisp = (SoFCVBO*) userdata;

  auto it = std::lower_bound(thisp->vbohash.begin(), thisp->vbohash.end(), context);
  if (it != thisp->vbohash.end() && it->context == context) {
    const cc_glglue * glue = cc_glglue_instance((int) context);
    cc_glglue_glDeleteBuffers(glue, 1, &it->handle);
    thisp->vbohash.erase(it);
  }
}


/*!
  Sets the global limits on the number of vertex data in a node before
  vertex buffer objects are considered to be used for rendering.
*/
void
SoFCVBO::setVertexCountLimits(const int minlimit, const int maxlimit)
{
  vbo_vertex_count_min_limit = minlimit;
  vbo_vertex_count_max_limit = maxlimit;
}

/*!
  Returns the vertex VBO minimum limit.

  \sa setVertexCountLimits()
 */
int
SoFCVBO::getVertexCountMinLimit(void)
{
  return vbo_vertex_count_min_limit;
}

/*!
  Returns the vertex VBO maximum limit.

  \sa setVertexCountLimits()
 */
int
SoFCVBO::getVertexCountMaxLimit(void)
{
  return vbo_vertex_count_max_limit;
}



SbBool
SoFCVBO::shouldCreateVBO(SoState * state, const uint32_t contextid, const int numdata)
{
  (void)state;
  static int vbo_checked = 0;
  if (!vbo_checked) {
    vbo_checked = 1;
    const cc_glglue * glue = cc_glglue_instance(static_cast<int>(contextid));
    if (!cc_glglue_has_vertex_buffer_object(glue)
        || !SoGLDriverDatabase::isSupported(glue, SO_GL_FRAMEBUFFER_OBJECT))
      vbo_checked = -1;
  }
  if (vbo_checked < 0)
    return FALSE;
  // if (!vbo_render_as_vertex_arrays || !SoGLVBOActivatedElement::get(state))
  if (!vbo_render_as_vertex_arrays)
    return FALSE;
  int minv = SoFCVBO::getVertexCountMinLimit();
  int maxv = SoFCVBO::getVertexCountMaxLimit();
  return (numdata >= minv) && (numdata <= maxv);
}

SbBool
SoFCVBO::shouldRenderAsVertexArrays(SoState * state,
                                  const uint32_t contextid,
                                  const int numdata)
{
  (void)state;
  (void)contextid;
  // FIXME: consider also using results from the performance tests

  // don't render as vertex arrays if there are very few elements to
  // be rendered. The VA setup overhead would make it slower than just
  // doing plain immediate mode rendering.
  return (numdata >= vbo_vertex_count_min_limit) && vbo_render_as_vertex_arrays;
}

long
SoFCVBO::getVBOMemUsage()
{
  return vbo_mem;
}

long
SoFCVBO::getSystemMemUsage()
{
  return vbo_buf_mem;
}
// vim: noai:ts=2:sw=2
