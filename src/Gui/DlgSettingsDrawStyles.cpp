/****************************************************************************
 *   Copyright (c) 2020 Zheng Lei (realthunder) <realthunder.dev@gmail.com> *
 *                                                                          *
 *   This file is part of the FreeCAD CAx development system.               *
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Library General Public            *
 *   License as published by the Free Software Foundation; either           *
 *   version 2 of the License, or (at your option) any later version.       *
 *                                                                          *
 *   This library  is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Library General Public License for more details.                   *
 *                                                                          *
 *   You should have received a copy of the GNU Library General Public      *
 *   License along with this library; see the file COPYING.LIB. If not,     *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,          *
 *   Suite 330, Boston, MA  02111-1307, USA                                 *
 *                                                                          *
 ****************************************************************************/

#include "PreCompiled.h"

#include <App/Material.h>

/*[[[cog
import DlgSettingsDrawStyles
DlgSettingsDrawStyles.define()
]]]*/

// Auto generated code (Tools/params_utils.py:446)
#ifndef _PreComp_
#   include <QApplication>
#   include <QLabel>
#   include <QGroupBox>
#   include <QGridLayout>
#   include <QVBoxLayout>
#   include <QHBoxLayout>
#endif
#include <Gui/PrefWidgets.h>
#include <Gui/ViewParams.h>
// Auto generated code (Tools/params_utils.py:464)
#include "DlgSettingsDrawStyles.h"
using namespace Gui::Dialog;
/* TRANSLATOR Gui::Dialog::DlgSettingsDrawStyles */

