/***************************************************************************
 *   Copyright (c) 2019 WandererFan <wandererfan@gmail.com>                *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"

#include <Base/Console.h>

#include "CenterLine.h"
#include "CosmeticExtension.h"
#include "CosmeticExtensionPy.h"
#include "Cosmetic.h"
#include "DrawUtil.h"
#include "DrawViewPart.h"


using namespace TechDraw;
using namespace std;

EXTENSION_PROPERTY_SOURCE(TechDraw::CosmeticExtension, App::DocumentObjectExtension)

CosmeticExtension::CosmeticExtension()
{
    static const char *cgroup = "Cosmetics";

    EXTENSION_ADD_PROPERTY_TYPE(CosmeticVertexes, (nullptr), cgroup, App::Prop_None, "CosmeticVertex Save/Restore");
    EXTENSION_ADD_PROPERTY_TYPE(CosmeticEdges, (nullptr), cgroup, App::Prop_None, "CosmeticEdge Save/Restore");
    EXTENSION_ADD_PROPERTY_TYPE(CenterLines ,(nullptr), cgroup, App::Prop_None, "Geometry format Save/Restore");
    EXTENSION_ADD_PROPERTY_TYPE(GeomFormats ,(nullptr), cgroup, App::Prop_None, "Geometry format Save/Restore");

    initExtensionType(CosmeticExtension::getExtensionClassTypeId());
}

CosmeticExtension::~CosmeticExtension()
{
}

//void CosmeticExtension::extHandleChangedPropertyName(Base::XMLReader &reader,
//                                                     const char* TypeName,
//                                                     const char* PropName)
//{
//}

//==============================================================================
//CosmeticVertex x, y are stored as unscaled, but mirrored (inverted Y) values.
//if you are creating a CV based on calculations of scaled geometry, you need to
//unscale x, y before creation.
//if you are creating a CV based on calculations of mirrored geometry, you need to
//mirror again before creation.

//returns unique CV id
//only adds cv to cvlist property.  does not add to display geometry until dvp executes.
std::string CosmeticExtension::addCosmeticVertex(Base::Vector3d pos)
{
//    Base::Console().Message("CEx::addCosmeticVertex(%s)\n",
 //                           DrawUtil::formatVector(pos).c_str());
    std::vector<CosmeticVertex*> verts = CosmeticVertexes.getValues();
    Base::Vector3d tempPos = DrawUtil::invertY(pos);
    TechDraw::CosmeticVertex* cv = new TechDraw::CosmeticVertex(tempPos);
    verts.push_back(cv);
    CosmeticVertexes.setValues(verts);
    std::string result = cv->getTagAsString();
    return result;
}

//get CV by unique id
TechDraw::CosmeticVertex* CosmeticExtension::getCosmeticVertex(std::string tagString) const
{
//    Base::Console().Message("CEx::getCosmeticVertex(%s)\n", tagString.c_str());
    const std::vector<TechDraw::CosmeticVertex*> verts = CosmeticVertexes.getValues();
    for (auto& cv: verts) {
        std::string cvTag = cv->getTagAsString();
        if (cvTag == tagString) {
            return cv;
        }
    }
    return nullptr;
}

// find the cosmetic vertex corresponding to selection name (Vertex5)
// used when selecting
TechDraw::CosmeticVertex* CosmeticExtension::getCosmeticVertexBySelection(std::string name) const
{
//    Base::Console().Message("CEx::getCVBySelection(%s)\n", name.c_str());
    App::DocumentObject* extObj = const_cast<App::DocumentObject*> (getExtendedObject());
    TechDraw::DrawViewPart* dvp = dynamic_cast<TechDraw::DrawViewPart*>(extObj);
    if (!dvp) {
        return nullptr;
    }
    int idx = DrawUtil::getIndexFromName(name);
    TechDraw::VertexPtr v = dvp->getProjVertexByIndex(idx);
    if (!v || v->getCosmeticTag().empty()) {
        return nullptr;
    }
    return getCosmeticVertex(v->getCosmeticTag());
}

//overload for index only
TechDraw::CosmeticVertex* CosmeticExtension::getCosmeticVertexBySelection(int i) const
{
//    Base::Console().Message("CEx::getCVBySelection(%d)\n", i);
    std::stringstream ss;
    ss << "Vertex" << i;
    std::string vName = ss.str();
    return getCosmeticVertexBySelection(vName);
}

void CosmeticExtension::removeCosmeticVertex(std::string delTag)
{
//    Base::Console().Message("DVP::removeCV(%s)\n", delTag.c_str());
    std::vector<CosmeticVertex*> cVerts = CosmeticVertexes.getValues();
    std::vector<CosmeticVertex*> newVerts;
    for (auto& cv: cVerts) {
        if (cv->getTagAsString() != delTag)  {
            newVerts.push_back(cv);
        }
    }
    CosmeticVertexes.setValues(newVerts);
}

void CosmeticExtension::removeCosmeticVertex(std::vector<std::string> delTags)
{
    for (auto& t: delTags) {
        removeCosmeticVertex(t);
    }
}

//********** Cosmetic Edge *****************************************************

//returns unique CE id
//only adds ce to celist property.  does not add to display geometry until dvp executes.
std::string CosmeticExtension::addCosmeticEdge(Base::Vector3d start,
                                               Base::Vector3d end)
{
//    Base::Console().Message("CEx::addCosmeticEdge(s, e)\n");
    std::vector<CosmeticEdge*> edges = CosmeticEdges.getValues();
    TechDraw::CosmeticEdge* ce = new TechDraw::CosmeticEdge(start, end);
    edges.push_back(ce);
    CosmeticEdges.setValues(edges);
    return ce->getTagAsString();
}

std::string CosmeticExtension::addCosmeticEdge(TechDraw::BaseGeomPtr bg)
{
//    Base::Console().Message("CEx::addCosmeticEdge(bg: %X)\n", bg);
    std::vector<CosmeticEdge*> edges = CosmeticEdges.getValues();
    TechDraw::CosmeticEdge* ce = new TechDraw::CosmeticEdge(bg);
    edges.push_back(ce);
    CosmeticEdges.setValues(edges);
    return ce->getTagAsString();
}

//get CE by unique id
TechDraw::CosmeticEdge* CosmeticExtension::getCosmeticEdge(std::string tagString) const
{
//    Base::Console().Message("CEx::getCosmeticEdge(%s)\n", tagString.c_str());
    const std::vector<TechDraw::CosmeticEdge*> edges = CosmeticEdges.getValues();
    for (auto& ce: edges) {
        std::string ceTag = ce->getTagAsString();
        if (ceTag == tagString) {
            return ce;
        }
    }

    // None found
    Base::Console().Message("CEx::getCosmeticEdge - CE for tag: %s not found.\n", tagString.c_str());
    return nullptr;
}

// find the cosmetic edge corresponding to selection name (Edge5)
// used when selecting
TechDraw::CosmeticEdge* CosmeticExtension::getCosmeticEdgeBySelection(std::string name) const
{
//    Base::Console().Message("CEx::getCEBySelection(%s)\n", name.c_str());
    App::DocumentObject* extObj = const_cast<App::DocumentObject*> (getExtendedObject());
    TechDraw::DrawViewPart* dvp = dynamic_cast<TechDraw::DrawViewPart*>(extObj);
    if (!dvp) {
        return nullptr;
    }
    int idx = DrawUtil::getIndexFromName(name);
    TechDraw::BaseGeomPtr base = dvp->getGeomByIndex(idx);
    if (!base || base->getCosmeticTag().empty()) {
        return nullptr;
    }

    return getCosmeticEdge(base->getCosmeticTag());
}

//overload for index only
TechDraw::CosmeticEdge* CosmeticExtension::getCosmeticEdgeBySelection(int i) const
{
//    Base::Console().Message("CEx::getCEBySelection(%d)\n", i);
    std::stringstream edgeName;
    edgeName << "Edge" << i;
    return getCosmeticEdgeBySelection(edgeName.str());
}

void CosmeticExtension::removeCosmeticEdge(std::string delTag)
{
//    Base::Console().Message("DVP::removeCE(%s)\n", delTag.c_str());
    std::vector<CosmeticEdge*> cEdges = CosmeticEdges.getValues();
    std::vector<CosmeticEdge*> newEdges;
    for (auto& ce: cEdges) {
        if (ce->getTagAsString() != delTag)  {
            newEdges.push_back(ce);
        }
    }
    CosmeticEdges.setValues(newEdges);
}

void CosmeticExtension::removeCosmeticEdge(std::vector<std::string> delTags)
{
    for (auto& t: delTags) {
        removeCosmeticEdge(t);
    }
}

//********** Center Line *******************************************************

//returns unique CL id
//only adds cl to cllist property.  does not add to display geometry until dvp executes.
std::string CosmeticExtension::addCenterLine(Base::Vector3d start,
                                               Base::Vector3d end)
{
//    Base::Console().Message("CEx::addCenterLine(%s)\n",
//                            DrawUtil::formatVector(start).c_str(),
//                            DrawUtil::formatVector(end).c_str());
    std::vector<CenterLine*> cLines = CenterLines.getValues();
    TechDraw::CenterLine* cl = new TechDraw::CenterLine(start, end);
    cLines.push_back(cl);
    CenterLines.setValues(cLines);
    return cl->getTagAsString();
}

std::string CosmeticExtension::addCenterLine(TechDraw::CenterLine* cl)
{
//    Base::Console().Message("CEx::addCenterLine(cl: %X)\n", cl);
    std::vector<CenterLine*> cLines = CenterLines.getValues();
    cLines.push_back(cl);
    CenterLines.setValues(cLines);
    return cl->getTagAsString();
}


std::string CosmeticExtension::addCenterLine(TechDraw::BaseGeomPtr bg)
{
//    Base::Console().Message("CEx::addCenterLine(bg: %X)\n", bg);
    std::vector<CenterLine*> cLines = CenterLines.getValues();
    TechDraw::CenterLine* cl = new TechDraw::CenterLine(bg);
    cLines.push_back(cl);
    CenterLines.setValues(cLines);
    return cl->getTagAsString();
}


//get CL by unique id
TechDraw::CenterLine* CosmeticExtension::getCenterLine(std::string tagString) const
{
//    Base::Console().Message("CEx::getCenterLine(%s)\n", tagString.c_str());
    const std::vector<TechDraw::CenterLine*> cLines = CenterLines.getValues();
    for (auto& cl: cLines) {
        std::string clTag = cl->getTagAsString();
        if (clTag == tagString) {
            return cl;
        }
    }
    return nullptr;
}

// find the center line corresponding to selection name (Edge5)
// used when selecting
TechDraw::CenterLine* CosmeticExtension::getCenterLineBySelection(std::string name) const
{
//    Base::Console().Message("CEx::getCLBySelection(%s)\n", name.c_str());
    App::DocumentObject* extObj = const_cast<App::DocumentObject*> (getExtendedObject());
    TechDraw::DrawViewPart* dvp = dynamic_cast<TechDraw::DrawViewPart*>(extObj);
    if (!dvp) {
        return nullptr;
    }
    int idx = DrawUtil::getIndexFromName(name);
    TechDraw::BaseGeomPtr base = dvp->getGeomByIndex(idx);
    if (!base || base->getCosmeticTag().empty()) {
        return nullptr;
    }
    return getCenterLine(base->getCosmeticTag());
}

//overload for index only
TechDraw::CenterLine* CosmeticExtension::getCenterLineBySelection(int i) const
{
//    Base::Console().Message("CEx::getCLBySelection(%d)\n", i);
    std::stringstream edgeName;
    edgeName << "Edge" << i;
    return getCenterLineBySelection(edgeName.str());
}

void CosmeticExtension::removeCenterLine(std::string delTag)
{
//    Base::Console().Message("DVP::removeCL(%s)\n", delTag.c_str());
    std::vector<CenterLine*> cLines = CenterLines.getValues();
    std::vector<CenterLine*> newLines;
    for (auto& cl: cLines) {
        if (cl->getTagAsString() != delTag)  {
            newLines.push_back(cl);
        }
    }
    CenterLines.setValues(newLines);
}

void CosmeticExtension::removeCenterLine(std::vector<std::string> delTags)
{
    for (auto& t: delTags) {
        removeCenterLine(t);
    }
}


//********** Geometry Formats **************************************************
//returns unique GF id
//only adds gf to gflist property.  does not add to display geometry until dvp repaints.
std::string CosmeticExtension::addGeomFormat(TechDraw::GeomFormat* gf)
{
//    Base::Console().Message("CEx::addGeomFormat(gf: %X)\n", gf);
    std::vector<GeomFormat*> formats = GeomFormats.getValues();
    TechDraw::GeomFormat* newGF = new TechDraw::GeomFormat(gf);
    formats.push_back(newGF);
    GeomFormats.setValues(formats);
    return newGF->getTagAsString();
}


//get GF by unique id
TechDraw::GeomFormat* CosmeticExtension::getGeomFormat(std::string tagString) const
{
//    Base::Console().Message("CEx::getGeomFormat(%s)\n", tagString.c_str());
    const std::vector<TechDraw::GeomFormat*> formats = GeomFormats.getValues();
    for (auto& gf: formats) {
        std::string gfTag = gf->getTagAsString();
        if (gfTag == tagString) {
            return gf;
        }
    }

    // Nothing found
    return nullptr;
}

// find the cosmetic edge corresponding to selection name (Edge5)
// used when selecting
TechDraw::GeomFormat* CosmeticExtension::getGeomFormatBySelection(std::string name) const
{
//    Base::Console().Message("CEx::getCEBySelection(%s)\n", name.c_str());
    App::DocumentObject* extObj = const_cast<App::DocumentObject*> (getExtendedObject());
    TechDraw::DrawViewPart* dvp = dynamic_cast<TechDraw::DrawViewPart*>(extObj);
    if (!dvp) {
        return nullptr;
    }
    int idx = DrawUtil::getIndexFromName(name);
    const std::vector<TechDraw::GeomFormat*> formats = GeomFormats.getValues();
    for (auto& gf: formats) {
        if (gf->m_geomIndex == idx) {
            return gf;
        }
    }

    // Nothing found
    return nullptr;
}

//overload for index only
TechDraw::GeomFormat* CosmeticExtension::getGeomFormatBySelection(int i) const
{
//    Base::Console().Message("CEx::getCEBySelection(%d)\n", i);
    std::stringstream edgeName;
    edgeName << "Edge" << i;
    return getGeomFormatBySelection(edgeName.str());
}

void CosmeticExtension::removeGeomFormat(std::string delTag)
{
//    Base::Console().Message("DVP::removeCE(%s)\n", delTag.c_str());
    std::vector<GeomFormat*> cFormats = GeomFormats.getValues();
    std::vector<GeomFormat*> newFormats;
    for (auto& gf: cFormats) {
        if (gf->getTagAsString() != delTag)  {
            newFormats.push_back(gf);
        }
    }
    GeomFormats.setValues(newFormats);
}

//================================================================================
PyObject* CosmeticExtension::getExtensionPyObject(void) {
    if (ExtensionPythonObject.is(Py::_None())){
        // ref counter is set to 1
        ExtensionPythonObject = Py::Object(new CosmeticExtensionPy(this), true);
    }
    return Py::new_reference_to(ExtensionPythonObject);
}

namespace App {
/// @cond DOXERR
  EXTENSION_PROPERTY_SOURCE_TEMPLATE(TechDraw::CosmeticExtensionPython, TechDraw::CosmeticExtension)
/// @endcond

// explicit template instantiation
  template class TechDrawExport ExtensionPythonT<TechDraw::CosmeticExtension>;
}


