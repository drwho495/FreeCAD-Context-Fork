import sys
from os import sys, path

# import Tools/params_utils.py
sys.path.append(path.join(path.dirname(path.dirname(path.abspath(__file__))), 'Tools'))
import params_utils

from params_utils import ParamBool, ParamInt, ParamString, ParamUInt, ParamFloat

NameSpace = 'Gui'
ClassName = 'TreeParams'
ParamPath = 'User parameter:BaseApp/Preferences/TreeView'
ClassDoc = 'Convenient class to obtain tree view related parameters'
SourceFile = 'Tree.cpp'

Params = [
    ParamBool('SyncSelection', True, on_change=True),
    ParamBool('CheckBoxesSelection',False, on_change=True),
    ParamBool('SyncView', True),
    ParamBool('PreSelection', True),
    ParamBool('SyncPlacement', False),
    ParamBool('RecordSelection', True),
    ParamInt('DocumentMode', 2, on_change=True),
    ParamInt('StatusTimeout', 100),
    ParamInt('SelectionTimeout', 100),
    ParamInt('PreSelectionTimeout', 500),
    ParamInt('PreSelectionDelay', 700),
    ParamInt('PreSelectionMinDelay', 200),
    ParamBool('RecomputeOnDrop', True),
    ParamBool('KeepRootOrder', True),
    ParamBool('TreeActiveAutoExpand', True),
    ParamUInt('TreeActiveColor',  0xe6e6ffff, on_change=True),
    ParamUInt('TreeEditColor',  0x929200ff, on_change=True),
    ParamUInt('SelectingGroupColor',  0x408081ff, on_change=True),
    ParamBool('TreeActiveBold', True, on_change=True),
    ParamBool('TreeActiveItalic', False, on_change=True),
    ParamBool('TreeActiveUnderlined', False, on_change=True),
    ParamBool('TreeActiveOverlined', False, on_change=True),
    ParamInt('Indentation', 0, on_change=True),
    ParamBool('LabelExpression', False),
    ParamInt('IconSize', 0, on_change=True),
    ParamInt('FontSize', 0, on_change=True),
    ParamInt('ItemSpacing', 0, on_change=True),
    ParamUInt('ItemBackground', 0, on_change=True,
        doc = "Tree view item background. Only effecitve in overlay."),
    ParamInt('ItemBackgroundPadding', 10, on_change=True,
        doc = "Tree view item background padding."),
    ParamBool('HideColumn', False, on_change=True,
        doc = "Hide extra tree view column for item description."),
    ParamBool('HideScrollBar', True,
        doc = "Hide tree view scroll bar in dock overlay"),
    ParamBool('HideHeaderView', True,
        doc = "Hide tree view header view in dock overlay"),
    ParamBool('ResizableColumn', False, on_change=True,
        doc = "Allow tree view columns to be manually resized"),
    ParamInt('ColumnSize1', 0),
    ParamInt('ColumnSize2', 0),
]

def declare_begin():
    params_utils.declare_begin(sys.modules[__name__])

def declare_end():
    params_utils.declare_end(sys.modules[__name__])

def define():
    params_utils.define(sys.modules[__name__])