// Auto generated code (Tools/params_utils.py:471)
DlgSettingsDrawStyles::DlgSettingsDrawStyles(QWidget* parent)
    : PreferencePage( parent )
{

    auto layout = new QVBoxLayout(this);


    // Auto generated code (Tools/params_utils.py:313)
    groupDocument = new QGroupBox(this);
    layout->addWidget(groupDocument);
    auto layoutHorizDocument = new QHBoxLayout(groupDocument);
    auto layoutDocument = new QGridLayout();
    layoutHorizDocument->addLayout(layoutDocument);
    layoutHorizDocument->addStretch();

    // Auto generated code (Tools/params_utils.py:324)
    labelDefaultDrawStyle = new QLabel(this);
    layoutDocument->addWidget(labelDefaultDrawStyle, 0, 0);
    comboBoxDefaultDrawStyle = new Gui::PrefComboBox(this);
    layoutDocument->addWidget(comboBoxDefaultDrawStyle, 0, 1);
    for (int i=0; i<9; ++i)
        comboBoxDefaultDrawStyle->addItem(QString());
    comboBoxDefaultDrawStyle->setCurrentIndex(Gui::ViewParams::defaultDefaultDrawStyle());
    comboBoxDefaultDrawStyle->setEntryName("DefaultDrawStyle");
    comboBoxDefaultDrawStyle->setParamGrpPath("View");


    // Auto generated code (Tools/params_utils.py:313)
    groupSelection = new QGroupBox(this);
    layout->addWidget(groupSelection);
    auto layoutHorizSelection = new QHBoxLayout(groupSelection);
    auto layoutSelection = new QGridLayout();
    layoutHorizSelection->addLayout(layoutSelection);
    layoutHorizSelection->addStretch();

    // Auto generated code (Tools/params_utils.py:324)
    labelTransparencyOnTop = new QLabel(this);
    layoutSelection->addWidget(labelTransparencyOnTop, 0, 0);
    spinBoxTransparencyOnTop = new Gui::PrefDoubleSpinBox(this);
    layoutSelection->addWidget(spinBoxTransparencyOnTop, 0, 1);
    spinBoxTransparencyOnTop->setValue(Gui::ViewParams::defaultTransparencyOnTop());
    spinBoxTransparencyOnTop->setEntryName("TransparencyOnTop");
    spinBoxTransparencyOnTop->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelSelectionLineThicken = new QLabel(this);
    layoutSelection->addWidget(labelSelectionLineThicken, 1, 0);
    spinBoxSelectionLineThicken = new Gui::PrefDoubleSpinBox(this);
    layoutSelection->addWidget(spinBoxSelectionLineThicken, 1, 1);
    spinBoxSelectionLineThicken->setValue(Gui::ViewParams::defaultSelectionLineThicken());
    spinBoxSelectionLineThicken->setEntryName("SelectionLineThicken");
    spinBoxSelectionLineThicken->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelSelectionLineMaxWidth = new QLabel(this);
    layoutSelection->addWidget(labelSelectionLineMaxWidth, 2, 0);
    spinBoxSelectionLineMaxWidth = new Gui::PrefDoubleSpinBox(this);
    layoutSelection->addWidget(spinBoxSelectionLineMaxWidth, 2, 1);
    spinBoxSelectionLineMaxWidth->setValue(Gui::ViewParams::defaultSelectionLineMaxWidth());
    spinBoxSelectionLineMaxWidth->setEntryName("SelectionLineMaxWidth");
    spinBoxSelectionLineMaxWidth->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelSelectionPointScale = new QLabel(this);
    layoutSelection->addWidget(labelSelectionPointScale, 3, 0);
    spinBoxSelectionPointScale = new Gui::PrefDoubleSpinBox(this);
    layoutSelection->addWidget(spinBoxSelectionPointScale, 3, 1);
    spinBoxSelectionPointScale->setValue(Gui::ViewParams::defaultSelectionPointScale());
    spinBoxSelectionPointScale->setEntryName("SelectionPointScale");
    spinBoxSelectionPointScale->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelSelectionPointMaxSize = new QLabel(this);
    layoutSelection->addWidget(labelSelectionPointMaxSize, 4, 0);
    spinBoxSelectionPointMaxSize = new Gui::PrefDoubleSpinBox(this);
    layoutSelection->addWidget(spinBoxSelectionPointMaxSize, 4, 1);
    spinBoxSelectionPointMaxSize->setValue(Gui::ViewParams::defaultSelectionPointMaxSize());
    spinBoxSelectionPointMaxSize->setEntryName("SelectionPointMaxSize");
    spinBoxSelectionPointMaxSize->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelSelectionLinePattern = new QLabel(this);
    layoutSelection->addWidget(labelSelectionLinePattern, 5, 0);
    linePatternSelectionLinePattern = new Gui::PrefLinePattern(this);
    layoutSelection->addWidget(linePatternSelectionLinePattern, 5, 1);
    for (int i=1; i<linePatternSelectionLinePattern->count(); ++i) {
        if (linePatternSelectionLinePattern->itemData(i).toInt() == 0)
            linePatternSelectionLinePattern->setCurrentIndex(i);
    }
    linePatternSelectionLinePattern->setEntryName("SelectionLinePattern");
    linePatternSelectionLinePattern->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelSelectionLinePatternScale = new QLabel(this);
    layoutSelection->addWidget(labelSelectionLinePatternScale, 6, 0);
    spinBoxSelectionLinePatternScale = new Gui::PrefSpinBox(this);
    layoutSelection->addWidget(spinBoxSelectionLinePatternScale, 6, 1);
    spinBoxSelectionLinePatternScale->setValue(Gui::ViewParams::defaultSelectionLinePatternScale());
    spinBoxSelectionLinePatternScale->setEntryName("SelectionLinePatternScale");
    spinBoxSelectionLinePatternScale->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelSelectionHiddenLineWidth = new QLabel(this);
    layoutSelection->addWidget(labelSelectionHiddenLineWidth, 7, 0);
    spinBoxSelectionHiddenLineWidth = new Gui::PrefDoubleSpinBox(this);
    layoutSelection->addWidget(spinBoxSelectionHiddenLineWidth, 7, 1);
    spinBoxSelectionHiddenLineWidth->setValue(Gui::ViewParams::defaultSelectionHiddenLineWidth());
    spinBoxSelectionHiddenLineWidth->setEntryName("SelectionHiddenLineWidth");
    spinBoxSelectionHiddenLineWidth->setParamGrpPath("View");


    // Auto generated code (Tools/params_utils.py:313)
    groupHiddenLines = new QGroupBox(this);
    layout->addWidget(groupHiddenLines);
    auto layoutHorizHiddenLines = new QHBoxLayout(groupHiddenLines);
    auto layoutHiddenLines = new QGridLayout();
    layoutHorizHiddenLines->addLayout(layoutHiddenLines);
    layoutHorizHiddenLines->addStretch();

    // Auto generated code (Tools/params_utils.py:324)
    colorButtonHiddenLineFaceColor = new Gui::PrefColorButton(this);
    layoutHiddenLines->addWidget(colorButtonHiddenLineFaceColor, 0, 1);
    colorButtonHiddenLineFaceColor->setPackedColor(Gui::ViewParams::defaultHiddenLineFaceColor());
    checkBoxHiddenLineOverrideFaceColor = new Gui::PrefCheckBox(this);
    layoutHiddenLines->addWidget(checkBoxHiddenLineOverrideFaceColor, 0, 0);
    checkBoxHiddenLineOverrideFaceColor->setChecked(Gui::ViewParams::defaultHiddenLineOverrideFaceColor());
    colorButtonHiddenLineFaceColor->setEnabled(checkBoxHiddenLineOverrideFaceColor->isChecked());
    connect(checkBoxHiddenLineOverrideFaceColor, SIGNAL(toggled(bool)), colorButtonHiddenLineFaceColor, SLOT(setEnabled(bool)));
    colorButtonHiddenLineFaceColor->setEntryName("HiddenLineFaceColor");
    colorButtonHiddenLineFaceColor->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    colorButtonHiddenLineColor = new Gui::PrefColorButton(this);
    layoutHiddenLines->addWidget(colorButtonHiddenLineColor, 1, 1);
    colorButtonHiddenLineColor->setPackedColor(Gui::ViewParams::defaultHiddenLineColor());
    checkBoxHiddenLineOverrideColor = new Gui::PrefCheckBox(this);
    layoutHiddenLines->addWidget(checkBoxHiddenLineOverrideColor, 1, 0);
    checkBoxHiddenLineOverrideColor->setChecked(Gui::ViewParams::defaultHiddenLineOverrideColor());
    colorButtonHiddenLineColor->setEnabled(checkBoxHiddenLineOverrideColor->isChecked());
    connect(checkBoxHiddenLineOverrideColor, SIGNAL(toggled(bool)), colorButtonHiddenLineColor, SLOT(setEnabled(bool)));
    colorButtonHiddenLineColor->setEntryName("HiddenLineColor");
    colorButtonHiddenLineColor->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    colorButtonHiddenLineBackground = new Gui::PrefColorButton(this);
    layoutHiddenLines->addWidget(colorButtonHiddenLineBackground, 2, 1);
    colorButtonHiddenLineBackground->setPackedColor(Gui::ViewParams::defaultHiddenLineBackground());
    checkBoxHiddenLineOverrideBackground = new Gui::PrefCheckBox(this);
    layoutHiddenLines->addWidget(checkBoxHiddenLineOverrideBackground, 2, 0);
    checkBoxHiddenLineOverrideBackground->setChecked(Gui::ViewParams::defaultHiddenLineOverrideBackground());
    colorButtonHiddenLineBackground->setEnabled(checkBoxHiddenLineOverrideBackground->isChecked());
    connect(checkBoxHiddenLineOverrideBackground, SIGNAL(toggled(bool)), colorButtonHiddenLineBackground, SLOT(setEnabled(bool)));
    colorButtonHiddenLineBackground->setEntryName("HiddenLineBackground");
    colorButtonHiddenLineBackground->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxHiddenLineShaded = new Gui::PrefCheckBox(this);
    layoutHiddenLines->addWidget(checkBoxHiddenLineShaded, 3, 0);
    checkBoxHiddenLineShaded->setChecked(Gui::ViewParams::defaultHiddenLineShaded());
    checkBoxHiddenLineShaded->setEntryName("HiddenLineShaded");
    checkBoxHiddenLineShaded->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxHiddenLineShowOutline = new Gui::PrefCheckBox(this);
    layoutHiddenLines->addWidget(checkBoxHiddenLineShowOutline, 4, 0);
    checkBoxHiddenLineShowOutline->setChecked(Gui::ViewParams::defaultHiddenLineShowOutline());
    checkBoxHiddenLineShowOutline->setEntryName("HiddenLineShowOutline");
    checkBoxHiddenLineShowOutline->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxHiddenLinePerFaceOutline = new Gui::PrefCheckBox(this);
    layoutHiddenLines->addWidget(checkBoxHiddenLinePerFaceOutline, 5, 0);
    checkBoxHiddenLinePerFaceOutline->setChecked(Gui::ViewParams::defaultHiddenLinePerFaceOutline());
    checkBoxHiddenLinePerFaceOutline->setEntryName("HiddenLinePerFaceOutline");
    checkBoxHiddenLinePerFaceOutline->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxHiddenLineHideFace = new Gui::PrefCheckBox(this);
    layoutHiddenLines->addWidget(checkBoxHiddenLineHideFace, 6, 0);
    checkBoxHiddenLineHideFace->setChecked(Gui::ViewParams::defaultHiddenLineHideFace());
    checkBoxHiddenLineHideFace->setEntryName("HiddenLineHideFace");
    checkBoxHiddenLineHideFace->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxHiddenLineHideSeam = new Gui::PrefCheckBox(this);
    layoutHiddenLines->addWidget(checkBoxHiddenLineHideSeam, 7, 0);
    checkBoxHiddenLineHideSeam->setChecked(Gui::ViewParams::defaultHiddenLineHideSeam());
    checkBoxHiddenLineHideSeam->setEntryName("HiddenLineHideSeam");
    checkBoxHiddenLineHideSeam->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxHiddenLineHideVertex = new Gui::PrefCheckBox(this);
    layoutHiddenLines->addWidget(checkBoxHiddenLineHideVertex, 8, 0);
    checkBoxHiddenLineHideVertex->setChecked(Gui::ViewParams::defaultHiddenLineHideVertex());
    checkBoxHiddenLineHideVertex->setEntryName("HiddenLineHideVertex");
    checkBoxHiddenLineHideVertex->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    spinBoxHiddenLineTransparency = new Gui::PrefDoubleSpinBox(this);
    layoutHiddenLines->addWidget(spinBoxHiddenLineTransparency, 9, 1);
    spinBoxHiddenLineTransparency->setValue(Gui::ViewParams::defaultHiddenLineTransparency());
    checkBoxHiddenLineOverrideTransparency = new Gui::PrefCheckBox(this);
    layoutHiddenLines->addWidget(checkBoxHiddenLineOverrideTransparency, 9, 0);
    checkBoxHiddenLineOverrideTransparency->setChecked(Gui::ViewParams::defaultHiddenLineOverrideTransparency());
    spinBoxHiddenLineTransparency->setEnabled(checkBoxHiddenLineOverrideTransparency->isChecked());
    connect(checkBoxHiddenLineOverrideTransparency, SIGNAL(toggled(bool)), spinBoxHiddenLineTransparency, SLOT(setEnabled(bool)));
    spinBoxHiddenLineTransparency->setEntryName("HiddenLineTransparency");
    spinBoxHiddenLineTransparency->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelHiddenLineWidth = new QLabel(this);
    layoutHiddenLines->addWidget(labelHiddenLineWidth, 10, 0);
    spinBoxHiddenLineWidth = new Gui::PrefDoubleSpinBox(this);
    layoutHiddenLines->addWidget(spinBoxHiddenLineWidth, 10, 1);
    spinBoxHiddenLineWidth->setValue(Gui::ViewParams::defaultHiddenLineWidth());
    spinBoxHiddenLineWidth->setEntryName("HiddenLineWidth");
    spinBoxHiddenLineWidth->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelHiddenLinePointSize = new QLabel(this);
    layoutHiddenLines->addWidget(labelHiddenLinePointSize, 11, 0);
    spinBoxHiddenLinePointSize = new Gui::PrefDoubleSpinBox(this);
    layoutHiddenLines->addWidget(spinBoxHiddenLinePointSize, 11, 1);
    spinBoxHiddenLinePointSize->setValue(Gui::ViewParams::defaultHiddenLinePointSize());
    spinBoxHiddenLinePointSize->setEntryName("HiddenLinePointSize");
    spinBoxHiddenLinePointSize->setParamGrpPath("View");


    // Auto generated code (Tools/params_utils.py:313)
    groupShadow = new QGroupBox(this);
    layout->addWidget(groupShadow);
    auto layoutHorizShadow = new QHBoxLayout(groupShadow);
    auto layoutShadow = new QGridLayout();
    layoutHorizShadow->addLayout(layoutShadow);
    layoutHorizShadow->addStretch();

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxShadowSpotLight = new Gui::PrefCheckBox(this);
    layoutShadow->addWidget(checkBoxShadowSpotLight, 0, 0);
    checkBoxShadowSpotLight->setChecked(Gui::ViewParams::defaultShadowSpotLight());
    checkBoxShadowSpotLight->setEntryName("ShadowSpotLight");
    checkBoxShadowSpotLight->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowLightColor = new QLabel(this);
    layoutShadow->addWidget(labelShadowLightColor, 1, 0);
    colorButtonShadowLightColor = new Gui::PrefColorButton(this);
    layoutShadow->addWidget(colorButtonShadowLightColor, 1, 1);
    colorButtonShadowLightColor->setPackedColor(Gui::ViewParams::defaultShadowLightColor());
    colorButtonShadowLightColor->setEntryName("ShadowLightColor");
    colorButtonShadowLightColor->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowLightIntensity = new QLabel(this);
    layoutShadow->addWidget(labelShadowLightIntensity, 2, 0);
    spinBoxShadowLightIntensity = new Gui::PrefDoubleSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowLightIntensity, 2, 1);
    spinBoxShadowLightIntensity->setValue(Gui::ViewParams::defaultShadowLightIntensity());
    spinBoxShadowLightIntensity->setEntryName("ShadowLightIntensity");
    spinBoxShadowLightIntensity->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxShadowShowGround = new Gui::PrefCheckBox(this);
    layoutShadow->addWidget(checkBoxShadowShowGround, 3, 0);
    checkBoxShadowShowGround->setChecked(Gui::ViewParams::defaultShadowShowGround());
    checkBoxShadowShowGround->setEntryName("ShadowShowGround");
    checkBoxShadowShowGround->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxShadowGroundBackFaceCull = new Gui::PrefCheckBox(this);
    layoutShadow->addWidget(checkBoxShadowGroundBackFaceCull, 4, 0);
    checkBoxShadowGroundBackFaceCull->setChecked(Gui::ViewParams::defaultShadowGroundBackFaceCull());
    checkBoxShadowGroundBackFaceCull->setEntryName("ShadowGroundBackFaceCull");
    checkBoxShadowGroundBackFaceCull->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowGroundColor = new QLabel(this);
    layoutShadow->addWidget(labelShadowGroundColor, 5, 0);
    colorButtonShadowGroundColor = new Gui::PrefColorButton(this);
    layoutShadow->addWidget(colorButtonShadowGroundColor, 5, 1);
    colorButtonShadowGroundColor->setPackedColor(Gui::ViewParams::defaultShadowGroundColor());
    colorButtonShadowGroundColor->setEntryName("ShadowGroundColor");
    colorButtonShadowGroundColor->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowGroundScale = new QLabel(this);
    layoutShadow->addWidget(labelShadowGroundScale, 6, 0);
    spinBoxShadowGroundScale = new Gui::PrefDoubleSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowGroundScale, 6, 1);
    spinBoxShadowGroundScale->setValue(Gui::ViewParams::defaultShadowGroundScale());
    spinBoxShadowGroundScale->setEntryName("ShadowGroundScale");
    spinBoxShadowGroundScale->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowGroundTransparency = new QLabel(this);
    layoutShadow->addWidget(labelShadowGroundTransparency, 7, 0);
    spinBoxShadowGroundTransparency = new Gui::PrefDoubleSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowGroundTransparency, 7, 1);
    spinBoxShadowGroundTransparency->setValue(Gui::ViewParams::defaultShadowGroundTransparency());
    spinBoxShadowGroundTransparency->setEntryName("ShadowGroundTransparency");
    spinBoxShadowGroundTransparency->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowGroundTexture = new QLabel(this);
    layoutShadow->addWidget(labelShadowGroundTexture, 8, 0);
    fileChooserShadowGroundTexture = new Gui::PrefFileChooser(this);
    layoutShadow->addWidget(fileChooserShadowGroundTexture, 8, 1);
    fileChooserShadowGroundTexture->setFileNameStd(Gui::ViewParams::defaultShadowGroundTexture());
    fileChooserShadowGroundTexture->setEntryName("ShadowGroundTexture");
    fileChooserShadowGroundTexture->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowGroundTextureSize = new QLabel(this);
    layoutShadow->addWidget(labelShadowGroundTextureSize, 9, 0);
    spinBoxShadowGroundTextureSize = new Gui::PrefDoubleSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowGroundTextureSize, 9, 1);
    spinBoxShadowGroundTextureSize->setValue(Gui::ViewParams::defaultShadowGroundTextureSize());
    spinBoxShadowGroundTextureSize->setEntryName("ShadowGroundTextureSize");
    spinBoxShadowGroundTextureSize->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowGroundBumpMap = new QLabel(this);
    layoutShadow->addWidget(labelShadowGroundBumpMap, 10, 0);
    fileChooserShadowGroundBumpMap = new Gui::PrefFileChooser(this);
    layoutShadow->addWidget(fileChooserShadowGroundBumpMap, 10, 1);
    fileChooserShadowGroundBumpMap->setFileNameStd(Gui::ViewParams::defaultShadowGroundBumpMap());
    fileChooserShadowGroundBumpMap->setEntryName("ShadowGroundBumpMap");
    fileChooserShadowGroundBumpMap->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxShadowGroundShading = new Gui::PrefCheckBox(this);
    layoutShadow->addWidget(checkBoxShadowGroundShading, 11, 0);
    checkBoxShadowGroundShading->setChecked(Gui::ViewParams::defaultShadowGroundShading());
    checkBoxShadowGroundShading->setEntryName("ShadowGroundShading");
    checkBoxShadowGroundShading->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxShadowUpdateGround = new Gui::PrefCheckBox(this);
    layoutShadow->addWidget(checkBoxShadowUpdateGround, 12, 0);
    checkBoxShadowUpdateGround->setChecked(Gui::ViewParams::defaultShadowUpdateGround());
    checkBoxShadowUpdateGround->setEntryName("ShadowUpdateGround");
    checkBoxShadowUpdateGround->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowDisplayMode = new QLabel(this);
    layoutShadow->addWidget(labelShadowDisplayMode, 13, 0);
    comboBoxShadowDisplayMode = new Gui::PrefComboBox(this);
    layoutShadow->addWidget(comboBoxShadowDisplayMode, 13, 1);
    for (int i=0; i<3; ++i)
        comboBoxShadowDisplayMode->addItem(QString());
    comboBoxShadowDisplayMode->setCurrentIndex(Gui::ViewParams::defaultShadowDisplayMode());
    comboBoxShadowDisplayMode->setEntryName("ShadowDisplayMode");
    comboBoxShadowDisplayMode->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowPrecision = new QLabel(this);
    layoutShadow->addWidget(labelShadowPrecision, 14, 0);
    spinBoxShadowPrecision = new Gui::PrefDoubleSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowPrecision, 14, 1);
    spinBoxShadowPrecision->setValue(Gui::ViewParams::defaultShadowPrecision());
    spinBoxShadowPrecision->setEntryName("ShadowPrecision");
    spinBoxShadowPrecision->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowSmoothBorder = new QLabel(this);
    layoutShadow->addWidget(labelShadowSmoothBorder, 15, 0);
    spinBoxShadowSmoothBorder = new Gui::PrefSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowSmoothBorder, 15, 1);
    spinBoxShadowSmoothBorder->setValue(Gui::ViewParams::defaultShadowSmoothBorder());
    spinBoxShadowSmoothBorder->setEntryName("ShadowSmoothBorder");
    spinBoxShadowSmoothBorder->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowSpreadSize = new QLabel(this);
    layoutShadow->addWidget(labelShadowSpreadSize, 16, 0);
    spinBoxShadowSpreadSize = new Gui::PrefSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowSpreadSize, 16, 1);
    spinBoxShadowSpreadSize->setValue(Gui::ViewParams::defaultShadowSpreadSize());
    spinBoxShadowSpreadSize->setEntryName("ShadowSpreadSize");
    spinBoxShadowSpreadSize->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowSpreadSampleSize = new QLabel(this);
    layoutShadow->addWidget(labelShadowSpreadSampleSize, 17, 0);
    spinBoxShadowSpreadSampleSize = new Gui::PrefSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowSpreadSampleSize, 17, 1);
    spinBoxShadowSpreadSampleSize->setValue(Gui::ViewParams::defaultShadowSpreadSampleSize());
    spinBoxShadowSpreadSampleSize->setEntryName("ShadowSpreadSampleSize");
    spinBoxShadowSpreadSampleSize->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowEpsilon = new QLabel(this);
    layoutShadow->addWidget(labelShadowEpsilon, 18, 0);
    spinBoxShadowEpsilon = new Gui::PrefDoubleSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowEpsilon, 18, 1);
    spinBoxShadowEpsilon->setValue(Gui::ViewParams::defaultShadowEpsilon());
    spinBoxShadowEpsilon->setEntryName("ShadowEpsilon");
    spinBoxShadowEpsilon->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowThreshold = new QLabel(this);
    layoutShadow->addWidget(labelShadowThreshold, 19, 0);
    spinBoxShadowThreshold = new Gui::PrefDoubleSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowThreshold, 19, 1);
    spinBoxShadowThreshold->setValue(Gui::ViewParams::defaultShadowThreshold());
    spinBoxShadowThreshold->setEntryName("ShadowThreshold");
    spinBoxShadowThreshold->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowBoundBoxScale = new QLabel(this);
    layoutShadow->addWidget(labelShadowBoundBoxScale, 20, 0);
    spinBoxShadowBoundBoxScale = new Gui::PrefDoubleSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowBoundBoxScale, 20, 1);
    spinBoxShadowBoundBoxScale->setValue(Gui::ViewParams::defaultShadowBoundBoxScale());
    spinBoxShadowBoundBoxScale->setEntryName("ShadowBoundBoxScale");
    spinBoxShadowBoundBoxScale->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    labelShadowMaxDistance = new QLabel(this);
    layoutShadow->addWidget(labelShadowMaxDistance, 21, 0);
    spinBoxShadowMaxDistance = new Gui::PrefDoubleSpinBox(this);
    layoutShadow->addWidget(spinBoxShadowMaxDistance, 21, 1);
    spinBoxShadowMaxDistance->setValue(Gui::ViewParams::defaultShadowMaxDistance());
    spinBoxShadowMaxDistance->setEntryName("ShadowMaxDistance");
    spinBoxShadowMaxDistance->setParamGrpPath("View");

    // Auto generated code (Tools/params_utils.py:324)
    checkBoxShadowTransparentShadow = new Gui::PrefCheckBox(this);
    layoutShadow->addWidget(checkBoxShadowTransparentShadow, 22, 0);
    checkBoxShadowTransparentShadow->setChecked(Gui::ViewParams::defaultShadowTransparentShadow());
    checkBoxShadowTransparentShadow->setEntryName("ShadowTransparentShadow");
    checkBoxShadowTransparentShadow->setParamGrpPath("View");
    layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Expanding));
    retranslateUi();
}

