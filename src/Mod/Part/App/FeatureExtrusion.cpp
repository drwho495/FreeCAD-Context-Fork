/***************************************************************************
 *   Copyright (c) 2008 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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
#ifndef _PreComp_
# include <BRepAdaptor_Curve.hxx>
# include <BRepAdaptor_Surface.hxx>
# include <BRepBuilderAPI_Copy.hxx>
# include <BRepBuilderAPI_MakeWire.hxx>
# include <BRepLib_FindSurface.hxx>
# include <BRepPrimAPI_MakePrism.hxx>
# include <gp_Pln.hxx>
# include <gp_Trsf.hxx>
# include <Precision.hxx>
# include <TopExp.hxx>
# include <TopExp_Explorer.hxx>
# include <TopoDS.hxx>
# include <TopTools_IndexedMapOfShape.hxx>
# include <BRepLib_FindSurface.hxx>
# include <BRepTools.hxx>
#endif


#include <Base/Tools.h>
#include <Base/Exception.h>
#include <App/Application.h>
#include <App/Document.h>

#include "FeatureExtrusion.h"
#include "ExtrusionHelper.h"
#include "Part2DObject.h"
#include "PartParams.h"
#include "TopoShapeOpCode.h"


using namespace Part;

PROPERTY_SOURCE(Part::Extrusion, Part::Feature)

const char* Extrusion::eDirModeStrings[] = {
    "Custom",
    "Edge",
    "Normal",
    nullptr };

Extrusion::Extrusion()
{
    ADD_PROPERTY_TYPE(Base, (nullptr), "Extrude", App::Prop_None, "Shape to extrude");
    ADD_PROPERTY_TYPE(Dir, (Base::Vector3d(0.0, 0.0, 1.0)), "Extrude", App::Prop_None, "Direction of extrusion (also magnitude, if both lengths are zero).");
    ADD_PROPERTY_TYPE(DirMode, (dmCustom), "Extrude", App::Prop_None, "Sets, how Dir is updated.");
    DirMode.setEnums(eDirModeStrings);
    ADD_PROPERTY_TYPE(DirLink,(nullptr), "Extrude", App::Prop_None, "Link to edge defining extrusion direction.");
    ADD_PROPERTY_TYPE(LengthFwd,(0.0), "Extrude", App::Prop_None, "Length of extrusion along direction. If both LengthFwd and LengthRev are zero, magnitude of Dir is used.");
    ADD_PROPERTY_TYPE(LengthRev,(0.0), "Extrude", App::Prop_None, "Length of additional extrusion, against direction.");
    ADD_PROPERTY_TYPE(Solid,(false), "Extrude", App::Prop_None, "If true, extruding a wire yields a solid. If false, a shell.");
    ADD_PROPERTY_TYPE(Reversed,(false), "Extrude", App::Prop_None, "Set to true to swap the direction of extrusion.");
    ADD_PROPERTY_TYPE(Symmetric,(false), "Extrude", App::Prop_None, "If true, extrusion is done in both directions to a total of LengthFwd. LengthRev is ignored.");
    ADD_PROPERTY_TYPE(TaperAngle,(0.0), "Extrude", App::Prop_None, "Sets the angle of slope (draft) to apply to the sides. The angle is for outward taper; negative value yields inward tapering.");
    ADD_PROPERTY_TYPE(TaperAngleRev,(0.0), "Extrude", App::Prop_None, "Taper angle of reverse part of extrusion.");
    ADD_PROPERTY_TYPE(TaperInnerAngle,(0.0), "Extrude", App::Prop_None, "Taper angle of inner holes.");
    ADD_PROPERTY_TYPE(TaperInnerAngleRev,(0.0), "Extrude", App::Prop_None, "Taper angle of the reverse part for inner holes.");
    ADD_PROPERTY_TYPE(AutoTaperInnerAngle,(true), "Extrude", App::Prop_None,
            "Automatically set inner taper angle to the negative of (outer) taper angle.\n"
            "If false, then inner taper angle can be set independent of taper angle.");
    ADD_PROPERTY_TYPE(UsePipeForDraft,(false), "Extrude", App::Prop_None, "Use pipe (i.e. sweep) operation to create draft angles.");
    ADD_PROPERTY_TYPE(Linearize,(false), "Extrude", App::Prop_None,
            "Linearize the resut shape by simplify linear edge and planar face into line and plane");
    ADD_PROPERTY_TYPE(FaceMakerClass,("Part::FaceMakerExtrusion"), "Extrude", App::Prop_None, "If Solid is true, this sets the facemaker class to use when converting wires to faces. Otherwise, ignored."); //default for old documents. See setupObject for default for new extrusions.
}

short Extrusion::mustExecute() const
{
    if (Base.isTouched() ||
        Dir.isTouched() ||
        DirMode.isTouched() ||
        DirLink.isTouched() ||
        LengthFwd.isTouched() ||
        LengthRev.isTouched() ||
        Solid.isTouched() ||
        Reversed.isTouched() ||
        Symmetric.isTouched() ||
        TaperAngle.isTouched() ||
        TaperAngleRev.isTouched() ||
        TaperInnerAngle.isTouched() ||
        TaperInnerAngleRev.isTouched() ||
        AutoTaperInnerAngle.isTouched() ||
        FaceMakerClass.isTouched())
        return 1;
    return 0;
}

void Extrusion::handleChangedPropertyName(Base::XMLReader &reader, const char * TypeName, const char *Name)
{
    if (strcmp(TypeName, App::PropertyAngle::getClassTypeId().getName()) == 0) {
        // Deliberately change 'InnerTaperAngle' to TaperAngleInner to identify
        // document from Link branch
        if (strcmp(Name, "InnerTaperAngle")) {
            AutoTaperInnerAngle.setValue(false);
            TaperInnerAngle.Restore(reader);
        } else if (strcmp(Name, "InnerTaperAngleRev")) {
            AutoTaperInnerAngle.setValue(false);
            TaperInnerAngleRev.Restore(reader);
        }
    }
    Feature::handleChangedPropertyName(reader, TypeName, Name);
}

void Extrusion::onChanged(const App::Property *prop)
{
    if (prop == &TaperAngle
            || prop == &TaperAngleRev
            || prop == &AutoTaperInnerAngle)
    {
        if (AutoTaperInnerAngle.getValue()) {
            TaperAngleRev.setStatus(App::Property::ReadOnly, true);
            TaperAngle.setStatus(App::Property::ReadOnly, true);
            TaperInnerAngle.setValue(-TaperAngle.getValue());
            TaperInnerAngleRev.setValue(-TaperAngleRev.getValue());
        }
        else {
            TaperAngleRev.setStatus(App::Property::ReadOnly, false);
            TaperAngle.setStatus(App::Property::ReadOnly, false);
        }
    }
    Feature::onChanged(prop);
}

bool Extrusion::fetchAxisLink(const App::PropertyLinkSub& axisLink, Base::Vector3d& basepoint, Base::Vector3d& dir)
{
    if (!axisLink.getValue())
        return false;

    TopoDS_Shape axEdge;
    if (!axisLink.getSubValues().empty() && axisLink.getSubValues()[0].length() > 0) {
        axEdge = Feature::getShape(axisLink.getValue(),axisLink.getSubValues()[0].c_str(), /*need element*/true);
    } else {
        axEdge = Feature::getShape(axisLink.getValue());
    }

    if (axEdge.IsNull())
        throw Base::ValueError("DirLink shape is null");
    if (axEdge.ShapeType() != TopAbs_EDGE)
        throw Base::TypeError("DirLink shape is not an edge");

    BRepAdaptor_Curve crv(TopoDS::Edge(axEdge));
    gp_Pnt startpoint;
    gp_Pnt endpoint;
    if (crv.GetType() == GeomAbs_Line) {
        startpoint = crv.Value(crv.FirstParameter());
        endpoint = crv.Value(crv.LastParameter());
        if (axEdge.Orientation() == TopAbs_REVERSED)
            std::swap(startpoint, endpoint);
    }
    else {
        throw Base::TypeError("DirLink edge is not a line.");
    }
    basepoint.Set(startpoint.X(), startpoint.Y(), startpoint.Z());
    gp_Vec vec = gp_Vec(startpoint, endpoint);
    dir.Set(vec.X(), vec.Y(), vec.Z());
    return true;
}

