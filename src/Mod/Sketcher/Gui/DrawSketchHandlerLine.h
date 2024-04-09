/***************************************************************************
 *   Copyright (c) 2022 Abdullah Tahiri <abdullah.tahiri.yo@gmail.com>     *
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


#ifndef SKETCHERGUI_DrawSketchHandlerLine_H
#define SKETCHERGUI_DrawSketchHandlerLine_H

#include <Gui/BitmapFactory.h>
#include <Gui/Notifications.h>
#include <Gui/Command.h>
#include <Gui/CommandT.h>

#include <Mod/Sketcher/App/SketchObject.h>

#include "DrawSketchDefaultWidgetController.h"
#include "DrawSketchControllableHandler.h"

#include "GeometryCreationMode.h"
#include "Utils.h"

namespace SketcherGui
{

extern GeometryCreationMode geometryCreationMode;  // defined in CommandCreateGeo.cpp

class DrawSketchHandlerLine;

namespace ConstructionMethods
{

enum class LineConstructionMethod
{
    OnePointLengthAngle,
    OnePointWidthHeight,
    TwoPoints,
    End  // Must be the last one
};

}

using DSHLineController =
    DrawSketchDefaultWidgetController<DrawSketchHandlerLine,
                                      /*SelectModeT*/ StateMachines::TwoSeekEnd,
                                      /*PAutoConstraintSize =*/2,
                                      /*OnViewParametersT =*/OnViewParameters<4, 4, 4>,  // NOLINT
                                      /*WidgetParametersT =*/WidgetParameters<0, 0, 0>,  // NOLINT
                                      /*WidgetCheckboxesT =*/WidgetCheckboxes<0, 0, 0>,  // NOLINT
                                      /*WidgetComboboxesT =*/WidgetComboboxes<1, 1, 1>,  // NOLINT
                                      ConstructionMethods::LineConstructionMethod,
                                      /*bool PFirstComboboxIsConstructionMethod =*/true>;

using DSHLineControllerBase = DSHLineController::ControllerBase;

using DrawSketchHandlerLineBase = DrawSketchControllableHandler<DSHLineController>;


class DrawSketchHandlerLine: public DrawSketchHandlerLineBase
{
    friend DSHLineController;
    friend DSHLineControllerBase;

public:
    explicit DrawSketchHandlerLine(
        ConstructionMethod constrMethod = ConstructionMethod::OnePointLengthAngle)
        : DrawSketchHandlerLineBase(constrMethod)
        , length(0.0) {};
    ~DrawSketchHandlerLine() override = default;

private:
    void updateDataAndDrawToPosition(Base::Vector2d onSketchPos) override
    {
        if (Mode==STATUS_SEEK_First) {
            setPositionText(onSketchPos);
            if (seekAutoConstraint(sugConstr1, onSketchPos, Base::Vector2d(0.f,0.f))) {
                renderSuggestConstraintsCursor(sugConstr1);
                return;
            }
        }
    }

    void executeCommands() override
    {
        try {
            createShape(false);

            Gui::Command::openCommand(QT_TRANSLATE_NOOP("Command", "Add sketch line"));

            commandAddShapeGeometryAndConstraints();

            Gui::Command::commitCommand();
        }
        catch (const Base::Exception&) {
            Gui::NotifyError(sketchgui,
                             QT_TRANSLATE_NOOP("Notifications", "Error"),
                             QT_TRANSLATE_NOOP("Notifications", "Failed to add line"));

            Gui::Command::abortCommand();
            THROWM(Base::RuntimeError,
                   QT_TRANSLATE_NOOP(
                       "Notifications",
                       "Tool execution aborted") "\n")  // This prevents constraints from being
                                                        // applied on non existing geometry
        }
    }