// Auto generated code (Tools/params_utils.py:481)
#include "DlgSettingsDrawStyles.h"
DlgSettingsDrawStyles::~DlgSettingsDrawStyles()
{
}

void DlgSettingsDrawStyles::saveSettings()
{
    // Auto generated code (Tools/params_utils.py:490)
    // Auto generated code (Tools/params_utils.py:356)
    comboBoxDefaultDrawStyle->onSave();
    spinBoxTransparencyOnTop->onSave();
    spinBoxSelectionLineThicken->onSave();
    spinBoxSelectionLineMaxWidth->onSave();
    spinBoxSelectionPointScale->onSave();
    spinBoxSelectionPointMaxSize->onSave();
    linePatternSelectionLinePattern->onSave();
    spinBoxSelectionLinePatternScale->onSave();
    spinBoxSelectionHiddenLineWidth->onSave();
    colorButtonHiddenLineFaceColor->onSave();
    colorButtonHiddenLineColor->onSave();
    colorButtonHiddenLineBackground->onSave();
    checkBoxHiddenLineShaded->onSave();
    checkBoxHiddenLineShowOutline->onSave();
    checkBoxHiddenLinePerFaceOutline->onSave();
    checkBoxHiddenLineHideFace->onSave();
    checkBoxHiddenLineHideSeam->onSave();
    checkBoxHiddenLineHideVertex->onSave();
    spinBoxHiddenLineTransparency->onSave();
    spinBoxHiddenLineWidth->onSave();
    spinBoxHiddenLinePointSize->onSave();
    checkBoxShadowSpotLight->onSave();
    colorButtonShadowLightColor->onSave();
    spinBoxShadowLightIntensity->onSave();
    checkBoxShadowShowGround->onSave();
    checkBoxShadowGroundBackFaceCull->onSave();
    colorButtonShadowGroundColor->onSave();
    spinBoxShadowGroundScale->onSave();
    spinBoxShadowGroundTransparency->onSave();
    fileChooserShadowGroundTexture->onSave();
    spinBoxShadowGroundTextureSize->onSave();
    fileChooserShadowGroundBumpMap->onSave();
    checkBoxShadowGroundShading->onSave();
    checkBoxShadowUpdateGround->onSave();
    comboBoxShadowDisplayMode->onSave();
    spinBoxShadowPrecision->onSave();
    spinBoxShadowSmoothBorder->onSave();
    spinBoxShadowSpreadSize->onSave();
    spinBoxShadowSpreadSampleSize->onSave();
    spinBoxShadowEpsilon->onSave();
    spinBoxShadowThreshold->onSave();
    spinBoxShadowBoundBoxScale->onSave();
    spinBoxShadowMaxDistance->onSave();
    checkBoxShadowTransparentShadow->onSave();
}

