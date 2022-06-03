import sys
from os import sys, path

# import Tools/params_utils.py
sys.path.append(path.join(path.dirname(path.dirname(path.abspath(__file__))), 'Tools'))
import params_utils

from params_utils import ParamBool, ParamInt, ParamString, ParamUInt, ParamFloat

NameSpace = 'App'
ClassName = 'GroupParams'
ParamPath = 'User parameter:BaseApp/Preferences/Group'
ClassDoc = 'Convenient class to obtain group object (App::GroupExtension and GeoFeatureGroupExtension) related parameters'

Params = [
    ParamBool('ClaimAllChildren', True,
        title = 'Claim all children',
        doc = "Claim all children objects in tree view. If disabled, then only claim\n"
              "children that are not claimed by other children."),
    ParamBool('KeepHiddenChildren', True,
        title = 'Remember hidden children',
        doc = "Remember invisible children objects and keep those objects hidden\n"
              "when the group is made visible."),
    ParamBool('ExportChildren', True,
        title = 'Export children by visibility',
        doc = "Export visible children (e.g. when doing STEP export). Note, that once this option\n"
              "is enabled, the group object will be touched when its child toggles visibility."),
    ParamBool('CreateOrigin', False,
        title = 'Always create origin features in origin group',
        doc = "Create all origin features when the origin group is created. If Disabled\n"
              "The origin features will only be created when the origin group is expanded\n"
              "for the first time."),
    ParamBool('GeoGroupAllowCrossLink', False,
        title = 'Allow cross coordinate links in GeoFeatureGroup (App::Part)',
        doc = "Allow objects to be contained in more than one GeoFeatureGroup (e.g. App::Part).\n"
              "If diabled, adding an object to one group will auto remove it from other groups.\n"
              "WARNING! Disabling this option may produce an invalid group after changing its children."),
]

def declare():
    params_utils.declare_begin(sys.modules[__name__])
    params_utils.declare_end(sys.modules[__name__])

def define():
    params_utils.define(sys.modules[__name__])

params_utils.init_params(Params, NameSpace, ClassName, ParamPath)
