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

#ifndef GUI_DIALOG_DLGSETTINGSOBJECTS_H
#define GUI_DIALOG_DLGSETTINGSOBJECTS_H

/*[[[cog
import DlgSettingsObjects
DlgSettingsObjects.declare()
]]]*/

// Auto generated code (Tools/params_utils.py:404)
#include <Gui/PropertyPage.h>
#include <Gui/PrefWidgets.h>
// Auto generated code (Tools/params_utils.py:409)
class QLabel;
class QGroupBox;

namespace Gui {
namespace Dialog {
/** Preference dialog for various document objects related settings

 * This class is auto generated by Gui/DlgSettingsObjects.py. Modify that file
 * instead of this one, if you want to make any change. You need
 * to install Cog Python package for code generation:
 * @code
 *     pip install cogapp
 * @endcode
 *
 * Once modified, you can regenerate the header and the source file,
 * @code
 *     python3 -m cogapp -r Gui/DlgSettingsObjects.h Gui/DlgSettingsObjects.cpp
 * @endcode
 */
class DlgSettingsObjects : public Gui::Dialog::PreferencePage
{
    Q_OBJECT

public:
    DlgSettingsObjects( QWidget* parent = 0 );
    ~DlgSettingsObjects();

    void saveSettings();
    void loadSettings();
    void retranslateUi();

protected:
    void changeEvent(QEvent *e);

private:

    // Auto generated code (Tools/params_utils.py:327)
    QGroupBox * groupGroupobjects = nullptr;
    Gui::PrefCheckBox *ClaimAllChildren = nullptr;
    Gui::PrefCheckBox *KeepHiddenChildren = nullptr;
    Gui::PrefCheckBox *ExportChildren = nullptr;
    Gui::PrefCheckBox *CreateOrigin = nullptr;
    // Auto generated code (Gui/DlgSettingsObjects.py:55)
    QPushButton *buttonCreateOrigin = nullptr;
    Gui::PrefCheckBox *GeoGroupAllowCrossLink = nullptr;
    Gui::PrefCheckBox *CreateGroupInGroup = nullptr;
// Auto generated code (Tools/params_utils.py:454)
};
} // namespace Dialog
} // namespace Gui
//[[[end]]]

#endif // GUI_DIALOG_DLGSETTINGSOBJECTS_IMP_H
