# -*- coding: utf-8 -*-
# ***************************************************************************
# *   Copyright (c) 2022 Zheng Lei (realthunder) <realthunder.dev@gmail.com>*
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with this program; if not, write to the Free Software   *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# ***************************************************************************
'''Auto code generator for parameters in Preferences/Expression
'''
import cog, sys
from os import sys, path

# import Tools/params_utils.py
sys.path.append(path.join(path.dirname(path.dirname(path.abspath(__file__))), 'Tools'))
import params_utils

from params_utils import ParamBool, ParamInt, ParamString, ParamUInt, ParamFloat

NameSpace = 'Gui'
ClassName = 'ExprParams'
ParamPath = 'User parameter:BaseApp/Preferences/Expression'
ClassDoc = 'Convenient class to obtain expression related parameters'

Params = [
    ParamBool('CompleterCaseSensitive', False,
        doc="Expression completer with case sensitive"),
    ParamBool('CompleterMatchExact', False,
        doc="Expression completer match exact"),
    ParamBool('CompleterUnfiltered', False,
        doc="Expression completer unfiltered completion mode"),
    ParamBool('NoSystemBackground', False,
        doc="Enable in place expressiong editing"),
    ParamString('EditorTrigger', "=",
        doc="Expression editor trigger character"),
    ParamBool('AutoHideEditorIcon', True,
        doc="Only show editor icon on mouse over"),
    ParamBool('AllowReturn', False,
        doc="Allow return key in expression edit box"),
    ParamBool('EvalFuncOnEdit', False,
        doc="Auto evaluate function call when editing expression"),
    ParamInt('EditDialogWidth',0),
    ParamInt('EditDialogHeight',0),
    ParamInt('EditDialogTextHeight',0),
    ParamInt('EditDialogBGAlpha','FC_EXPR_PARAM_EDIT_BG_ALPHA',
        doc="Expression editor background opacity value when using in place editing"),
]

def declare():
    params_utils.declare_begin(sys.modules[__name__])
    params_utils.declare_end(sys.modules[__name__])

def define():
    cog.out(f'''
#ifdef FC_OS_MACOSX
    // Both MacOSX and Windows will pass through mouse event for the
    // transparent part of a top level dialog, and we need the mouse event to
    // handle resizing of expression editor. Windows will capture the mouse
    // event as long as there is any non transparency, hence setting alpha to 1
    // works. But OSX seems to require a higher transparency.
#   define FC_EXPR_PARAM_EDIT_BG_ALPHA 16
#else
#   define FC_EXPR_PARAM_EDIT_BG_ALPHA 1
#endif
''')
    params_utils.define(sys.modules[__name__])

