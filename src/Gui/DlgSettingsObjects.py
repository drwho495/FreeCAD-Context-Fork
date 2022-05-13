import sys
from os import sys, path

# import Tools/params_utils.py
sys.path.append(path.join(path.dirname(path.dirname(path.abspath(__file__))), 'Tools'))
import params_utils

from params_utils import ParamBool, ParamInt, ParamString, ParamUInt, ParamFloat

sys.path.append(path.join(path.dirname(path.dirname(path.abspath(__file__))), 'App'))
import GroupParams

Title = 'Objects'
NameSpace = 'Gui'
ClassName = 'DlgSettingsObjects'
ClassDoc = 'Preference dialog for various document objects related settings'
ParamGroup = (
    ('GroupObjects', 'Group objects', GroupParams.Params),
)

def declare():
    params_utils.preference_dialog_declare(sys.modules[__name__])

def define():
    params_utils.preference_dialog_define(sys.modules[__name__])
