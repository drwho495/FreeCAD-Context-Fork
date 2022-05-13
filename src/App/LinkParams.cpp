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
import LinkParams
LinkParams.define()
]]]*/

// Auto generated code. See class document of LinkParams.
#include <unordered_map>
#include <App/Application.h>
#include <App/DynamicProperty.h>
#include "LinkParams.h"
using namespace App;

namespace {

// Auto generated code. See class document of LinkParams.
class LinkParamsP: public ParameterGrp::ObserverType {
public:
    // Auto generated code. See class document of LinkParams.
    ParameterGrp::handle handle;

    // Auto generated code. See class document of LinkParams.
    std::unordered_map<const char *,void(*)(LinkParamsP*),App::CStringHasher,App::CStringHasher> funcs;

    

    bool HideScaleVector; // Auto generated code. See class document of LinkParams.
    bool CreateInPlace; // Auto generated code. See class document of LinkParams.
    bool CreateInContainer; // Auto generated code. See class document of LinkParams.
    std::string ActiveContainerKey; // Auto generated code. See class document of LinkParams.
    bool CopyOnChangeApplyToAll; // Auto generated code. See class document of LinkParams.

    // Auto generated code. See class document of LinkParams.
    LinkParamsP() {
        handle = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Link");
        handle->Attach(this);

        HideScaleVector = handle->GetBool("HideScaleVector", true);
        funcs["HideScaleVector"] = &LinkParamsP::updateHideScaleVector;
        CreateInPlace = handle->GetBool("CreateInPlace", true);
        funcs["CreateInPlace"] = &LinkParamsP::updateCreateInPlace;
        CreateInContainer = handle->GetBool("CreateInContainer", true);
        funcs["CreateInContainer"] = &LinkParamsP::updateCreateInContainer;
        ActiveContainerKey = handle->GetASCII("ActiveContainerKey", "");
        funcs["ActiveContainerKey"] = &LinkParamsP::updateActiveContainerKey;
        CopyOnChangeApplyToAll = handle->GetBool("CopyOnChangeApplyToAll", true);
        funcs["CopyOnChangeApplyToAll"] = &LinkParamsP::updateCopyOnChangeApplyToAll;
    }

    // Auto generated code. See class document of LinkParams.
    ~LinkParamsP() {
    }

    // Auto generated code. See class document of LinkParams.
    void OnChange(Base::Subject<const char*> &, const char* sReason) {
        if(!sReason)
            return;
        auto it = funcs.find(sReason);
        if(it == funcs.end())
            return;
        it->second(this);
        
    }


    // Auto generated code. See class document of LinkParams.
    static void updateHideScaleVector(LinkParamsP *self) {
        self->HideScaleVector = self->handle->GetBool("HideScaleVector", true);
    }
    // Auto generated code. See class document of LinkParams.
    static void updateCreateInPlace(LinkParamsP *self) {
        self->CreateInPlace = self->handle->GetBool("CreateInPlace", true);
    }
    // Auto generated code. See class document of LinkParams.
    static void updateCreateInContainer(LinkParamsP *self) {
        self->CreateInContainer = self->handle->GetBool("CreateInContainer", true);
    }
    // Auto generated code. See class document of LinkParams.
    static void updateActiveContainerKey(LinkParamsP *self) {
        self->ActiveContainerKey = self->handle->GetASCII("ActiveContainerKey", "");
    }
    // Auto generated code. See class document of LinkParams.
    static void updateCopyOnChangeApplyToAll(LinkParamsP *self) {
        self->CopyOnChangeApplyToAll = self->handle->GetBool("CopyOnChangeApplyToAll", true);
    }
};

// Auto generated code. See class document of LinkParams.
LinkParamsP *instance() {
    static LinkParamsP *inst = new LinkParamsP;
    return inst;
}

} // Anonymous namespace

// Auto generated code. See class document of LinkParams.
ParameterGrp::handle LinkParams::getHandle() {
    return instance()->handle;
}

