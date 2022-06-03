/****************************************************************************
 *   Copyright (c) 2021 Zheng Lei (realthunder) <realthunder.dev@gmail.com> *
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

/*[[[cog
import DlgSettingsObjects
DlgSettingsObjects.define()
]]]*/

// Auto generated code (Tools/params_utils.py:469)
#ifndef _PreComp_
#   include <QApplication>
#   include <QLabel>
#   include <QGroupBox>
#   include <QGridLayout>
#   include <QVBoxLayout>
#   include <QHBoxLayout>
#endif
#include <Gui/PrefWidgets.h>
#include <App/GroupParams.h>
// Auto generated code (Tools/params_utils.py:487)
#include "DlgSettingsObjects.h"
using namespace Gui::Dialog;
/* TRANSLATOR Gui::Dialog::DlgSettingsObjects */

// Auto generated code (Tools/params_utils.py:494)
DlgSettingsObjects::DlgSettingsObjects(QWidget* parent)
    : PreferencePage( parent )
{

    auto layout = new QVBoxLayout(this);


    // Auto generated code (Tools/params_utils.py:336)
    groupGroupObjects = new QGroupBox(this);
    layout->addWidget(groupGroupObjects);
    auto layoutHorizGroupObjects = new QHBoxLayout(groupGroupObjects);
    auto layoutGroupObjects = new QGridLayout();
    layoutHorizGroupObjects->addLayout(layoutGroupObjects);
    layoutHorizGroupObjects->addStretch();

    // Auto generated code (Tools/params_utils.py:347)
    checkBoxClaimAllChildren = new Gui::PrefCheckBox(this);
    layoutGroupObjects->addWidget(checkBoxClaimAllChildren, 0, 0);
    checkBoxClaimAllChildren->setChecked(App::GroupParams::defaultClaimAllChildren());
    checkBoxClaimAllChildren->setEntryName("ClaimAllChildren");
    checkBoxClaimAllChildren->setParamGrpPath("Group");

    // Auto generated code (Tools/params_utils.py:347)
    checkBoxKeepHiddenChildren = new Gui::PrefCheckBox(this);
    layoutGroupObjects->addWidget(checkBoxKeepHiddenChildren, 1, 0);
    checkBoxKeepHiddenChildren->setChecked(App::GroupParams::defaultKeepHiddenChildren());
    checkBoxKeepHiddenChildren->setEntryName("KeepHiddenChildren");
    checkBoxKeepHiddenChildren->setParamGrpPath("Group");

    // Auto generated code (Tools/params_utils.py:347)
    checkBoxExportChildren = new Gui::PrefCheckBox(this);
    layoutGroupObjects->addWidget(checkBoxExportChildren, 2, 0);
    checkBoxExportChildren->setChecked(App::GroupParams::defaultExportChildren());
    checkBoxExportChildren->setEntryName("ExportChildren");
    checkBoxExportChildren->setParamGrpPath("Group");

    // Auto generated code (Tools/params_utils.py:347)
    checkBoxCreateOrigin = new Gui::PrefCheckBox(this);
    layoutGroupObjects->addWidget(checkBoxCreateOrigin, 3, 0);
    checkBoxCreateOrigin->setChecked(App::GroupParams::defaultCreateOrigin());
    checkBoxCreateOrigin->setEntryName("CreateOrigin");
    checkBoxCreateOrigin->setParamGrpPath("Group");

    // Auto generated code (Tools/params_utils.py:347)
    checkBoxGeoGroupAllowCrossLink = new Gui::PrefCheckBox(this);
    layoutGroupObjects->addWidget(checkBoxGeoGroupAllowCrossLink, 4, 0);
    checkBoxGeoGroupAllowCrossLink->setChecked(App::GroupParams::defaultGeoGroupAllowCrossLink());
    checkBoxGeoGroupAllowCrossLink->setEntryName("GeoGroupAllowCrossLink");
    checkBoxGeoGroupAllowCrossLink->setParamGrpPath("Group");
    layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Expanding));
    retranslateUi();
}

// Auto generated code (Tools/params_utils.py:504)
#include "DlgSettingsObjects.h"
DlgSettingsObjects::~DlgSettingsObjects()
{
}

void DlgSettingsObjects::saveSettings()
{
    // Auto generated code (Tools/params_utils.py:513)
    // Auto generated code (Tools/params_utils.py:379)
    checkBoxClaimAllChildren->onSave();
    checkBoxKeepHiddenChildren->onSave();
    checkBoxExportChildren->onSave();
    checkBoxCreateOrigin->onSave();
    checkBoxGeoGroupAllowCrossLink->onSave();
}

void DlgSettingsObjects::loadSettings()
{
    // Auto generated code (Tools/params_utils.py:520)
    // Auto generated code (Tools/params_utils.py:369)
    checkBoxClaimAllChildren->onRestore();
    checkBoxKeepHiddenChildren->onRestore();
    checkBoxExportChildren->onRestore();
    checkBoxCreateOrigin->onRestore();
    checkBoxGeoGroupAllowCrossLink->onRestore();
}

void DlgSettingsObjects::retranslateUi()
{
    // Auto generated code (Tools/params_utils.py:527)
    setWindowTitle(QObject::tr("Objects"));
    groupGroupObjects->setTitle(QObject::tr("Group objects"));
    checkBoxClaimAllChildren->setToolTip(QApplication::translate("GroupParams", App::GroupParams::docClaimAllChildren()));
    checkBoxClaimAllChildren->setText(QObject::tr("Claim all children"));
    checkBoxKeepHiddenChildren->setToolTip(QApplication::translate("GroupParams", App::GroupParams::docKeepHiddenChildren()));
    checkBoxKeepHiddenChildren->setText(QObject::tr("Remember hidden children"));
    checkBoxExportChildren->setToolTip(QApplication::translate("GroupParams", App::GroupParams::docExportChildren()));
    checkBoxExportChildren->setText(QObject::tr("Export children by visibility"));
    checkBoxCreateOrigin->setToolTip(QApplication::translate("GroupParams", App::GroupParams::docCreateOrigin()));
    checkBoxCreateOrigin->setText(QObject::tr("Always create origin features in origin group"));
    checkBoxGeoGroupAllowCrossLink->setToolTip(QApplication::translate("GroupParams", App::GroupParams::docGeoGroupAllowCrossLink()));
    checkBoxGeoGroupAllowCrossLink->setText(QObject::tr("Allow cross coordinate links in GeoFeatureGroup (App::Part)"));
}

void DlgSettingsObjects::changeEvent(QEvent *e)
{
    // Auto generated code (Tools/params_utils.py:539)
    if (e->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(e);
}

#include "moc_DlgSettingsObjects.cpp"
//[[[end]]]
