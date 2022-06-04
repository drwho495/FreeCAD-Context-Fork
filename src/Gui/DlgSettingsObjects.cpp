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

// Auto generated code (Tools/params_utils.py:472)
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
// Auto generated code (Tools/params_utils.py:490)
#include "DlgSettingsObjects.h"
using namespace Gui::Dialog;
/* TRANSLATOR Gui::Dialog::DlgSettingsObjects */

// Auto generated code (Tools/params_utils.py:497)
DlgSettingsObjects::DlgSettingsObjects(QWidget* parent)
    : PreferencePage( parent )
{

    auto layout = new QVBoxLayout(this);


    // Auto generated code (Tools/params_utils.py:338)
    groupGroupobjects = new QGroupBox(this);
    layout->addWidget(groupGroupobjects);
    auto layoutHorizGroupobjects = new QHBoxLayout(groupGroupobjects);
    auto layoutGroupobjects = new QGridLayout();
    layoutHorizGroupobjects->addLayout(layoutGroupobjects);
    layoutHorizGroupobjects->addStretch();

    // Auto generated code (Tools/params_utils.py:349)
    ClaimAllChildren = new Gui::PrefCheckBox(this);
    layoutGroupobjects->addWidget(ClaimAllChildren, 0, 0);
    ClaimAllChildren->setChecked(App::GroupParams::defaultClaimAllChildren());
    ClaimAllChildren->setEntryName("ClaimAllChildren");
    ClaimAllChildren->setParamGrpPath("Group");

    // Auto generated code (Tools/params_utils.py:349)
    KeepHiddenChildren = new Gui::PrefCheckBox(this);
    layoutGroupobjects->addWidget(KeepHiddenChildren, 1, 0);
    KeepHiddenChildren->setChecked(App::GroupParams::defaultKeepHiddenChildren());
    KeepHiddenChildren->setEntryName("KeepHiddenChildren");
    KeepHiddenChildren->setParamGrpPath("Group");

    // Auto generated code (Tools/params_utils.py:349)
    ExportChildren = new Gui::PrefCheckBox(this);
    layoutGroupobjects->addWidget(ExportChildren, 2, 0);
    ExportChildren->setChecked(App::GroupParams::defaultExportChildren());
    ExportChildren->setEntryName("ExportChildren");
    ExportChildren->setParamGrpPath("Group");

    // Auto generated code (Tools/params_utils.py:349)
    CreateOrigin = new Gui::PrefCheckBox(this);
    layoutGroupobjects->addWidget(CreateOrigin, 3, 0);
    CreateOrigin->setChecked(App::GroupParams::defaultCreateOrigin());
    CreateOrigin->setEntryName("CreateOrigin");
    CreateOrigin->setParamGrpPath("Group");

    // Auto generated code (Tools/params_utils.py:349)
    GeoGroupAllowCrossLink = new Gui::PrefCheckBox(this);
    layoutGroupobjects->addWidget(GeoGroupAllowCrossLink, 4, 0);
    GeoGroupAllowCrossLink->setChecked(App::GroupParams::defaultGeoGroupAllowCrossLink());
    GeoGroupAllowCrossLink->setEntryName("GeoGroupAllowCrossLink");
    GeoGroupAllowCrossLink->setParamGrpPath("Group");
    layout->addItem(new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Expanding));
    retranslateUi();
    // Auto generated code (Tools/params_utils.py:504)
    
}

#include "DlgSettingsObjects.h"
// Auto generated code (Tools/params_utils.py:510)
DlgSettingsObjects::~DlgSettingsObjects()
{
}

// Auto generated code (Tools/params_utils.py:516)
void DlgSettingsObjects::saveSettings()
{
    // Auto generated code (Tools/params_utils.py:381)
    ClaimAllChildren->onSave();
    KeepHiddenChildren->onSave();
    ExportChildren->onSave();
    CreateOrigin->onSave();
    GeoGroupAllowCrossLink->onSave();
}

// Auto generated code (Tools/params_utils.py:523)
void DlgSettingsObjects::loadSettings()
{
    // Auto generated code (Tools/params_utils.py:371)
    ClaimAllChildren->onRestore();
    KeepHiddenChildren->onRestore();
    ExportChildren->onRestore();
    CreateOrigin->onRestore();
    GeoGroupAllowCrossLink->onRestore();
}

// Auto generated code (Tools/params_utils.py:530)
void DlgSettingsObjects::retranslateUi()
{
    setWindowTitle(QObject::tr("Objects"));
    groupGroupobjects->setTitle(QObject::tr("Group objects"));
    ClaimAllChildren->setToolTip(QApplication::translate("GroupParams", App::GroupParams::docClaimAllChildren()));
    ClaimAllChildren->setText(QObject::tr("Claim all children"));
    KeepHiddenChildren->setToolTip(QApplication::translate("GroupParams", App::GroupParams::docKeepHiddenChildren()));
    KeepHiddenChildren->setText(QObject::tr("Remember hidden children"));
    ExportChildren->setToolTip(QApplication::translate("GroupParams", App::GroupParams::docExportChildren()));
    ExportChildren->setText(QObject::tr("Export children by visibility"));
    CreateOrigin->setToolTip(QApplication::translate("GroupParams", App::GroupParams::docCreateOrigin()));
    CreateOrigin->setText(QObject::tr("Always create origin features in origin group"));
    GeoGroupAllowCrossLink->setToolTip(QApplication::translate("GroupParams", App::GroupParams::docGeoGroupAllowCrossLink()));
    GeoGroupAllowCrossLink->setText(QObject::tr("Allow cross coordinate links in GeoFeatureGroup (App::Part)"));
}

// Auto generated code (Tools/params_utils.py:543)
void DlgSettingsObjects::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QWidget::changeEvent(e);
}

// Auto generated code (Tools/params_utils.py:554)
#include "moc_DlgSettingsObjects.cpp"
//[[[end]]]