    void generateAutoConstraints() override
    {
        int LineGeoId = getHighestCurveIndex();

        // Generate temporary autoconstraints (but do not actually add them to the sketch)
        if (avoidRedundants) {
            removeRedundantHorizontalVertical(getSketchObject(),
                                              sugConstraints[0],
                                              sugConstraints[1]);
        }

        auto& ac1 = sugConstraints[0];
        auto& ac2 = sugConstraints[1];

        generateAutoConstraintsOnElement(ac1, LineGeoId, Sketcher::PointPos::start);
        generateAutoConstraintsOnElement(ac2, LineGeoId, Sketcher::PointPos::end);

        // Ensure temporary autoconstraints do not generate a redundancy and that the geometry
        // parameters are accurate This is particularly important for adding widget mandated
        // constraints.
        removeRedundantAutoConstraints();
    }

    void createAutoConstraints() override
    {

        sugConstraints[0].clear();
        sugConstraints[1].clear();
    }

    std::string getToolName() const override
    {
        return "DSH_Line";
    }

    QString getCrosshairCursorSVGName() const override
    {
        if (constructionMethod() == ConstructionMethod::OnePointLengthAngle) {
            return QString::fromLatin1("Sketcher_Pointer_Create_Line_Polar");
        }
        else {
            return QString::fromLatin1("Sketcher_Pointer_Create_Line.svg");
        }
    }

    std::unique_ptr<QWidget> createWidget() const override
    {
        return std::make_unique<SketcherToolDefaultWidget>();
    }

    bool isWidgetVisible() const override
    {
        ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
            "User parameter:BaseApp/Preferences/Mod/Sketcher/Tools");
        auto index = hGrp->GetInt("OnViewParameterVisibility", 1);
        return index != 0;
    };

    QPixmap getToolIcon() const override
    {
        return Gui::BitmapFactory().pixmap("Sketcher_CreateLine");
    }

    QString getToolWidgetText() const override
    {
        return QString(QObject::tr("Line parameters"));
    }

    bool canGoToNextMode() override
    {
        if (state() == SelectMode::SeekSecond && length < Precision::Confusion()) {
            // Prevent validation of null line.
            return false;
        }
        return true;
    }

    void angleSnappingControl() override
    {
        if (state() == SelectMode::SeekSecond) {
            setAngleSnapping(true, startPoint);
        }

        else {
            setAngleSnapping(false);
        }
    }

private:
    Base::Vector2d startPoint, endPoint;
    double length;

    void createShape(bool onlyeditoutline) override
    {
        Q_UNUSED(onlyeditoutline);
        ShapeGeometry.clear();

        Base::Vector2d vecL = endPoint - startPoint;
        length = vecL.Length();
        if (length > Precision::Confusion()) {

            addLineToShapeGeometry(toVector3d(startPoint),
                                   toVector3d(endPoint),
                                   isConstructionMode());
        }
    }
};

template<>
auto DSHLineControllerBase::getState(int labelindex) const
{
    switch (labelindex) {
        case OnViewParameter::First:
        case OnViewParameter::Second:
            return SelectMode::SeekFirst;
            break;
        case OnViewParameter::Third:
        case OnViewParameter::Fourth:
            return SelectMode::SeekSecond;
            break;
        default:
            THROWM(Base::ValueError, "Label index without an associated machine state")
    }
}

