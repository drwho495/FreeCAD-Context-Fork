/***************************************************************************
 *   Copyright (c) 2010 Juergen Riegel <FreeCAD@juergen-riegel.net>        *
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
# include <BRepAlgoAPI_Fuse.hxx>
# include <BRepPrimAPI_MakeRevol.hxx>
# include <BRepFeat_MakeRevol.hxx>
# include <gp_Lin.hxx>
# include <Precision.hxx>
# include <TopExp_Explorer.hxx>
# include <TopoDS.hxx>
#endif

#include <Base/Axis.h>
#include <Base/Exception.h>
#include <Base/Placement.h>
#include <Base/Tools.h>
#include <App/Document.h>
#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/TopoShapeOpCode.h>
#include "FeatureRevolution.h"

using namespace PartDesign;

namespace PartDesign {

const char* Revolution::TypeEnums[]= {"Angle", "UpToLast", "UpToFirst", "UpToFace", "TwoAngles", nullptr};

PROPERTY_SOURCE(PartDesign::Revolution, PartDesign::ProfileBased)

const App::PropertyAngle::Constraints Revolution::floatAngle = { Base::toDegrees<double>(Precision::Angular()), 360.0, 1.0 };

Revolution::Revolution()
{
    ADD_PROPERTY_TYPE(Type, (0L), "Revolution", App::Prop_None, "Revolution type");
    Type.setEnums(TypeEnums);
    ADD_PROPERTY_TYPE(Base,(Base::Vector3d(0.0,0.0,0.0)),"Revolution", App::Prop_ReadOnly, "Base");
    ADD_PROPERTY_TYPE(Axis,(Base::Vector3d(0.0,1.0,0.0)),"Revolution", App::Prop_ReadOnly, "Axis");
    ADD_PROPERTY_TYPE(Angle,(360.0),"Revolution", App::Prop_None, "Angle");
    ADD_PROPERTY_TYPE(UpToFace, (nullptr), "Revolution", App::Prop_None, "Face where revolution will end");
    ADD_PROPERTY_TYPE(Angle2, (60.0), "Revolution", App::Prop_None, "Revolution length in 2nd direction");

    Angle.setConstraints(&floatAngle);
    ADD_PROPERTY_TYPE(ReferenceAxis,(nullptr),"Revolution",(App::Prop_None),"Reference axis of revolution");
}

short Revolution::mustExecute() const
{
    if (Placement.isTouched() ||
        ReferenceAxis.isTouched() ||
        Axis.isTouched() ||
        Base.isTouched() ||
        UpToFace.isTouched() ||
        Angle.isTouched() ||
        Angle2.isTouched())
        return 1;
    return ProfileBased::mustExecute();
}

App::DocumentObjectExecReturn *Revolution::execute()
{
    // Validate parameters
    // All angles are in radians unless explicitly stated
    double angleDeg = Angle.getValue();
    if (angleDeg > 360.0)
        return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Angle of revolution too large"));

    double angle = Base::toRadians<double>(angleDeg);
    if (angle < Precision::Angular())
        return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Angle of revolution too small"));

    double angle2 = Base::toRadians(Angle2.getValue());

    TopoShape sketchshape;
    try {
        sketchshape = getVerifiedFace();
    } catch (const Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }

    // if the Base property has a valid shape, fuse the AddShape into it
    TopoShape base;
    try {
        base = getBaseShape();
    } catch (const Base::Exception&) {
        // fall back to support (for legacy features)
    }

    // update Axis from ReferenceAxis
    try {
        updateAxis();
    } catch (const Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }

    // get revolve axis
    Base::Vector3d b = Base.getValue();
    gp_Pnt pnt(b.x,b.y,b.z);
    Base::Vector3d v = Axis.getValue();
    gp_Dir dir(v.x,v.y,v.z);

    try {
        if (sketchshape.isNull())
            return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Creating a face from sketch failed"));

       RevolMethod method = methodFromString(Type.getValueAsString());

        auto invObjLoc = this->positionByPrevious();
        pnt.Transform(invObjLoc.Transformation());
        dir.Transform(invObjLoc.Transformation());
        base.move(invObjLoc);
        sketchshape.move(invObjLoc);

        // Check distance between sketchshape and axis - to avoid failures and crashes
        TopExp_Explorer xp;
        xp.Init(sketchshape.getShape(), TopAbs_FACE);
        for (;xp.More(); xp.Next()) {
            if (checkLineCrossesFace(gp_Lin(pnt, dir), TopoDS::Face(xp.Current())))
                return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Revolve axis intersects the sketch"));
        }

        TopoShape result(0,getDocument()->getStringHasher());

        if (method == RevolMethod::ToFace || method == RevolMethod::ToFirst || method == RevolMethod::ToLast) {
            TopoShape upToFace;
            if (method == RevolMethod::ToFace) {
                getUpToFaceFromLinkSub(upToFace, UpToFace);
                upToFace.move(invObjLoc);
            }
            else
                throw Base::RuntimeError("ProfileBased: Revolution up to first/last is not yet supported");

            // TODO: This method is designed for extrusions. needs to be adapted for revolutions.
            // getUpToFace(upToFace, base, supportface, sketchshape, method, dir);

            TopoShape supportface = getSupportFace();
            if (supportface.countSubShapes(TopAbs_FACE) == 0)
                supportface = TopoShape();
            else
                supportface.move(invObjLoc);

            if (Reversed.getValue())
                dir.Reverse();

            RevolMode mode = RevolMode::None;
            generateRevolution(result, base, sketchshape, supportface, upToFace, gp_Ax1(pnt, dir), method, mode, Standard_True);
        }
        else {
            bool midplane = Midplane.getValue();
            bool reversed = Reversed.getValue();
            generateRevolution(result, sketchshape, gp_Ax1(pnt, dir), angle, angle2, midplane, reversed, method);
        }

        result = refineShapeIfActive(result);

        // eventually disable some settings that are not valid for the current method
        updateProperties(method);


        // set the additive shape property for later usage in e.g. pattern
        this->AddSubShape.setValue(result);            
        if (isRecomputePaused())
            return App::DocumentObject::StdReturn;

        if(base.isNull()) {
            result = refineShapeIfActive(result);
            Shape.setValue(getSolid(result));
            return App::DocumentObject::StdReturn;
        }

        result.Tag = -getID();

        TopoShape boolOp(0,getDocument()->getStringHasher());

        const char *maker;
        switch(getAddSubType()) {
        case Additive:
            maker = Part::OpCodes::Fuse;
            break;
        case Subtractive:
            maker = Part::OpCodes::Cut;
            break;
        case Intersecting:
            maker = Part::OpCodes::Common;
            break;
        default:
            return new App::DocumentObjectExecReturn("Unknown operation type");
        }
        try {
            this->fixShape(result);
            boolOp.makEBoolean(maker, {base,result});
        }catch(Standard_Failure &e) {
            return new App::DocumentObjectExecReturn("Failed to perform boolean operation");
        }
        boolOp = this->getSolid(boolOp);
        // lets check if the result is a solid
        if (boolOp.isNull())
            return new App::DocumentObjectExecReturn("Resulting shape is not a solid");

        boolOp = refineShapeIfActive(boolOp);
        Shape.setValue(getSolid(boolOp));

        return App::DocumentObject::StdReturn;
    }
    catch (Standard_Failure& e) {

        if (std::string(e.GetMessageString()) == "TopoDS::Face")
            return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP("Exception", "Could not create face from sketch.\n"
                "Intersecting sketch entities in a sketch are not allowed."));
        else
            return new App::DocumentObjectExecReturn(e.GetMessageString());
    }
    catch (Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }
}

bool Revolution::suggestReversed()
{
    try {
        updateAxis();
    } catch (const Base::Exception&) {
        return false;
    }

    return ProfileBased::getReversedAngle(Base.getValue(), Axis.getValue()) < 0.0;
}

void Revolution::updateAxis()
{
    App::DocumentObject *pcReferenceAxis = ReferenceAxis.getValue();
    const std::vector<std::string> &subReferenceAxis = ReferenceAxis.getSubValues();
    Base::Vector3d base;
    Base::Vector3d dir;
    getAxis(pcReferenceAxis, subReferenceAxis, base, dir, ForbiddenAxis::NotParallelWithNormal);

    Base.setValue(base.x,base.y,base.z);
    Axis.setValue(dir.x,dir.y,dir.z);
}

Revolution::RevolMethod Revolution::methodFromString(const std::string& methodStr)
{
    if (methodStr == "Angle")
        return RevolMethod::Dimension;
    if (methodStr == "UpToLast")
        return RevolMethod::ToLast;
    if (methodStr == "ThroughAll")
        return RevolMethod::ThroughAll;
    if (methodStr == "UpToFirst")
        return RevolMethod::ToFirst;
    if (methodStr == "UpToFace")
        return RevolMethod::ToFace;
    if (methodStr == "TwoAngles")
        return RevolMethod::TwoDimensions;

    throw Base::ValueError("Revolution:: No such method");
    return RevolMethod::Dimension;
}

void Revolution::generateRevolution(TopoShape& revol,
                                    const TopoShape& sketchshape,
                                    const gp_Ax1& axis,
                                    const double angle,
                                    const double angle2,
                                    const bool midplane,
                                    const bool reversed,
                                    RevolMethod method)
{
    if (method == RevolMethod::Dimension || method == RevolMethod::TwoDimensions || method == RevolMethod::ThroughAll) {
        double angleTotal = angle;
        double angleOffset = 0.;

        if (method == RevolMethod::TwoDimensions) {
            // Rotate the face by `angle2`/`angle` to get "second" angle
            angleTotal += angle2;
            angleOffset = angle2 * -1.0;
        }
        else if (midplane) {
            // Rotate the face by half the angle to get Revolution symmetric to sketch plane
            angleOffset = -angle / 2;
        }

        if (fabs(angleTotal) < Precision::Angular())
            throw Base::ValueError("Cannot create a revolution with zero angle.");

        gp_Ax1 revolAx(axis);
        if (reversed) {
            revolAx.Reverse();
        }

        TopoShape from = sketchshape;
        if (method == RevolMethod::TwoDimensions || midplane) {
            gp_Trsf mov;
            mov.SetRotation(revolAx, angleOffset);
            TopLoc_Location loc(mov);
            from.move(loc);
        }

        // revolve the face to a solid
        try {
            result.makERevolve(from, revolAx, angleTotal);
        }catch(Standard_Failure &) {
            throw Base::RuntimeError("ProfileBased: RevolMaker failed! Could not revolve the sketch!");
        }
    }
    else {
        std::stringstream str;
        str << "ProfileBased: Internal error: Unknown method for generateRevolution()";
        throw Base::RuntimeError(str.str());
    }
}

void Revolution::generateRevolution(TopoShape& revol,
                                    const TopoShape& baseshape,
                                    const TopoShape& profileshape,
                                    const TopoShape& supportface,
                                    const TopoDS_Face& uptoface,
                                    const gp_Ax1& axis,
                                    RevolMethod method,
                                    RevolMode Mode,
                                    Standard_Boolean Modify)
{
    if (method == RevolMethod::ToFirst || method == RevolMethod::ToFace || method == RevolMethod::ToLast) {
        BRepFeat_MakeRevol RevolMaker;
        TopoShape base = baseshape;
        for (const auto &face : profileshape.getSubShapes(TopAbs_FACE)) {
            RevolMaker.Init(base, TopoDS::Face(face), supportface, axis, Mode, Modify);
            RevolMaker.Perform(uptoface);
            if (!RevolMaker.IsDone())
                throw Base::RuntimeError("ProfileBased: Up to face: Could not revolve the sketch!");

            revol.makEShape(RevolMaker, {base, profileshape, supportface}, TopoShapeOpCode::Revolve);
            base = revol;

            if (Mode == RevolMode::None)
                Mode = RevolMode::FuseWithBase;
        }

        revol = base;
    }
    else {
        std::stringstream str;
        str << "ProfileBased: Internal error: Unknown method for generateRevolution()";
        throw Base::RuntimeError(str.str());
    }
}

void Revolution::updateProperties(RevolMethod method)
{
    // disable settings that are not valid on the current method
    // disable everything unless we are sure we need it
    bool isAngleEnabled = false;
    bool isAngle2Enabled = false;
    bool isMidplaneEnabled = false;
    bool isReversedEnabled = false;
    bool isUpToFaceEnabled = false;
    if (method == RevolMethod::Dimension) {
        isAngleEnabled = true;
        isMidplaneEnabled = true;
        isReversedEnabled = !Midplane.getValue();
    }
    else if (method == RevolMethod::ToLast) {
        isReversedEnabled = true;
    }
    else if (method == RevolMethod::ThroughAll) {
        isMidplaneEnabled = true;
        isReversedEnabled = !Midplane.getValue();
    }
    else if (method == RevolMethod::ToFirst) {
        isReversedEnabled = true;
    }
    else if (method == RevolMethod::ToFace) {
        isReversedEnabled = true;
        isUpToFaceEnabled = true;
    }
    else if (method == RevolMethod::TwoDimensions) {
        isAngleEnabled = true;
        isAngle2Enabled = true;
        isReversedEnabled = true;
    }

    Angle.setReadOnly(!isAngleEnabled);
    Angle2.setReadOnly(!isAngle2Enabled);
    Midplane.setReadOnly(!isMidplaneEnabled);
    Reversed.setReadOnly(!isReversedEnabled);
    UpToFace.setReadOnly(!isUpToFaceEnabled);
}

}