ExtrusionHelper::Parameters Extrusion::computeFinalParameters()
{
    ExtrusionHelper::Parameters result;
    result.usepipe = this->UsePipeForDraft.getValue();
    result.linearize = this->Linearize.getValue();
    Base::Vector3d dir;
    switch (this->DirMode.getValue()) {
    case dmCustom:
        dir = this->Dir.getValue();
        break;
    case dmEdge: {
        bool fetched;
        Base::Vector3d base;
        fetched = fetchAxisLink(this->DirLink, base, dir);
        if (!fetched)
            throw Base::ValueError("DirMode is set to use edge, but no edge is linked.");
        this->Dir.setValue(dir);
    } break;
    case dmNormal:
        dir = calculateShapeNormal(this->Base);
        this->Dir.setValue(dir);
        break;
    default:
        throw Base::ValueError("Unexpected enum value");
    }
    if (dir.Length() < Precision::Confusion())
        throw Base::ValueError("Direction is zero-length");
    result.dir = gp_Dir(dir.x, dir.y, dir.z);
    if (this->Reversed.getValue())
        result.dir.Reverse();

    result.lengthFwd = this->LengthFwd.getValue();
    result.lengthRev = this->LengthRev.getValue();
    if (fabs(result.lengthFwd) < Precision::Confusion()
        && fabs(result.lengthRev) < Precision::Confusion()) {
        result.lengthFwd = dir.Length();
    }

    if (this->Symmetric.getValue()) {
        result.lengthRev = result.lengthFwd * 0.5;
        result.lengthFwd = result.lengthFwd * 0.5;
    }

    if (fabs(result.lengthFwd + result.lengthRev) < Precision::Confusion())
        throw Base::ValueError("Total length of extrusion is zero.");

    result.solid = this->Solid.getValue();

    result.taperAngleFwd = this->TaperAngle.getValue() * M_PI / 180.0;
    if (fabs(result.taperAngleFwd) > M_PI * 0.5 - Precision::Angular())
        throw Base::ValueError("Magnitude of taper angle matches or exceeds 90 degrees. That is too much.");
    result.taperAngleRev = this->TaperAngleRev.getValue() * M_PI / 180.0;
    if (fabs(result.taperAngleRev) > M_PI * 0.5 - Precision::Angular())
        throw Base::ValueError("Magnitude of taper angle matches or exceeds 90 degrees. That is too much.");
    result.innerTaperAngleFwd = this->TaperInnerAngle.getValue() * M_PI / 180.0;
    if (fabs(result.innerTaperAngleFwd) > M_PI * 0.5 - Precision::Angular() )
        throw Base::ValueError("Magnitude of inner taper angle matches or exceeds 90 degrees. That is too much.");
    result.innerTaperAngleRev = this->TaperInnerAngleRev.getValue() * M_PI / 180.0;
    if (fabs(result.innerTaperAngleRev) > M_PI * 0.5 - Precision::Angular() )
        throw Base::ValueError("Magnitude of inner taper angle matches or exceeds 90 degrees. That is too much.");

    result.faceMakerClass = this->FaceMakerClass.getValue();

    return result;
}