template<>
void DSHLineController::configureToolWidget()
{
    if (!init) {  // Code to be executed only upon initialisation
        QStringList names = {QStringLiteral("Point, length, angle"),
                             QStringLiteral("Point, width, height"),
                             QStringLiteral("2 points")};
        toolWidget->setComboboxElements(WCombobox::FirstCombo, names);

        if (isConstructionMode()) {
            toolWidget->setComboboxItemIcon(
                WCombobox::FirstCombo,
                0,
                Gui::BitmapFactory().iconFromTheme("Sketcher_CreateLineAngleLength_Constr"));
            toolWidget->setComboboxItemIcon(
                WCombobox::FirstCombo,
                1,
                Gui::BitmapFactory().iconFromTheme("Sketcher_CreateLineLengthWidth_Constr"));
            toolWidget->setComboboxItemIcon(
                WCombobox::FirstCombo,
                2,
                Gui::BitmapFactory().iconFromTheme("Sketcher_CreateLine_Constr"));
        }
        else {
            toolWidget->setComboboxItemIcon(
                WCombobox::FirstCombo,
                0,
                Gui::BitmapFactory().iconFromTheme("Sketcher_CreateLineAngleLength"));
            toolWidget->setComboboxItemIcon(
                WCombobox::FirstCombo,
                1,
                Gui::BitmapFactory().iconFromTheme("Sketcher_CreateLineLengthWidth"));
            toolWidget->setComboboxItemIcon(
                WCombobox::FirstCombo,
                2,
                Gui::BitmapFactory().iconFromTheme("Sketcher_CreateLine"));
        }
    }

    onViewParameters[OnViewParameter::First]->setLabelType(Gui::SoDatumLabel::DISTANCEX);
    onViewParameters[OnViewParameter::Second]->setLabelType(Gui::SoDatumLabel::DISTANCEY);

    if (handler->constructionMethod() == ConstructionMethod::OnePointLengthAngle) {
        onViewParameters[OnViewParameter::Third]->setLabelType(
            Gui::SoDatumLabel::DISTANCE,
            Gui::EditableDatumLabel::Function::Dimensioning);
        onViewParameters[OnViewParameter::Fourth]->setLabelType(
            Gui::SoDatumLabel::ANGLE,
            Gui::EditableDatumLabel::Function::Dimensioning);
    }
    else if (handler->constructionMethod() == ConstructionMethod::TwoPoints) {
        onViewParameters[OnViewParameter::Third]->setLabelType(
            Gui::SoDatumLabel::DISTANCEX,
            Gui::EditableDatumLabel::Function::Positioning);
        onViewParameters[OnViewParameter::Fourth]->setLabelType(
            Gui::SoDatumLabel::DISTANCEY,
            Gui::EditableDatumLabel::Function::Positioning);
    }
    else {
        onViewParameters[OnViewParameter::Third]->setLabelType(
            Gui::SoDatumLabel::DISTANCEX,
            Gui::EditableDatumLabel::Function::Dimensioning);
        onViewParameters[OnViewParameter::Fourth]->setLabelType(
            Gui::SoDatumLabel::DISTANCEY,
            Gui::EditableDatumLabel::Function::Dimensioning);
    }
}


template<>
void DSHLineControllerBase::doEnforceControlParameters(Base::Vector2d& onSketchPos)
{
    switch (handler->state()) {
        case SelectMode::SeekFirst: {
            if (onViewParameters[OnViewParameter::First]->isSet) {
                onSketchPos.x = onViewParameters[OnViewParameter::First]->getValue();
            }

            if (onViewParameters[OnViewParameter::Second]->isSet) {
                onSketchPos.y = onViewParameters[OnViewParameter::Second]->getValue();
            }
        } break;
        case SelectMode::SeekSecond: {
            if (handler->constructionMethod() == ConstructionMethod::OnePointWidthHeight) {
                if (onViewParameters[OnViewParameter::Third]->isSet) {
                    onSketchPos.x = handler->startPoint.x
                        + onViewParameters[OnViewParameter::Third]->getValue();
                }

                if (onViewParameters[OnViewParameter::Fourth]->isSet) {
                    onSketchPos.y = handler->startPoint.y
                        + onViewParameters[OnViewParameter::Fourth]->getValue();
                }
            }
            else if (handler->constructionMethod() == ConstructionMethod::OnePointLengthAngle) {

                Base::Vector2d dir = onSketchPos - handler->startPoint;
                if (dir.Length() < Precision::Confusion()) {
                    dir.x = 1.0;  // if direction null, default to (1,0)
                }
                double length = dir.Length();

                if (onViewParameters[OnViewParameter::Third]->isSet) {
                    length = onViewParameters[OnViewParameter::Third]->getValue();
                    if (length < Precision::Confusion()) {
                        unsetOnViewParameter(onViewParameters[OnViewParameter::Third].get());
                        return;
                    }

                    onSketchPos = handler->startPoint + length * dir.Normalize();
                }

                if (onViewParameters[OnViewParameter::Fourth]->isSet) {
                    double angle =
                        Base::toRadians(onViewParameters[OnViewParameter::Fourth]->getValue());
                    onSketchPos.x = handler->startPoint.x + cos(angle) * length;
                    onSketchPos.y = handler->startPoint.y + sin(angle) * length;
                }
            }
            else {
                if (onViewParameters[OnViewParameter::Third]->isSet) {
                    onSketchPos.x = onViewParameters[OnViewParameter::Third]->getValue();
                }
                if (onViewParameters[OnViewParameter::Fourth]->isSet) {
                    onSketchPos.y = onViewParameters[OnViewParameter::Fourth]->getValue();
                }
            }

            if (onViewParameters[OnViewParameter::Third]->isSet
                && onViewParameters[OnViewParameter::Fourth]->isSet
                && (onSketchPos - handler->startPoint).Length() < Precision::Confusion()) {
                unsetOnViewParameter(onViewParameters[OnViewParameter::Third].get());
                unsetOnViewParameter(onViewParameters[OnViewParameter::Fourth].get());
            }
        } break;
        default:
            break;
    }
}

