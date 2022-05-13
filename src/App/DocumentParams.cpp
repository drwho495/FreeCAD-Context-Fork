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


/*[[[cog
import DocumentParams
DocumentParams.define()
]]]*/

// Auto generated code. See class document of DocumentParams.
#include <unordered_map>
#include <App/Application.h>
#include <App/DynamicProperty.h>
#include "DocumentParams.h"
using namespace App;

namespace {

// Auto generated code. See class document of DocumentParams.
class DocumentParamsP: public ParameterGrp::ObserverType {
public:
    // Auto generated code. See class document of DocumentParams.
    ParameterGrp::handle handle;

    // Auto generated code. See class document of DocumentParams.
    std::unordered_map<const char *,void(*)(DocumentParamsP*),App::CStringHasher,App::CStringHasher> funcs;

    boost::signals2::signal<void (const char*)> signalParamChanged;

    std::string prefAuthor; // Auto generated code. See class document of DocumentParams.
    bool prefSetAuthorOnSave; // Auto generated code. See class document of DocumentParams.
    std::string prefCompany; // Auto generated code. See class document of DocumentParams.
    long prefLicenseType; // Auto generated code. See class document of DocumentParams.
    std::string prefLicenseUrl; // Auto generated code. See class document of DocumentParams.
    long CompressionLevel; // Auto generated code. See class document of DocumentParams.
    bool CheckExtension; // Auto generated code. See class document of DocumentParams.
    long ForceXML; // Auto generated code. See class document of DocumentParams.
    bool SplitXML; // Auto generated code. See class document of DocumentParams.
    bool PreferBinary; // Auto generated code. See class document of DocumentParams.
    bool AutoRemoveFile; // Auto generated code. See class document of DocumentParams.
    bool BackupPolicy; // Auto generated code. See class document of DocumentParams.
    bool CreateBackupFiles; // Auto generated code. See class document of DocumentParams.
    bool UseFCBakExtension; // Auto generated code. See class document of DocumentParams.
    std::string SaveBackupDateFormat; // Auto generated code. See class document of DocumentParams.
    long CountBackupFiles; // Auto generated code. See class document of DocumentParams.
    bool OptimizeRecompute; // Auto generated code. See class document of DocumentParams.
    bool CanAbortRecompute; // Auto generated code. See class document of DocumentParams.
    bool UseHasher; // Auto generated code. See class document of DocumentParams.
    bool ViewObjectTransaction; // Auto generated code. See class document of DocumentParams.
    bool WarnRecomputeOnRestore; // Auto generated code. See class document of DocumentParams.
    bool NoPartialLoading; // Auto generated code. See class document of DocumentParams.
    bool ThumbnailNoBackground; // Auto generated code. See class document of DocumentParams.
    long ThumbnailSampleSize; // Auto generated code. See class document of DocumentParams.
    bool DuplicateLabels; // Auto generated code. See class document of DocumentParams.
    bool TransactionOnRecompute; // Auto generated code. See class document of DocumentParams.
    bool RelativeStringID; // Auto generated code. See class document of DocumentParams.
    bool EnableMaterialEdit; // Auto generated code. See class document of DocumentParams.