Base::Vector3d Extrusion::calculateShapeNormal(const App::PropertyLink& shapeLink)
{
    App::DocumentObject* docobj = nullptr;
    Base::Matrix4D mat;
    TopoDS_Shape sh = Feature::getShape(shapeLink.getValue(), nullptr, false, &mat, &docobj);

    if (!docobj)
        throw Base::ValueError("calculateShapeNormal: link is empty");

    //special case for sketches and the like: no matter what shape they have, use their local Z axis.
    if (docobj->isDerivedFrom(Part::Part2DObject::getClassTypeId())) {
        Base::Vector3d OZ(0.0, 0.0, 1.0);
        Base::Vector3d result;
        Base::Rotation(mat).multVec(OZ, result);
        return result;
    }

    if (sh.IsNull())
        throw NullShapeException("calculateShapeNormal: link points to a valid object, but its shape is null.");

    //find plane
    BRepLib_FindSurface planeFinder(sh, -1, /*OnlyPlane=*/true);
    if (!planeFinder.Found())
        throw Base::ValueError("Can't find normal direction, because the shape is not on a plane.");

    //find plane normal and return result.
    GeomAdaptor_Surface surf(planeFinder.Surface());
    gp_Dir normal = surf.Plane().Axis().Direction();

    //now we know the plane. But if there are faces, the
    //plane normal direction is not dependent on face orientation (because findPlane only uses edges).
    //let's fix that.
    TopExp_Explorer ex(sh, TopAbs_FACE);
    if (ex.More()) {
        BRepAdaptor_Surface surf(TopoDS::Face(ex.Current()));
        if (surf.GetType() == GeomAbs_Plane) { // could be BSplineSurface, which will throw
            normal = surf.Plane().Axis().Direction();
            if (ex.Current().Orientation() == TopAbs_REVERSED){
                normal.Reverse();
            }
        }
    }

    return Base::Vector3d(normal.X(), normal.Y(), normal.Z());
}