// Auto generated code. See class document of LinkParams.
const char *LinkParams::docHideScaleVector() {
    return "";
}

// Auto generated code. See class document of LinkParams.
const bool & LinkParams::getHideScaleVector() {
    return instance()->HideScaleVector;
}

// Auto generated code. See class document of LinkParams.
const bool & LinkParams::defaultHideScaleVector() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of LinkParams.
void LinkParams::setHideScaleVector(const bool &v) {
    instance()->handle->SetBool("HideScaleVector",v);
    instance()->HideScaleVector = v;
}

// Auto generated code. See class document of LinkParams.
void LinkParams::removeHideScaleVector() {
    instance()->handle->RemoveBool("HideScaleVector");
}

// Auto generated code. See class document of LinkParams.
const char *LinkParams::docCreateInPlace() {
    return "";
}

// Auto generated code. See class document of LinkParams.
const bool & LinkParams::getCreateInPlace() {
    return instance()->CreateInPlace;
}

// Auto generated code. See class document of LinkParams.
const bool & LinkParams::defaultCreateInPlace() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of LinkParams.
void LinkParams::setCreateInPlace(const bool &v) {
    instance()->handle->SetBool("CreateInPlace",v);
    instance()->CreateInPlace = v;
}

// Auto generated code. See class document of LinkParams.
void LinkParams::removeCreateInPlace() {
    instance()->handle->RemoveBool("CreateInPlace");
}

// Auto generated code. See class document of LinkParams.
const char *LinkParams::docCreateInContainer() {
    return "";
}

// Auto generated code. See class document of LinkParams.
const bool & LinkParams::getCreateInContainer() {
    return instance()->CreateInContainer;
}

// Auto generated code. See class document of LinkParams.
const bool & LinkParams::defaultCreateInContainer() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of LinkParams.
void LinkParams::setCreateInContainer(const bool &v) {
    instance()->handle->SetBool("CreateInContainer",v);
    instance()->CreateInContainer = v;
}

// Auto generated code. See class document of LinkParams.
void LinkParams::removeCreateInContainer() {
    instance()->handle->RemoveBool("CreateInContainer");
}

// Auto generated code. See class document of LinkParams.
const char *LinkParams::docActiveContainerKey() {
    return "";
}

// Auto generated code. See class document of LinkParams.
const std::string & LinkParams::getActiveContainerKey() {
    return instance()->ActiveContainerKey;
}

// Auto generated code. See class document of LinkParams.
const std::string & LinkParams::defaultActiveContainerKey() {
    const static std::string def = "";
    return def;
}

// Auto generated code. See class document of LinkParams.
void LinkParams::setActiveContainerKey(const std::string &v) {
    instance()->handle->SetASCII("ActiveContainerKey",v);
    instance()->ActiveContainerKey = v;
}

// Auto generated code. See class document of LinkParams.
void LinkParams::removeActiveContainerKey() {
    instance()->handle->RemoveASCII("ActiveContainerKey");
}

// Auto generated code. See class document of LinkParams.
const char *LinkParams::docCopyOnChangeApplyToAll() {
    return "";
}

// Auto generated code. See class document of LinkParams.
const bool & LinkParams::getCopyOnChangeApplyToAll() {
    return instance()->CopyOnChangeApplyToAll;
}

// Auto generated code. See class document of LinkParams.
const bool & LinkParams::defaultCopyOnChangeApplyToAll() {
    const static bool def = true;
    return def;
}

// Auto generated code. See class document of LinkParams.
void LinkParams::setCopyOnChangeApplyToAll(const bool &v) {
    instance()->handle->SetBool("CopyOnChangeApplyToAll",v);
    instance()->CopyOnChangeApplyToAll = v;
}

// Auto generated code. See class document of LinkParams.
void LinkParams::removeCopyOnChangeApplyToAll() {
    instance()->handle->RemoveBool("CopyOnChangeApplyToAll");
}
//[[[end]]]