void DlgSettingsDrawStyles::loadSettings()
{
    // Auto generated code (Tools/params_utils.py:497)
    // Auto generated code (Tools/params_utils.py:346)
    comboBoxDefaultDrawStyle->onRestore();
    spinBoxTransparencyOnTop->onRestore();
    spinBoxSelectionLineThicken->onRestore();
    spinBoxSelectionLineMaxWidth->onRestore();
    spinBoxSelectionPointScale->onRestore();
    spinBoxSelectionPointMaxSize->onRestore();
    linePatternSelectionLinePattern->onRestore();
    spinBoxSelectionLinePatternScale->onRestore();
    spinBoxSelectionHiddenLineWidth->onRestore();
    colorButtonHiddenLineFaceColor->onRestore();
    colorButtonHiddenLineColor->onRestore();
    colorButtonHiddenLineBackground->onRestore();
    checkBoxHiddenLineShaded->onRestore();
    checkBoxHiddenLineShowOutline->onRestore();
    checkBoxHiddenLinePerFaceOutline->onRestore();
    checkBoxHiddenLineHideFace->onRestore();
    checkBoxHiddenLineHideSeam->onRestore();
    checkBoxHiddenLineHideVertex->onRestore();
    spinBoxHiddenLineTransparency->onRestore();
    spinBoxHiddenLineWidth->onRestore();
    spinBoxHiddenLinePointSize->onRestore();
    checkBoxShadowSpotLight->onRestore();
    colorButtonShadowLightColor->onRestore();
    spinBoxShadowLightIntensity->onRestore();
    checkBoxShadowShowGround->onRestore();
    checkBoxShadowGroundBackFaceCull->onRestore();
    colorButtonShadowGroundColor->onRestore();
    spinBoxShadowGroundScale->onRestore();
    spinBoxShadowGroundTransparency->onRestore();
    fileChooserShadowGroundTexture->onRestore();
    spinBoxShadowGroundTextureSize->onRestore();
    fileChooserShadowGroundBumpMap->onRestore();
    checkBoxShadowGroundShading->onRestore();
    checkBoxShadowUpdateGround->onRestore();
    comboBoxShadowDisplayMode->onRestore();
    spinBoxShadowPrecision->onRestore();
    spinBoxShadowSmoothBorder->onRestore();
    spinBoxShadowSpreadSize->onRestore();
    spinBoxShadowSpreadSampleSize->onRestore();
    spinBoxShadowEpsilon->onRestore();
    spinBoxShadowThreshold->onRestore();
    spinBoxShadowBoundBoxScale->onRestore();
    spinBoxShadowMaxDistance->onRestore();
    checkBoxShadowTransparentShadow->onRestore();
}

