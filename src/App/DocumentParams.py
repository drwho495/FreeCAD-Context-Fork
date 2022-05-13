import sys
from os import sys, path

# import Tools/params_utils.py
sys.path.append(path.join(path.dirname(path.dirname(path.abspath(__file__))), 'Tools'))
import params_utils

from params_utils import ParamBool, ParamInt, ParamString, ParamUInt, ParamFloat

NameSpace = 'App'
ClassName = 'DocumentParams'
ParamPath = 'User parameter:BaseApp/Preferences/Document'
ClassDoc = 'Convenient class to obtain App::Document related parameters'
Signal = True

Params = [
    ParamString('prefAuthor', ""),
    ParamBool('prefSetAuthorOnSave', False),
    ParamString('prefCompany', ""),
    ParamInt('prefLicenseType', 0),
    ParamString('prefLicenseUrl', ""),
    ParamInt('CompressionLevel', 3),
    ParamBool('CheckExtension', True),
    ParamInt('ForceXML', 3),
    ParamBool('SplitXML', True),
    ParamBool('PreferBinary', False),
    ParamBool('AutoRemoveFile', True),
    ParamBool('BackupPolicy', True),
    ParamBool('CreateBackupFiles', True),
    ParamBool('UseFCBakExtension', False),
    ParamString('SaveBackupDateFormat', "%Y%m%d-%H%M%S"),
    ParamInt('CountBackupFiles', 1),
    ParamBool('OptimizeRecompute', True),
    ParamBool('CanAbortRecompute', True),
    ParamBool('UseHasher', True),
    ParamBool('ViewObjectTransaction', False),
    ParamBool('WarnRecomputeOnRestore', True),
    ParamBool('NoPartialLoading', False),
    ParamBool('ThumbnailNoBackground', False),
    ParamInt('ThumbnailSampleSize', 0),
    ParamBool('DuplicateLabels', False),
    ParamBool('TransactionOnRecompute', False),
    ParamBool('RelativeStringID', True),
    ParamBool('EnableMaterialEdit', True),
]

def declare():
    params_utils.declare_begin(sys.modules[__name__])
    params_utils.declare_end(sys.modules[__name__])

def define():
    params_utils.define(sys.modules[__name__])
