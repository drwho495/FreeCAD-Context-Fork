/****************************************************************************
 *   Copyright (c) 2022 Zheng Lei (realthunder) <realthunder.dev@gmail.com> *
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
import ReportViewParams
ReportViewParams.define()
]]]*/

// Auto generated code. See class document of ReportViewParams.
#include <unordered_map>
#include <App/Application.h>
#include <App/DynamicProperty.h>
#include "ReportViewParams.h"
using namespace Gui;

namespace {

// Auto generated code. See class document of ReportViewParams.
class ReportViewParamsP: public ParameterGrp::ObserverType {
public:
    // Auto generated code. See class document of ReportViewParams.
    ParameterGrp::handle handle;

    // Auto generated code. See class document of ReportViewParams.
    std::unordered_map<const char *,void(*)(ReportViewParamsP*),App::CStringHasher,App::CStringHasher> funcs;

    // Auto generated code. See class document of ReportViewParams.
    boost::signals2::signal<void (const char*)> signalParamChanged;

    // Auto generated code. See class document of ReportViewParams.
    void signalAll()
    {
        signalParamChanged("checkShowReportViewOnWarning");
        signalParamChanged("checkShowReportViewOnError");
        signalParamChanged("checkShowReportViewOnNormalMessage");
        signalParamChanged("checkShowReportViewOnLogMessage");
        signalParamChanged("checkShowReportTimecode");
        signalParamChanged("LogMessageSize");
        signalParamChanged("CommandRedirect");
    }
    bool checkShowReportViewOnWarning; // Auto generated code. See class document of ReportViewParams.
    bool checkShowReportViewOnError; // Auto generated code. See class document of ReportViewParams.
    bool checkShowReportViewOnNormalMessage; // Auto generated code. See class document of ReportViewParams.
    bool checkShowReportViewOnLogMessage; // Auto generated code. See class document of ReportViewParams.
    bool checkShowReportTimecode; // Auto generated code. See class document of ReportViewParams.
    long LogMessageSize; // Auto generated code. See class document of ReportViewParams.
    QString CommandRedirect; // Auto generated code. See class document of ReportViewParams.

    // Auto generated code. See class document of ReportViewParams.
    ReportViewParamsP() {
        handle = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/OutputWindow");
        handle->Attach(this);

        checkShowReportViewOnWarning = handle->GetBool("checkShowReportViewOnWarning", true);
        funcs["checkShowReportViewOnWarning"] = &ReportViewParamsP::updatecheckShowReportViewOnWarning;
        checkShowReportViewOnError = handle->GetBool("checkShowReportViewOnError", true);
        funcs["checkShowReportViewOnError"] = &ReportViewParamsP::updatecheckShowReportViewOnError;
        checkShowReportViewOnNormalMessage = handle->GetBool("checkShowReportViewOnNormalMessage", false);
        funcs["checkShowReportViewOnNormalMessage"] = &ReportViewParamsP::updatecheckShowReportViewOnNormalMessage;
        checkShowReportViewOnLogMessage = handle->GetBool("checkShowReportViewOnLogMessage", false);
        funcs["checkShowReportViewOnLogMessage"] = &ReportViewParamsP::updatecheckShowReportViewOnLogMessage;
        checkShowReportTimecode = handle->GetBool("checkShowReportTimecode", true);
        funcs["checkShowReportTimecode"] = &ReportViewParamsP::updatecheckShowReportTimecode;
        LogMessageSize = handle->GetInt("LogMessageSize", 0);
        funcs["LogMessageSize"] = &ReportViewParamsP::updateLogMessageSize;
        CommandRedirect = QString::fromUtf8(handle->GetASCII("CommandRedirect", "").c_str());
        funcs["CommandRedirect"] = &ReportViewParamsP::updateCommandRedirect;
    }

    // Auto generated code. See class document of ReportViewParams.
    ~ReportViewParamsP() {
    }

    // Auto generated code. See class document of ReportViewParams.
    void OnChange(Base::Subject<const char*> &, const char* sReason) {
        if(!sReason)
            return;
        auto it = funcs.find(sReason);
        if(it == funcs.end())
            return;
        it->second(this);
        signalParamChanged(sReason);
    }