    // Auto generated code. See class document of DocumentParams.
    DocumentParamsP() {
        handle = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Document");
        handle->Attach(this);

        prefAuthor = handle->GetASCII("prefAuthor", "");
        funcs["prefAuthor"] = &DocumentParamsP::updateprefAuthor;
        prefSetAuthorOnSave = handle->GetBool("prefSetAuthorOnSave", false);
        funcs["prefSetAuthorOnSave"] = &DocumentParamsP::updateprefSetAuthorOnSave;
        prefCompany = handle->GetASCII("prefCompany", "");
        funcs["prefCompany"] = &DocumentParamsP::updateprefCompany;
        prefLicenseType = handle->GetInt("prefLicenseType", 0);
        funcs["prefLicenseType"] = &DocumentParamsP::updateprefLicenseType;
        prefLicenseUrl = handle->GetASCII("prefLicenseUrl", "");
        funcs["prefLicenseUrl"] = &DocumentParamsP::updateprefLicenseUrl;
        CompressionLevel = handle->GetInt("CompressionLevel", 3);
        funcs["CompressionLevel"] = &DocumentParamsP::updateCompressionLevel;
        CheckExtension = handle->GetBool("CheckExtension", true);
        funcs["CheckExtension"] = &DocumentParamsP::updateCheckExtension;
        ForceXML = handle->GetInt("ForceXML", 3);
        funcs["ForceXML"] = &DocumentParamsP::updateForceXML;
        SplitXML = handle->GetBool("SplitXML", true);
        funcs["SplitXML"] = &DocumentParamsP::updateSplitXML;
        PreferBinary = handle->GetBool("PreferBinary", false);
        funcs["PreferBinary"] = &DocumentParamsP::updatePreferBinary;
        AutoRemoveFile = handle->GetBool("AutoRemoveFile", true);
        funcs["AutoRemoveFile"] = &DocumentParamsP::updateAutoRemoveFile;
        BackupPolicy = handle->GetBool("BackupPolicy", true);
        funcs["BackupPolicy"] = &DocumentParamsP::updateBackupPolicy;
        CreateBackupFiles = handle->GetBool("CreateBackupFiles", true);
        funcs["CreateBackupFiles"] = &DocumentParamsP::updateCreateBackupFiles;
        UseFCBakExtension = handle->GetBool("UseFCBakExtension", false);
        funcs["UseFCBakExtension"] = &DocumentParamsP::updateUseFCBakExtension;
        SaveBackupDateFormat = handle->GetASCII("SaveBackupDateFormat", "%Y%m%d-%H%M%S");
        funcs["SaveBackupDateFormat"] = &DocumentParamsP::updateSaveBackupDateFormat;
        CountBackupFiles = handle->GetInt("CountBackupFiles", 1);
        funcs["CountBackupFiles"] = &DocumentParamsP::updateCountBackupFiles;
        OptimizeRecompute = handle->GetBool("OptimizeRecompute", true);
        funcs["OptimizeRecompute"] = &DocumentParamsP::updateOptimizeRecompute;
        CanAbortRecompute = handle->GetBool("CanAbortRecompute", true);
        funcs["CanAbortRecompute"] = &DocumentParamsP::updateCanAbortRecompute;
        UseHasher = handle->GetBool("UseHasher", true);
        funcs["UseHasher"] = &DocumentParamsP::updateUseHasher;
        ViewObjectTransaction = handle->GetBool("ViewObjectTransaction", false);
        funcs["ViewObjectTransaction"] = &DocumentParamsP::updateViewObjectTransaction;
        WarnRecomputeOnRestore = handle->GetBool("WarnRecomputeOnRestore", true);
        funcs["WarnRecomputeOnRestore"] = &DocumentParamsP::updateWarnRecomputeOnRestore;
        NoPartialLoading = handle->GetBool("NoPartialLoading", false);
        funcs["NoPartialLoading"] = &DocumentParamsP::updateNoPartialLoading;
        ThumbnailNoBackground = handle->GetBool("ThumbnailNoBackground", false);
        funcs["ThumbnailNoBackground"] = &DocumentParamsP::updateThumbnailNoBackground;
        ThumbnailSampleSize = handle->GetInt("ThumbnailSampleSize", 0);
        funcs["ThumbnailSampleSize"] = &DocumentParamsP::updateThumbnailSampleSize;
        DuplicateLabels = handle->GetBool("DuplicateLabels", false);
        funcs["DuplicateLabels"] = &DocumentParamsP::updateDuplicateLabels;
        TransactionOnRecompute = handle->GetBool("TransactionOnRecompute", false);
        funcs["TransactionOnRecompute"] = &DocumentParamsP::updateTransactionOnRecompute;
        RelativeStringID = handle->GetBool("RelativeStringID", true);
        funcs["RelativeStringID"] = &DocumentParamsP::updateRelativeStringID;
        EnableMaterialEdit = handle->GetBool("EnableMaterialEdit", true);
        funcs["EnableMaterialEdit"] = &DocumentParamsP::updateEnableMaterialEdit;
    }

    // Auto generated code. See class document of DocumentParams.
    ~DocumentParamsP() {
    }

    // Auto generated code. See class document of DocumentParams.
    void OnChange(Base::Subject<const char*> &, const char* sReason) {
        if(!sReason)
            return;
        auto it = funcs.find(sReason);
        if(it == funcs.end())
            return;
        it->second(this);
        signalParamChanged(sReason);
    }