template<>
void DSHLineController::adaptParameters(Base::Vector2d onSketchPos)
{
    switch (handler->state()) {
        case SelectMode::SeekFirst: {
            if (!onViewParameters[OnViewParameter::First]->isSet) {
                setOnViewParameterValue(OnViewParameter::First, onSketchPos.x);
            }

            if (!onViewParameters[OnViewParameter::Second]->isSet) {
                setOnViewParameterValue(OnViewParameter::Second, onSketchPos.y);
            }

            bool sameSign = onSketchPos.x * onSketchPos.y > 0.;
            onViewParameters[OnViewParameter::First]->setLabelAutoDistanceReverse(!sameSign);
            onViewParameters[OnViewParameter::Second]->setLabelAutoDistanceReverse(sameSign);
            onViewParameters[OnViewParameter::First]->setPoints(Base::Vector3d(),
                                                                toVector3d(onSketchPos));
            onViewParameters[OnViewParameter::Second]->setPoints(Base::Vector3d(),
                                                                 toVector3d(onSketchPos));
        } break;
        case SelectMode::SeekSecond: {
            if (handler->constructionMethod() == ConstructionMethod::OnePointWidthHeight) {
                Base::Vector3d start = toVector3d(handler->startPoint);
                Base::Vector3d end = toVector3d(handler->endPoint);
                Base::Vector3d vec = end - start;

                if (!onViewParameters[OnViewParameter::Third]->isSet) {
                    setOnViewParameterValue(OnViewParameter::Third, vec.x);
                }

                if (!onViewParameters[OnViewParameter::Fourth]->isSet) {
                    setOnViewParameterValue(OnViewParameter::Fourth, vec.y);
                }

                bool sameSign = vec.x * vec.y > 0.;

                onViewParameters[OnViewParameter::Third]->setLabelAutoDistanceReverse(!sameSign);
                onViewParameters[OnViewParameter::Fourth]->setLabelAutoDistanceReverse(sameSign);

                onViewParameters[OnViewParameter::Third]->setPoints(start, end);
                onViewParameters[OnViewParameter::Fourth]->setPoints(start, end);
            }
            else if (handler->constructionMethod() == ConstructionMethod::OnePointLengthAngle) {
                Base::Vector3d start = toVector3d(handler->startPoint);
                Base::Vector3d end = toVector3d(handler->endPoint);
                Base::Vector3d vec = end - start;

                if (!onViewParameters[OnViewParameter::Third]->isSet) {
                    setOnViewParameterValue(OnViewParameter::Third, vec.Length());
                }

                double range = (handler->endPoint - handler->startPoint).Angle();
                if (!onViewParameters[OnViewParameter::Fourth]->isSet) {
                    setOnViewParameterValue(OnViewParameter::Fourth,
                                            Base::toDegrees(range),
                                            Base::Unit::Angle);
                }

                onViewParameters[OnViewParameter::Third]->setPoints(start, end);
                onViewParameters[OnViewParameter::Fourth]->setPoints(start, Base::Vector3d());
                onViewParameters[OnViewParameter::Fourth]->setLabelRange(range);
            }
            else {
                if (!onViewParameters[OnViewParameter::Third]->isSet) {
                    setOnViewParameterValue(OnViewParameter::Third, onSketchPos.x);
                }

                if (!onViewParameters[OnViewParameter::Fourth]->isSet) {
                    setOnViewParameterValue(OnViewParameter::Fourth, onSketchPos.y);
                }

                bool sameSign = onSketchPos.x * onSketchPos.y > 0.;
                onViewParameters[OnViewParameter::Third]->setLabelAutoDistanceReverse(!sameSign);
                onViewParameters[OnViewParameter::Fourth]->setLabelAutoDistanceReverse(sameSign);
                onViewParameters[OnViewParameter::Third]->setPoints(Base::Vector3d(),
                                                                    toVector3d(onSketchPos));
                onViewParameters[OnViewParameter::Fourth]->setPoints(Base::Vector3d(),
                                                                     toVector3d(onSketchPos));
            }
        } break;
        default:
            break;
    }
=======
    explicit DrawSketchHandlerLine(
        ConstructionMethod constrMethod = ConstructionMethod::OnePointLengthAngle)
        : DrawSketchHandlerLineBase(constrMethod)
        , length(0.0) {};
    ~DrawSketchHandlerLine() override = default;
>>>>>>> upstream/main

template<>
void DSHLineController::doChangeDrawSketchHandlerMode()
{
    switch (handler->state()) {
        case SelectMode::SeekFirst: {
            if (onViewParameters[OnViewParameter::First]->isSet
                && onViewParameters[OnViewParameter::Second]->isSet) {

                handler->setState(SelectMode::SeekSecond);
            }
        } break;
        case SelectMode::SeekSecond: {
            if (onViewParameters[OnViewParameter::Third]->isSet
                && onViewParameters[OnViewParameter::Fourth]->isSet) {

                handler->setState(SelectMode::End);
            }
        } break;
        default:
            break;
    }
}

    bool isWidgetVisible() const override
    {
        ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath(
            "User parameter:BaseApp/Preferences/Mod/Sketcher/Tools");
        auto index = hGrp->GetInt("OnViewParameterVisibility", 1);
        return index != 0;
    };

    QPixmap getToolIcon() const override
    {
        return Gui::BitmapFactory().pixmap("Sketcher_CreateLine");
    }

    QString getToolWidgetText() const override
    {
        return QString(QObject::tr("Line parameters"));
    }

    bool canGoToNextMode() override
    {
        if (state() == SelectMode::SeekSecond && length < Precision::Confusion()) {
            // Prevent validation of null line.
            return false;
        }
        return true;
    }

    void angleSnappingControl() override
    {
        if (state() == SelectMode::SeekSecond) {
            setAngleSnapping(true, startPoint);
        }

        else {
            setAngleSnapping(false);
        }
    }

private:
    Base::Vector2d startPoint, endPoint;
    double length;

    void createShape(bool onlyeditoutline) override
    {
        Q_UNUSED(onlyeditoutline);
        ShapeGeometry.clear();

        Base::Vector2d vecL = endPoint - startPoint;
        length = vecL.Length();
        if (length > Precision::Confusion()) {

            addLineToShapeGeometry(toVector3d(startPoint),
                                   toVector3d(endPoint),
                                   isConstructionMode());
        }
    }
};


}  // namespace SketcherGui


#endif  // SKETCHERGUI_DrawSketchHandlerLine_H