    // Auto generated code. See class document of ReportViewParams.
    static void updatecheckShowReportViewOnWarning(ReportViewParamsP *self) {
        self->checkShowReportViewOnWarning = self->handle->GetBool("checkShowReportViewOnWarning", true);
    }
    // Auto generated code. See class document of ReportViewParams.
    static void updatecheckShowReportViewOnError(ReportViewParamsP *self) {
        self->checkShowReportViewOnError = self->handle->GetBool("checkShowReportViewOnError", true);
    }
    // Auto generated code. See class document of ReportViewParams.
    static void updatecheckShowReportViewOnNormalMessage(ReportViewParamsP *self) {
        self->checkShowReportViewOnNormalMessage = self->handle->GetBool("checkShowReportViewOnNormalMessage", false);
    }
    // Auto generated code. See class document of ReportViewParams.
    static void updatecheckShowReportViewOnLogMessage(ReportViewParamsP *self) {
        self->checkShowReportViewOnLogMessage = self->handle->GetBool("checkShowReportViewOnLogMessage", false);
    }
    // Auto generated code. See class document of ReportViewParams.
    static void updatecheckShowReportTimecode(ReportViewParamsP *self) {
        self->checkShowReportTimecode = self->handle->GetBool("checkShowReportTimecode", true);
    }
    // Auto generated code. See class document of ReportViewParams.
    static void updateLogMessageSize(ReportViewParamsP *self) {
        self->LogMessageSize = self->handle->GetInt("LogMessageSize", 0);
    }
    // Auto generated code. See class document of ReportViewParams.
    static void updateCommandRedirect(ReportViewParamsP *self) {
        self->CommandRedirect = QString::fromUtf8(self->handle->GetASCII("CommandRedirect", "").c_str());
    }
};

// Auto generated code. See class document of ReportViewParams.
ReportViewParamsP *instance() {
    static ReportViewParamsP *inst = new ReportViewParamsP;
    return inst;
}

} // Anonymous namespace

// Auto generated code. See class document of ReportViewParams.
ParameterGrp::handle ReportViewParams::getHandle() {
    return instance()->handle;
}

// Auto generated code. See class document of ReportViewParams.
boost::signals2::signal<void (const char*)> &
ReportViewParams::signalParamChanged() {
    return instance()->signalParamChanged;
}

// Auto generated code. See class document of ReportViewParams.
void signalAll() {
    instance()->signalAll();
}

// Auto generated code. See class document of ReportViewParams.
const char *ReportViewParams::doccheckShowReportViewOnWarning() {
    return "";
}

// Auto generated code. See class document of ReportViewParams.
const bool & ReportViewParams::getcheckShowReportViewOnWarning() {
    return instance()->checkShowReportViewOnWarning;
}

