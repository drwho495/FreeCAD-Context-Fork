/***************************************************************************
 *   Copyright (c) 2017 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#ifndef PART_PYCXX_H
#define PART_PYCXX_H

#include <CXX/Extensions.hxx>

#include <Mod/Part/App/TopoShapePy.h>

namespace App {
class DocumentObjectT;
}

namespace Py {
    using TopoShape = ExtensionObject<Part::TopoShapePy>;
    template<>
    bool TopoShape::accepts (PyObject *pyob) const;
}

namespace Part {
    PartExport Py::Object shape2pyshape(const TopoShape &shape);
    PartExport Py::Object shape2pyshape(const TopoDS_Shape &shape);
    PartExport void getPyShapes(PyObject *obj, std::vector<TopoShape> &shapes, std::vector<App::DocumentObjectT> *objs = nullptr);
    PartExport std::vector<TopoShape> getPyShapes(PyObject *obj, std::vector<App::DocumentObjectT> *objs = nullptr);
    PartExport int initOCCTExtension();
}

#endif //PART_PYCXX_H