void DlgSettingsDrawStyles::retranslateUi()
{
    // Auto generated code (Tools/params_utils.py:504)
    setWindowTitle(QObject::tr("Draw styles"));
    groupDocument->setTitle(QObject::tr("Document"));
    comboBoxDefaultDrawStyle->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docDefaultDrawStyle()));
    labelDefaultDrawStyle->setText(QObject::tr("Default draw style"));
    labelDefaultDrawStyle->setToolTip(comboBoxDefaultDrawStyle->toolTip());
    comboBoxDefaultDrawStyle->setItemText(0, QObject::tr("As Is"));
    comboBoxDefaultDrawStyle->setItemData(0, QObject::tr("Draw style, normal display mode"), Qt::ToolTipRole);
    comboBoxDefaultDrawStyle->setItemText(1, QObject::tr("Points"));
    comboBoxDefaultDrawStyle->setItemData(1, QObject::tr("Draw style, show points only"), Qt::ToolTipRole);
    comboBoxDefaultDrawStyle->setItemText(2, QObject::tr("Wireframe"));
    comboBoxDefaultDrawStyle->setItemData(2, QObject::tr("Draw style, show wire frame only"), Qt::ToolTipRole);
    comboBoxDefaultDrawStyle->setItemText(3, QObject::tr("Hidden Line"));
    comboBoxDefaultDrawStyle->setItemData(3, QObject::tr("Draw style, show hidden line by display object as transparent"), Qt::ToolTipRole);
    comboBoxDefaultDrawStyle->setItemText(4, QObject::tr("No Shading"));
    comboBoxDefaultDrawStyle->setItemData(4, QObject::tr("Draw style, shading forced off"), Qt::ToolTipRole);
    comboBoxDefaultDrawStyle->setItemText(5, QObject::tr("Shaded"));
    comboBoxDefaultDrawStyle->setItemData(5, QObject::tr("Draw style, shading force on"), Qt::ToolTipRole);
    comboBoxDefaultDrawStyle->setItemText(6, QObject::tr("Flat Lines"));
    comboBoxDefaultDrawStyle->setItemData(6, QObject::tr("Draw style, show both wire frame and face with shading"), Qt::ToolTipRole);
    comboBoxDefaultDrawStyle->setItemText(7, QObject::tr("Tessellation"));
    comboBoxDefaultDrawStyle->setItemData(7, QObject::tr("Draw style, show tessellation wire frame"), Qt::ToolTipRole);
    comboBoxDefaultDrawStyle->setItemText(8, QObject::tr("Shadow"));
    comboBoxDefaultDrawStyle->setItemData(8, QObject::tr("Draw style, drop shadows for the scene.\nClick this button while in shadow mode to toggle light manipulator"), Qt::ToolTipRole);
    groupSelection->setTitle(QObject::tr("Selection"));
    spinBoxTransparencyOnTop->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docTransparencyOnTop()));
    labelTransparencyOnTop->setText(QObject::tr("Transparency"));
    labelTransparencyOnTop->setToolTip(spinBoxTransparencyOnTop->toolTip());
    spinBoxSelectionLineThicken->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docSelectionLineThicken()));
    labelSelectionLineThicken->setText(QObject::tr("Line width multiplier"));
    labelSelectionLineThicken->setToolTip(spinBoxSelectionLineThicken->toolTip());
    spinBoxSelectionLineMaxWidth->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docSelectionLineMaxWidth()));
    labelSelectionLineMaxWidth->setText(QObject::tr("Maximum line width"));
    labelSelectionLineMaxWidth->setToolTip(spinBoxSelectionLineMaxWidth->toolTip());
    spinBoxSelectionPointScale->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docSelectionPointScale()));
    labelSelectionPointScale->setText(QObject::tr("Point size multiplier"));
    labelSelectionPointScale->setToolTip(spinBoxSelectionPointScale->toolTip());
    spinBoxSelectionPointMaxSize->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docSelectionPointMaxSize()));
    labelSelectionPointMaxSize->setText(QObject::tr("Maximum point size"));
    labelSelectionPointMaxSize->setToolTip(spinBoxSelectionPointMaxSize->toolTip());
    linePatternSelectionLinePattern->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docSelectionLinePattern()));
    labelSelectionLinePattern->setText(QObject::tr("Selected hidden line pattern"));
    labelSelectionLinePattern->setToolTip(linePatternSelectionLinePattern->toolTip());
    spinBoxSelectionLinePatternScale->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docSelectionLinePatternScale()));
    labelSelectionLinePatternScale->setText(QObject::tr("Selected line pattern scale"));
    labelSelectionLinePatternScale->setToolTip(spinBoxSelectionLinePatternScale->toolTip());
    spinBoxSelectionHiddenLineWidth->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docSelectionHiddenLineWidth()));
    labelSelectionHiddenLineWidth->setText(QObject::tr("Selected hidden line width"));
    labelSelectionHiddenLineWidth->setToolTip(spinBoxSelectionHiddenLineWidth->toolTip());
    groupHiddenLines->setTitle(QObject::tr("Hidden Lines"));
    colorButtonHiddenLineFaceColor->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineFaceColor()));
    checkBoxHiddenLineOverrideFaceColor->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineOverrideFaceColor()));
    checkBoxHiddenLineOverrideFaceColor->setText(QObject::tr("Override face color"));
    colorButtonHiddenLineColor->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineColor()));
    checkBoxHiddenLineOverrideColor->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineOverrideColor()));
    checkBoxHiddenLineOverrideColor->setText(QObject::tr("Override line color"));
    colorButtonHiddenLineBackground->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineBackground()));
    checkBoxHiddenLineOverrideBackground->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineOverrideBackground()));
    checkBoxHiddenLineOverrideBackground->setText(QObject::tr("Override background color"));
    checkBoxHiddenLineShaded->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineShaded()));
    checkBoxHiddenLineShaded->setText(QObject::tr("Shaded"));
    checkBoxHiddenLineShowOutline->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineShowOutline()));
    checkBoxHiddenLineShowOutline->setText(QObject::tr("Draw outline"));
    checkBoxHiddenLinePerFaceOutline->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLinePerFaceOutline()));
    checkBoxHiddenLinePerFaceOutline->setText(QObject::tr("Draw per face outline"));
    checkBoxHiddenLineHideFace->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineHideFace()));
    checkBoxHiddenLineHideFace->setText(QObject::tr("Hide face"));
    checkBoxHiddenLineHideSeam->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineHideSeam()));
    checkBoxHiddenLineHideSeam->setText(QObject::tr("Hide seam edge"));
    checkBoxHiddenLineHideVertex->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineHideVertex()));
    checkBoxHiddenLineHideVertex->setText(QObject::tr("Hide vertex"));
    spinBoxHiddenLineTransparency->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineTransparency()));
    checkBoxHiddenLineOverrideTransparency->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineOverrideTransparency()));
    checkBoxHiddenLineOverrideTransparency->setText(QObject::tr("Override transparency"));
    spinBoxHiddenLineWidth->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLineWidth()));
    labelHiddenLineWidth->setText(QObject::tr("Line width"));
    labelHiddenLineWidth->setToolTip(spinBoxHiddenLineWidth->toolTip());
    spinBoxHiddenLinePointSize->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docHiddenLinePointSize()));
    labelHiddenLinePointSize->setText(QObject::tr("Point size"));
    labelHiddenLinePointSize->setToolTip(spinBoxHiddenLinePointSize->toolTip());
    groupShadow->setTitle(QObject::tr("Shadow"));
    checkBoxShadowSpotLight->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowSpotLight()));
    checkBoxShadowSpotLight->setText(QObject::tr("Use spot light"));
    colorButtonShadowLightColor->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowLightColor()));
    labelShadowLightColor->setText(QObject::tr("Light color"));
    labelShadowLightColor->setToolTip(colorButtonShadowLightColor->toolTip());
    spinBoxShadowLightIntensity->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowLightIntensity()));
    labelShadowLightIntensity->setText(QObject::tr("Light intensity"));
    labelShadowLightIntensity->setToolTip(spinBoxShadowLightIntensity->toolTip());
    checkBoxShadowShowGround->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowShowGround()));
    checkBoxShadowShowGround->setText(QObject::tr("Show ground"));
    checkBoxShadowGroundBackFaceCull->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowGroundBackFaceCull()));
    checkBoxShadowGroundBackFaceCull->setText(QObject::tr("Ground back face culling"));
    colorButtonShadowGroundColor->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowGroundColor()));
    labelShadowGroundColor->setText(QObject::tr("Ground color"));
    labelShadowGroundColor->setToolTip(colorButtonShadowGroundColor->toolTip());
    spinBoxShadowGroundScale->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowGroundScale()));
    labelShadowGroundScale->setText(QObject::tr("Ground scale"));
    labelShadowGroundScale->setToolTip(spinBoxShadowGroundScale->toolTip());
    spinBoxShadowGroundTransparency->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowGroundTransparency()));
    labelShadowGroundTransparency->setText(QObject::tr("Ground transparency"));
    labelShadowGroundTransparency->setToolTip(spinBoxShadowGroundTransparency->toolTip());
    fileChooserShadowGroundTexture->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowGroundTexture()));
    labelShadowGroundTexture->setText(QObject::tr("Ground texture"));
    labelShadowGroundTexture->setToolTip(fileChooserShadowGroundTexture->toolTip());
    spinBoxShadowGroundTextureSize->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowGroundTextureSize()));
    labelShadowGroundTextureSize->setText(QObject::tr("Ground texture size"));
    labelShadowGroundTextureSize->setToolTip(spinBoxShadowGroundTextureSize->toolTip());
    fileChooserShadowGroundBumpMap->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowGroundBumpMap()));
    labelShadowGroundBumpMap->setText(QObject::tr("Ground bump map"));
    labelShadowGroundBumpMap->setToolTip(fileChooserShadowGroundBumpMap->toolTip());
    checkBoxShadowGroundShading->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowGroundShading()));
    checkBoxShadowGroundShading->setText(QObject::tr("Ground shading"));
    checkBoxShadowUpdateGround->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowUpdateGround()));
    checkBoxShadowUpdateGround->setText(QObject::tr("Update ground on scene change"));
    comboBoxShadowDisplayMode->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowDisplayMode()));
    labelShadowDisplayMode->setText(QObject::tr("Override display mode"));
    labelShadowDisplayMode->setToolTip(comboBoxShadowDisplayMode->toolTip());
    comboBoxShadowDisplayMode->setItemText(0, QObject::tr("Flat Lines"));
    comboBoxShadowDisplayMode->setItemText(1, QObject::tr("Shaded"));
    comboBoxShadowDisplayMode->setItemText(2, QObject::tr("As Is"));
    spinBoxShadowPrecision->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowPrecision()));
    labelShadowPrecision->setText(QObject::tr("Precision"));
    labelShadowPrecision->setToolTip(spinBoxShadowPrecision->toolTip());
    spinBoxShadowSmoothBorder->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowSmoothBorder()));
    labelShadowSmoothBorder->setText(QObject::tr("Smooth border"));
    labelShadowSmoothBorder->setToolTip(spinBoxShadowSmoothBorder->toolTip());
    spinBoxShadowSpreadSize->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowSpreadSize()));
    labelShadowSpreadSize->setText(QObject::tr("Spread size"));
    labelShadowSpreadSize->setToolTip(spinBoxShadowSpreadSize->toolTip());
    spinBoxShadowSpreadSampleSize->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowSpreadSampleSize()));
    labelShadowSpreadSampleSize->setText(QObject::tr("Spread sample size"));
    labelShadowSpreadSampleSize->setToolTip(spinBoxShadowSpreadSampleSize->toolTip());
    spinBoxShadowEpsilon->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowEpsilon()));
    labelShadowEpsilon->setText(QObject::tr("Epsilon"));
    labelShadowEpsilon->setToolTip(spinBoxShadowEpsilon->toolTip());
    spinBoxShadowThreshold->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowThreshold()));
    labelShadowThreshold->setText(QObject::tr("Threshold"));
    labelShadowThreshold->setToolTip(spinBoxShadowThreshold->toolTip());
    spinBoxShadowBoundBoxScale->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowBoundBoxScale()));
    labelShadowBoundBoxScale->setText(QObject::tr("Bounding box scale"));
    labelShadowBoundBoxScale->setToolTip(spinBoxShadowBoundBoxScale->toolTip());
    spinBoxShadowMaxDistance->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowMaxDistance()));
    labelShadowMaxDistance->setText(QObject::tr("Maximum distance"));
    labelShadowMaxDistance->setToolTip(spinBoxShadowMaxDistance->toolTip());
    checkBoxShadowTransparentShadow->setToolTip(QApplication::translate("ViewParams", Gui::ViewParams::docShadowTransparentShadow()));
    checkBoxShadowTransparentShadow->setText(QObject::tr("Transparent shadow"));
}

void DlgSettingsDrawStyles::changeEvent(QEvent *e)
{
    // Auto generated code (Tools/params_utils.py:516)
    if (e->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(e);
}

#include "moc_DlgSettingsDrawStyles.cpp"
//[[[end]]]