// Auto generated code. See class document of ReportViewParams.
const bool & ReportViewParams::defaultcheckShowReportViewOnWarning() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::setcheckShowReportViewOnWarning(const bool &v) {
    instance()->handle->SetBool("checkShowReportViewOnWarning",v);
    instance()->checkShowReportViewOnWarning = v;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::removecheckShowReportViewOnWarning() {
    instance()->handle->RemoveBool("checkShowReportViewOnWarning");
}

// Auto generated code. See class document of ReportViewParams.
const char *ReportViewParams::doccheckShowReportViewOnError() {
    return "";
}

// Auto generated code. See class document of ReportViewParams.
const bool & ReportViewParams::getcheckShowReportViewOnError() {
    return instance()->checkShowReportViewOnError;
}

// Auto generated code. See class document of ReportViewParams.
const bool & ReportViewParams::defaultcheckShowReportViewOnError() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::setcheckShowReportViewOnError(const bool &v) {
    instance()->handle->SetBool("checkShowReportViewOnError",v);
    instance()->checkShowReportViewOnError = v;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::removecheckShowReportViewOnError() {
    instance()->handle->RemoveBool("checkShowReportViewOnError");
}

// Auto generated code. See class document of ReportViewParams.
const char *ReportViewParams::doccheckShowReportViewOnNormalMessage() {
    return "";
}

// Auto generated code. See class document of ReportViewParams.
const bool & ReportViewParams::getcheckShowReportViewOnNormalMessage() {
    return instance()->checkShowReportViewOnNormalMessage;
}

// Auto generated code. See class document of ReportViewParams.
const bool & ReportViewParams::defaultcheckShowReportViewOnNormalMessage() {
    const static bool def = false;
    return def;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::setcheckShowReportViewOnNormalMessage(const bool &v) {
    instance()->handle->SetBool("checkShowReportViewOnNormalMessage",v);
    instance()->checkShowReportViewOnNormalMessage = v;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::removecheckShowReportViewOnNormalMessage() {
    instance()->handle->RemoveBool("checkShowReportViewOnNormalMessage");
}

// Auto generated code. See class document of ReportViewParams.
const char *ReportViewParams::doccheckShowReportViewOnLogMessage() {
    return "";
}

// Auto generated code. See class document of ReportViewParams.
const bool & ReportViewParams::getcheckShowReportViewOnLogMessage() {
    return instance()->checkShowReportViewOnLogMessage;
}

// Auto generated code. See class document of ReportViewParams.
const bool & ReportViewParams::defaultcheckShowReportViewOnLogMessage() {
    const static bool def = false;
    return def;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::setcheckShowReportViewOnLogMessage(const bool &v) {
    instance()->handle->SetBool("checkShowReportViewOnLogMessage",v);
    instance()->checkShowReportViewOnLogMessage = v;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::removecheckShowReportViewOnLogMessage() {
    instance()->handle->RemoveBool("checkShowReportViewOnLogMessage");
}

// Auto generated code. See class document of ReportViewParams.
const char *ReportViewParams::doccheckShowReportTimecode() {
    return "";
}

// Auto generated code. See class document of ReportViewParams.
const bool & ReportViewParams::getcheckShowReportTimecode() {
    return instance()->checkShowReportTimecode;
}

// Auto generated code. See class document of ReportViewParams.
const bool & ReportViewParams::defaultcheckShowReportTimecode() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::setcheckShowReportTimecode(const bool &v) {
    instance()->handle->SetBool("checkShowReportTimecode",v);
    instance()->checkShowReportTimecode = v;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::removecheckShowReportTimecode() {
    instance()->handle->RemoveBool("checkShowReportTimecode");
}

// Auto generated code. See class document of ReportViewParams.
const char *ReportViewParams::docLogMessageSize() {
    return "";
}

// Auto generated code. See class document of ReportViewParams.
const long & ReportViewParams::getLogMessageSize() {
    return instance()->LogMessageSize;
}

// Auto generated code. See class document of ReportViewParams.
const long & ReportViewParams::defaultLogMessageSize() {
    const static long def = 0;
    return def;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::setLogMessageSize(const long &v) {
    instance()->handle->SetInt("LogMessageSize",v);
    instance()->LogMessageSize = v;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::removeLogMessageSize() {
    instance()->handle->RemoveInt("LogMessageSize");
}

// Auto generated code. See class document of ReportViewParams.
const char *ReportViewParams::docCommandRedirect() {
    return QT_TRANSLATE_NOOP("ReportViewParams",
"Prefix for marking python command in message to be redirected to Python console\n"
"This is used as a debug help for output command from external libraries");
}

// Auto generated code. See class document of ReportViewParams.
const QString & ReportViewParams::getCommandRedirect() {
    return instance()->CommandRedirect;
}

// Auto generated code. See class document of ReportViewParams.
const QString & ReportViewParams::defaultCommandRedirect() {
    const static QString def = QStringLiteral("");
    return def;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::setCommandRedirect(const QString &v) {
    instance()->handle->SetASCII("CommandRedirect",v.toUtf8().constData());
    instance()->CommandRedirect = v;
}

// Auto generated code. See class document of ReportViewParams.
void ReportViewParams::removeCommandRedirect() {
    instance()->handle->RemoveASCII("CommandRedirect");
}
//[[[end]]]