void Extrusion::extrudeShape(TopoShape &result,
                             const TopoShape &source,
                             const ExtrusionHelper::Parameters& params)
{
    gp_Vec vec = gp_Vec(params.dir).Multiplied(params.lengthFwd + params.lengthRev);//total vector of extrusion

    if (std::fabs(params.taperAngleFwd) >= Precision::Angular() ||
        std::fabs(params.taperAngleRev) >= Precision::Angular()) {
        //Tapered extrusion!
#if defined(__GNUC__) && defined (FC_OS_LINUX)
        Base::SignalException se;
#endif
        if (source.isNull())
            Standard_Failure::Raise("Cannot extrude empty shape");
        // #0000910: Circles Extrude Only Surfaces, thus use BRepBuilderAPI_Copy
        TopoShape myShape(source.makECopy());

        std::vector<TopoShape> drafts;
        ExtrusionHelper::makeDraft(params, myShape, drafts, result.Hasher);
        if (drafts.empty()) {
            Standard_Failure::Raise("Drafting shape failed");
        }else
            result.makECompound(drafts,0,false);
    }
    else {
        //Regular (non-tapered) extrusion!
        if (source.isNull())
            Standard_Failure::Raise("Cannot extrude empty shape");

        // #0000910: Circles Extrude Only Surfaces, thus use BRepBuilderAPI_Copy
        TopoShape myShape(source.makECopy());

        //apply reverse part of extrusion by shifting the source shape
        if (fabs(params.lengthRev) > Precision::Confusion()) {
            gp_Trsf mov;
            mov.SetTranslation(gp_Vec(params.dir) * (-params.lengthRev));
            myShape = myShape.makETransform(mov);
        }

        //make faces from wires
        if (params.solid) {
            //test if we need to make faces from wires. If there are faces - we don't.
            if(!myShape.hasSubShape(TopAbs_FACE)) {
                if(!myShape.Hasher)
                    myShape.Hasher = result.Hasher;
                myShape = myShape.makEFace(0,params.faceMakerClass.c_str());
            }
        }

        //extrude!
        result.makEPrism(myShape,vec);
    }

    if (result.isNull())
        throw NullShapeException("Result of extrusion is null shape.");
}

App::DocumentObjectExecReturn* Extrusion::execute()
{
    App::DocumentObject* link = Base.getValue();
    if (!link)
        return new App::DocumentObjectExecReturn("No object linked");

    try {
        ExtrusionHelper::Parameters params = computeFinalParameters();
        TopoShape result(0,getDocument()->getStringHasher());
        extrudeShape(result,Feature::getTopoShape(link),params);
        this->Shape.setValue(result);
        return Part::Feature::execute();
    }
    catch (Standard_Failure& e) {
        return new App::DocumentObjectExecReturn(e.GetMessageString());
    }
}

//----------------------------------------------------------------

TYPESYSTEM_SOURCE(Part::FaceMakerExtrusion, Part::FaceMakerCheese)

std::string FaceMakerExtrusion::getUserFriendlyName() const
{
    return std::string(QT_TRANSLATE_NOOP("Part_FaceMaker", "Part Extrude facemaker"));
}

std::string FaceMakerExtrusion::getBriefExplanation() const
{
    return std::string(QT_TRANSLATE_NOOP("Part_FaceMaker", "Supports making faces with holes, does not support nesting."));
}

#if OCC_VERSION_HEX >= 0x070600
void FaceMakerExtrusion::Build(const Message_ProgressRange&)
#else
void FaceMakerExtrusion::Build()
#endif
{
    this->NotDone();
    this->myGenerated.Clear();
    this->myShapesToReturn.clear();
    this->myShape = TopoDS_Shape();
    TopoDS_Shape inputShape;
    if (mySourceShapes.empty())
        throw Base::ValueError("No input shapes!");
    if (mySourceShapes.size() == 1){
        inputShape = mySourceShapes[0].getShape();
    } else {
        TopoDS_Builder builder;
        TopoDS_Compound cmp;
        builder.MakeCompound(cmp);
        for (auto& sh: mySourceShapes){
            builder.Add(cmp, sh.getShape());
        }
        inputShape = cmp;
    }

    std::vector<TopoDS_Wire> wires;
    TopTools_IndexedMapOfShape mapOfWires;
    TopExp::MapShapes(inputShape, TopAbs_WIRE, mapOfWires);

    // if there are no wires then check also for edges
    if (mapOfWires.IsEmpty()) {
        TopTools_IndexedMapOfShape mapOfEdges;
        TopExp::MapShapes(inputShape, TopAbs_EDGE, mapOfEdges);
        for (int i = 1; i <= mapOfEdges.Extent(); i++) {
            BRepBuilderAPI_MakeWire mkWire(TopoDS::Edge(mapOfEdges.FindKey(i)));
            wires.push_back(mkWire.Wire());
        }
    }
    else {
        wires.reserve(mapOfWires.Extent());
        for (int i = 1; i <= mapOfWires.Extent(); i++) {
            wires.push_back(TopoDS::Wire(mapOfWires.FindKey(i)));
        }
    }

    if (!wires.empty()) {
        //try {
        TopoDS_Shape res = FaceMakerCheese::makeFace(wires);
        if (!res.IsNull())
            this->myShape = res;
        //}
        //catch (...) {

        //}
    }

    this->postBuild();

}

void Part::Extrusion::setupObject()
{
    Part::Feature::setupObject();
    UsePipeForDraft.setValue(PartParams::getUsePipeForExtrusionDraft());
    Linearize.setValue(Part::PartParams::getLinearizeExtrusionDraft());
    this->FaceMakerClass.setValue("Part::FaceMakerBullseye"); //default for newly created features
}