    // Auto generated code. See class document of DocumentParams.
    static void updateprefAuthor(DocumentParamsP *self) {
        self->prefAuthor = self->handle->GetASCII("prefAuthor", "");
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateprefSetAuthorOnSave(DocumentParamsP *self) {
        self->prefSetAuthorOnSave = self->handle->GetBool("prefSetAuthorOnSave", false);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateprefCompany(DocumentParamsP *self) {
        self->prefCompany = self->handle->GetASCII("prefCompany", "");
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateprefLicenseType(DocumentParamsP *self) {
        self->prefLicenseType = self->handle->GetInt("prefLicenseType", 0);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateprefLicenseUrl(DocumentParamsP *self) {
        self->prefLicenseUrl = self->handle->GetASCII("prefLicenseUrl", "");
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateCompressionLevel(DocumentParamsP *self) {
        self->CompressionLevel = self->handle->GetInt("CompressionLevel", 3);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateCheckExtension(DocumentParamsP *self) {
        self->CheckExtension = self->handle->GetBool("CheckExtension", true);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateForceXML(DocumentParamsP *self) {
        self->ForceXML = self->handle->GetInt("ForceXML", 3);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateSplitXML(DocumentParamsP *self) {
        self->SplitXML = self->handle->GetBool("SplitXML", true);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updatePreferBinary(DocumentParamsP *self) {
        self->PreferBinary = self->handle->GetBool("PreferBinary", false);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateAutoRemoveFile(DocumentParamsP *self) {
        self->AutoRemoveFile = self->handle->GetBool("AutoRemoveFile", true);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateBackupPolicy(DocumentParamsP *self) {
        self->BackupPolicy = self->handle->GetBool("BackupPolicy", true);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateCreateBackupFiles(DocumentParamsP *self) {
        self->CreateBackupFiles = self->handle->GetBool("CreateBackupFiles", true);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateUseFCBakExtension(DocumentParamsP *self) {
        self->UseFCBakExtension = self->handle->GetBool("UseFCBakExtension", false);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateSaveBackupDateFormat(DocumentParamsP *self) {
        self->SaveBackupDateFormat = self->handle->GetASCII("SaveBackupDateFormat", "%Y%m%d-%H%M%S");
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateCountBackupFiles(DocumentParamsP *self) {
        self->CountBackupFiles = self->handle->GetInt("CountBackupFiles", 1);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateOptimizeRecompute(DocumentParamsP *self) {
        self->OptimizeRecompute = self->handle->GetBool("OptimizeRecompute", true);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateCanAbortRecompute(DocumentParamsP *self) {
        self->CanAbortRecompute = self->handle->GetBool("CanAbortRecompute", true);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateUseHasher(DocumentParamsP *self) {
        self->UseHasher = self->handle->GetBool("UseHasher", true);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateViewObjectTransaction(DocumentParamsP *self) {
        self->ViewObjectTransaction = self->handle->GetBool("ViewObjectTransaction", false);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateWarnRecomputeOnRestore(DocumentParamsP *self) {
        self->WarnRecomputeOnRestore = self->handle->GetBool("WarnRecomputeOnRestore", true);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateNoPartialLoading(DocumentParamsP *self) {
        self->NoPartialLoading = self->handle->GetBool("NoPartialLoading", false);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateThumbnailNoBackground(DocumentParamsP *self) {
        self->ThumbnailNoBackground = self->handle->GetBool("ThumbnailNoBackground", false);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateThumbnailSampleSize(DocumentParamsP *self) {
        self->ThumbnailSampleSize = self->handle->GetInt("ThumbnailSampleSize", 0);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateDuplicateLabels(DocumentParamsP *self) {
        self->DuplicateLabels = self->handle->GetBool("DuplicateLabels", false);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateTransactionOnRecompute(DocumentParamsP *self) {
        self->TransactionOnRecompute = self->handle->GetBool("TransactionOnRecompute", false);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateRelativeStringID(DocumentParamsP *self) {
        self->RelativeStringID = self->handle->GetBool("RelativeStringID", true);
    }
    // Auto generated code. See class document of DocumentParams.
    static void updateEnableMaterialEdit(DocumentParamsP *self) {
        self->EnableMaterialEdit = self->handle->GetBool("EnableMaterialEdit", true);
    }
};

// Auto generated code. See class document of DocumentParams.
DocumentParamsP *instance() {
    static DocumentParamsP *inst = new DocumentParamsP;
    return inst;
}

} // Anonymous namespace

// Auto generated code. See class document of DocumentParams.
ParameterGrp::handle DocumentParams::getHandle() {
    return instance()->handle;
}

// Auto generated code. See class document of DocumentParams.
boost::signals2::signal<void (const char*)> &
DocumentParams::signalParamChanged() {
    return instance()->signalParamChanged;
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docprefAuthor() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const std::string & DocumentParams::getprefAuthor() {
    return instance()->prefAuthor;
}

// Auto generated code. See class document of DocumentParams.
const std::string & DocumentParams::defaultprefAuthor() {
    const static std::string def = "";
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setprefAuthor(const std::string &v) {
    instance()->handle->SetASCII("prefAuthor",v);
    instance()->prefAuthor = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeprefAuthor() {
    instance()->handle->RemoveASCII("prefAuthor");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docprefSetAuthorOnSave() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getprefSetAuthorOnSave() {
    return instance()->prefSetAuthorOnSave;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultprefSetAuthorOnSave() {
    const static bool def = false;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setprefSetAuthorOnSave(const bool &v) {
    instance()->handle->SetBool("prefSetAuthorOnSave",v);
    instance()->prefSetAuthorOnSave = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeprefSetAuthorOnSave() {
    instance()->handle->RemoveBool("prefSetAuthorOnSave");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docprefCompany() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const std::string & DocumentParams::getprefCompany() {
    return instance()->prefCompany;
}

// Auto generated code. See class document of DocumentParams.
const std::string & DocumentParams::defaultprefCompany() {
    const static std::string def = "";
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setprefCompany(const std::string &v) {
    instance()->handle->SetASCII("prefCompany",v);
    instance()->prefCompany = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeprefCompany() {
    instance()->handle->RemoveASCII("prefCompany");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docprefLicenseType() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const long & DocumentParams::getprefLicenseType() {
    return instance()->prefLicenseType;
}

// Auto generated code. See class document of DocumentParams.
const long & DocumentParams::defaultprefLicenseType() {
    const static long def = 0;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setprefLicenseType(const long &v) {
    instance()->handle->SetInt("prefLicenseType",v);
    instance()->prefLicenseType = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeprefLicenseType() {
    instance()->handle->RemoveInt("prefLicenseType");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docprefLicenseUrl() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const std::string & DocumentParams::getprefLicenseUrl() {
    return instance()->prefLicenseUrl;
}

// Auto generated code. See class document of DocumentParams.
const std::string & DocumentParams::defaultprefLicenseUrl() {
    const static std::string def = "";
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setprefLicenseUrl(const std::string &v) {
    instance()->handle->SetASCII("prefLicenseUrl",v);
    instance()->prefLicenseUrl = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeprefLicenseUrl() {
    instance()->handle->RemoveASCII("prefLicenseUrl");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docCompressionLevel() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const long & DocumentParams::getCompressionLevel() {
    return instance()->CompressionLevel;
}

// Auto generated code. See class document of DocumentParams.
const long & DocumentParams::defaultCompressionLevel() {
    const static long def = 3;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setCompressionLevel(const long &v) {
    instance()->handle->SetInt("CompressionLevel",v);
    instance()->CompressionLevel = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeCompressionLevel() {
    instance()->handle->RemoveInt("CompressionLevel");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docCheckExtension() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getCheckExtension() {
    return instance()->CheckExtension;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultCheckExtension() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setCheckExtension(const bool &v) {
    instance()->handle->SetBool("CheckExtension",v);
    instance()->CheckExtension = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeCheckExtension() {
    instance()->handle->RemoveBool("CheckExtension");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docForceXML() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const long & DocumentParams::getForceXML() {
    return instance()->ForceXML;
}

// Auto generated code. See class document of DocumentParams.
const long & DocumentParams::defaultForceXML() {
    const static long def = 3;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setForceXML(const long &v) {
    instance()->handle->SetInt("ForceXML",v);
    instance()->ForceXML = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeForceXML() {
    instance()->handle->RemoveInt("ForceXML");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docSplitXML() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getSplitXML() {
    return instance()->SplitXML;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultSplitXML() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setSplitXML(const bool &v) {
    instance()->handle->SetBool("SplitXML",v);
    instance()->SplitXML = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeSplitXML() {
    instance()->handle->RemoveBool("SplitXML");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docPreferBinary() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getPreferBinary() {
    return instance()->PreferBinary;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultPreferBinary() {
    const static bool def = false;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setPreferBinary(const bool &v) {
    instance()->handle->SetBool("PreferBinary",v);
    instance()->PreferBinary = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removePreferBinary() {
    instance()->handle->RemoveBool("PreferBinary");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docAutoRemoveFile() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getAutoRemoveFile() {
    return instance()->AutoRemoveFile;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultAutoRemoveFile() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setAutoRemoveFile(const bool &v) {
    instance()->handle->SetBool("AutoRemoveFile",v);
    instance()->AutoRemoveFile = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeAutoRemoveFile() {
    instance()->handle->RemoveBool("AutoRemoveFile");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docBackupPolicy() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getBackupPolicy() {
    return instance()->BackupPolicy;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultBackupPolicy() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setBackupPolicy(const bool &v) {
    instance()->handle->SetBool("BackupPolicy",v);
    instance()->BackupPolicy = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeBackupPolicy() {
    instance()->handle->RemoveBool("BackupPolicy");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docCreateBackupFiles() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getCreateBackupFiles() {
    return instance()->CreateBackupFiles;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultCreateBackupFiles() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setCreateBackupFiles(const bool &v) {
    instance()->handle->SetBool("CreateBackupFiles",v);
    instance()->CreateBackupFiles = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeCreateBackupFiles() {
    instance()->handle->RemoveBool("CreateBackupFiles");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docUseFCBakExtension() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getUseFCBakExtension() {
    return instance()->UseFCBakExtension;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultUseFCBakExtension() {
    const static bool def = false;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setUseFCBakExtension(const bool &v) {
    instance()->handle->SetBool("UseFCBakExtension",v);
    instance()->UseFCBakExtension = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeUseFCBakExtension() {
    instance()->handle->RemoveBool("UseFCBakExtension");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docSaveBackupDateFormat() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const std::string & DocumentParams::getSaveBackupDateFormat() {
    return instance()->SaveBackupDateFormat;
}

// Auto generated code. See class document of DocumentParams.
const std::string & DocumentParams::defaultSaveBackupDateFormat() {
    const static std::string def = "%Y%m%d-%H%M%S";
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setSaveBackupDateFormat(const std::string &v) {
    instance()->handle->SetASCII("SaveBackupDateFormat",v);
    instance()->SaveBackupDateFormat = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeSaveBackupDateFormat() {
    instance()->handle->RemoveASCII("SaveBackupDateFormat");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docCountBackupFiles() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const long & DocumentParams::getCountBackupFiles() {
    return instance()->CountBackupFiles;
}

// Auto generated code. See class document of DocumentParams.
const long & DocumentParams::defaultCountBackupFiles() {
    const static long def = 1;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setCountBackupFiles(const long &v) {
    instance()->handle->SetInt("CountBackupFiles",v);
    instance()->CountBackupFiles = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeCountBackupFiles() {
    instance()->handle->RemoveInt("CountBackupFiles");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docOptimizeRecompute() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getOptimizeRecompute() {
    return instance()->OptimizeRecompute;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultOptimizeRecompute() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setOptimizeRecompute(const bool &v) {
    instance()->handle->SetBool("OptimizeRecompute",v);
    instance()->OptimizeRecompute = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeOptimizeRecompute() {
    instance()->handle->RemoveBool("OptimizeRecompute");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docCanAbortRecompute() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getCanAbortRecompute() {
    return instance()->CanAbortRecompute;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultCanAbortRecompute() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setCanAbortRecompute(const bool &v) {
    instance()->handle->SetBool("CanAbortRecompute",v);
    instance()->CanAbortRecompute = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeCanAbortRecompute() {
    instance()->handle->RemoveBool("CanAbortRecompute");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docUseHasher() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getUseHasher() {
    return instance()->UseHasher;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultUseHasher() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setUseHasher(const bool &v) {
    instance()->handle->SetBool("UseHasher",v);
    instance()->UseHasher = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeUseHasher() {
    instance()->handle->RemoveBool("UseHasher");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docViewObjectTransaction() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getViewObjectTransaction() {
    return instance()->ViewObjectTransaction;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultViewObjectTransaction() {
    const static bool def = false;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setViewObjectTransaction(const bool &v) {
    instance()->handle->SetBool("ViewObjectTransaction",v);
    instance()->ViewObjectTransaction = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeViewObjectTransaction() {
    instance()->handle->RemoveBool("ViewObjectTransaction");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docWarnRecomputeOnRestore() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getWarnRecomputeOnRestore() {
    return instance()->WarnRecomputeOnRestore;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultWarnRecomputeOnRestore() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setWarnRecomputeOnRestore(const bool &v) {
    instance()->handle->SetBool("WarnRecomputeOnRestore",v);
    instance()->WarnRecomputeOnRestore = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeWarnRecomputeOnRestore() {
    instance()->handle->RemoveBool("WarnRecomputeOnRestore");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docNoPartialLoading() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getNoPartialLoading() {
    return instance()->NoPartialLoading;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultNoPartialLoading() {
    const static bool def = false;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setNoPartialLoading(const bool &v) {
    instance()->handle->SetBool("NoPartialLoading",v);
    instance()->NoPartialLoading = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeNoPartialLoading() {
    instance()->handle->RemoveBool("NoPartialLoading");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docThumbnailNoBackground() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getThumbnailNoBackground() {
    return instance()->ThumbnailNoBackground;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultThumbnailNoBackground() {
    const static bool def = false;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setThumbnailNoBackground(const bool &v) {
    instance()->handle->SetBool("ThumbnailNoBackground",v);
    instance()->ThumbnailNoBackground = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeThumbnailNoBackground() {
    instance()->handle->RemoveBool("ThumbnailNoBackground");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docThumbnailSampleSize() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const long & DocumentParams::getThumbnailSampleSize() {
    return instance()->ThumbnailSampleSize;
}

// Auto generated code. See class document of DocumentParams.
const long & DocumentParams::defaultThumbnailSampleSize() {
    const static long def = 0;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setThumbnailSampleSize(const long &v) {
    instance()->handle->SetInt("ThumbnailSampleSize",v);
    instance()->ThumbnailSampleSize = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeThumbnailSampleSize() {
    instance()->handle->RemoveInt("ThumbnailSampleSize");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docDuplicateLabels() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getDuplicateLabels() {
    return instance()->DuplicateLabels;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultDuplicateLabels() {
    const static bool def = false;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setDuplicateLabels(const bool &v) {
    instance()->handle->SetBool("DuplicateLabels",v);
    instance()->DuplicateLabels = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeDuplicateLabels() {
    instance()->handle->RemoveBool("DuplicateLabels");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docTransactionOnRecompute() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getTransactionOnRecompute() {
    return instance()->TransactionOnRecompute;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultTransactionOnRecompute() {
    const static bool def = false;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setTransactionOnRecompute(const bool &v) {
    instance()->handle->SetBool("TransactionOnRecompute",v);
    instance()->TransactionOnRecompute = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeTransactionOnRecompute() {
    instance()->handle->RemoveBool("TransactionOnRecompute");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docRelativeStringID() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getRelativeStringID() {
    return instance()->RelativeStringID;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultRelativeStringID() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setRelativeStringID(const bool &v) {
    instance()->handle->SetBool("RelativeStringID",v);
    instance()->RelativeStringID = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeRelativeStringID() {
    instance()->handle->RemoveBool("RelativeStringID");
}

// Auto generated code. See class document of DocumentParams.
const char *DocumentParams::docEnableMaterialEdit() {
    return "";
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::getEnableMaterialEdit() {
    return instance()->EnableMaterialEdit;
}

// Auto generated code. See class document of DocumentParams.
const bool & DocumentParams::defaultEnableMaterialEdit() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::setEnableMaterialEdit(const bool &v) {
    instance()->handle->SetBool("EnableMaterialEdit",v);
    instance()->EnableMaterialEdit = v;
}

// Auto generated code. See class document of DocumentParams.
void DocumentParams::removeEnableMaterialEdit() {
    instance()->handle->RemoveBool("EnableMaterialEdit");
}
//[[[end]]]
