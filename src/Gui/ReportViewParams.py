import sys
from os import sys, path

# import Tools/params_utils.py
sys.path.append(path.join(path.dirname(path.dirname(path.abspath(__file__))), 'Tools'))
import params_utils

from params_utils import ParamBool, ParamInt, ParamString, ParamQString, ParamUInt, ParamFloat

NameSpace = 'Gui'
ClassName = 'ReportViewParams'
ParamPath = 'User parameter:BaseApp/Preferences/OutputWindow'
ClassDoc = 'Convenient class to obtain ReportView related parameters'
Signal = True

Params = [
    ParamBool('checkShowReportViewOnWarning', True),
    ParamBool('checkShowReportViewOnError', True),
    ParamBool('checkShowReportViewOnNormalMessage', False),
    ParamBool('checkShowReportViewOnLogMessage', False),
    ParamBool("checkShowReportTimecode", True),

    ParamInt("LogMessageSize", 0),
    ParamQString('CommandRedirect', '',
        doc='Prefix for marking python command in message to be redirected to Python console\n'
            'This is used as a debug help for output command from external libraries'),
]

def declare():
    params_utils.declare_begin(sys.modules[__name__])
    params_utils.declare_end(sys.modules[__name__])

def define():
    params_utils.define(sys.modules[__name__])
