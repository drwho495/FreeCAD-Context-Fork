/****************************************************************************
 *   Copyright (c) 2017 Zheng, Lei (realthunder) <realthunder.dev@gmail.com>*
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

#ifndef _PreComp_
#endif

#include <boost/algorithm/string/predicate.hpp>
#include <boost/preprocessor/stringize.hpp>
#include "Application.h"
#include "Document.h"
#include "GeoFeatureGroupExtension.h"
#include "Link.h"
#include "LinkBaseExtensionPy.h"
#include <Base/Console.h>

FC_LOG_LEVEL_INIT("App::Link", true,true)

using namespace App;

EXTENSION_PROPERTY_SOURCE(App::LinkBaseExtension, App::DocumentObjectExtension)

LinkBaseExtension::LinkBaseExtension(void)
    :lastSubname(0),enableLabelCache(false),myOwner(0)
{
    initExtensionType(LinkBaseExtension::getExtensionClassTypeId());
    EXTENSION_ADD_PROPERTY_TYPE(_LinkRecomputed, (false), " Link", 
            PropertyType(Prop_Hidden|Prop_Transient),0);
    props.resize(PropMax,0);
}

LinkBaseExtension::~LinkBaseExtension()
{
}

PyObject* LinkBaseExtension::getExtensionPyObject(void) {
    if (ExtensionPythonObject.is(Py::_None())){
        // ref counter is set to 1
        ExtensionPythonObject = Py::Object(new LinkBaseExtensionPy(this),true);
    }
    return Py::new_reference_to(ExtensionPythonObject);
}

const std::vector<LinkBaseExtension::PropInfo> &LinkBaseExtension::getPropertyInfo() const {
    static std::vector<LinkBaseExtension::PropInfo> PropsInfo;
    if(PropsInfo.empty()) {
        BOOST_PP_SEQ_FOR_EACH(LINK_PROP_INFO,PropsInfo,LINK_PARAMS);
    }
    return PropsInfo;
}

const LinkBaseExtension::PropInfoMap &LinkBaseExtension::getPropertyInfoMap() const {
    static PropInfoMap PropsMap;
    if(PropsMap.empty()) {
        const auto &infos = getPropertyInfo();
        for(const auto &info : infos) 
            PropsMap[info.name] = info;
    }
    return PropsMap;
}

Property *LinkBaseExtension::getProperty(int idx) {
    if(idx>=0 && idx<(int)props.size())
        return props[idx];
    return 0;
}

Property *LinkBaseExtension::getProperty(const char *name) {
    const auto &info = getPropertyInfoMap();
    auto it = info.find(name);
    if(it == info.end())
        return 0;
    return getProperty(it->second.index);
}

void LinkBaseExtension::setProperty(int idx, Property *prop) {
    const auto &infos = getPropertyInfo();
    if(idx<0 || idx>=(int)infos.size())
        LINK_THROW(Base::RuntimeError,"App::LinkBaseExtension: property index out of range");
    if(!prop) {
        if(props[idx]) {
            props[idx]->setStatus(Property::LockDynamic,false);
            props[idx] = 0;
        }
        return;
    }
    if(!prop->isDerivedFrom(infos[idx].type)) {
        std::ostringstream str;
        str << "App::LinkBaseExtension: expected property type '" << 
            infos[idx].type.getName() << "', instead of '" << 
            prop->getClassTypeId().getName() << "'";
        LINK_THROW(Base::TypeError,str.str().c_str());
    }

    props[idx] = prop;
    props[idx]->setStatus(Property::LockDynamic,true);

    switch(idx) {
    case PropLinkMode: {
        static const char *linkModeEnums[] = {"None","Auto Delete","Auto Link","Auto Unlink",0};
        auto propLinkMode = dynamic_cast<PropertyEnumeration*>(prop);
        if(!propLinkMode->getEnums())
            propLinkMode->setEnums(linkModeEnums);
        break;
    } case PropLinkTransform:
    case PropLinkPlacement:
    case PropPlacement:
        if(getLinkTransformProperty() &&
           getLinkPlacementProperty() &&
           getPlacementProperty())
        {
            bool transform = getLinkTransformValue();
            getPlacementProperty()->setStatus(Property::Hidden,transform);
            getLinkPlacementProperty()->setStatus(Property::Hidden,!transform);
        }
        break;
    case PropElementList:
        getElementListProperty()->setStatus(Property::Hidden,true);
        // fall through
    case PropLinkedObject:
        // Make ElementList as read-only if we are not a group (i.e. having
        // LinkedObject property), because it is for holding array elements.
        if(getElementListProperty())
            getElementListProperty()->setStatus(
                    Property::Immutable,getLinkedObjectProperty()!=0);
        break;
    case PropVisibilityList:
        getVisibilityListProperty()->setStatus(Property::Immutable,true);
        getVisibilityListProperty()->setStatus(Property::Hidden,true);
        break;
    }

    if(FC_LOG_INSTANCE.isEnabled(FC_LOGLEVEL_TRACE)) {
        const char *propName;
        if(!prop) 
            propName = "<null>";
        else if(prop->getContainer())
            propName = prop->getName();
        else 
            propName = extensionGetPropertyName(prop);
        if(!propName) 
            propName = "?";
        FC_TRACE("set property " << infos[idx].name << ": " << propName);
    }
}

App::DocumentObjectExecReturn *LinkBaseExtension::extensionExecute(void) {
    // The actual value of LinkRecompouted is not important, just to notify view
    // provider that the link (in fact, its dependents, i.e. linked ones) have
    // recomputed.
    _LinkRecomputed.touch();

    if(getLinkedObjectProperty() && !getTrueLinkedObject(true))
        return new App::DocumentObjectExecReturn("Link broken");
    return inherited::extensionExecute();
}

short LinkBaseExtension::extensionMustExecute(void) {
    auto link = getLink();
    if(!link) return 0;
    return link->mustExecute();
}

bool LinkBaseExtension::hasElements() const {
    auto propElements = getElementListProperty();
    return propElements && propElements->getSize();
}

bool LinkBaseExtension::extensionHasChildElement() const {
    if(hasElements())
        return true;
    if(getElementCountValue())
        return false;
    DocumentObject *linked = getTrueLinkedObject(true);
    if(linked) {
        if(linked->hasChildElement())
            return true;
        // return linked->hasExtension(App::GroupExtension::getExtensionClassTypeId(),true);
    }
    return false;
}

int LinkBaseExtension::extensionSetElementVisible(const char *element, bool visible) {
    int index = getShowElementValue()?getElementIndex(element):getArrayIndex(element);
    if(index>=0) {
        auto propElementVis = getVisibilityListProperty();
        if(!propElementVis || !element || !element[0]) 
            return -1;
        if(propElementVis->getSize()<=index) {
            if(visible) return 1;
            propElementVis->setSize(index+1, true);
        }
        propElementVis->setStatus(Property::User3,true);
        propElementVis->set1Value(index,visible,true);
        propElementVis->setStatus(Property::User3,false);
        const auto &elements = getElementListValue();
        if(index<(int)elements.size()) {
            if(!visible)
                myHiddenElements.insert(elements[index]);
            else
                myHiddenElements.erase(elements[index]);
        }
        return 1;
    }
    DocumentObject *linked = getTrueLinkedObject(true);
    if(linked)
        return linked->setElementVisible(element,visible);
    return -1;
}

int LinkBaseExtension::extensionIsElementVisible(const char *element) {
    int index = getShowElementValue()?getElementIndex(element):getArrayIndex(element);
    if(index>=0) {
        auto propElementVis = getVisibilityListProperty();
        if(propElementVis) {
            if(propElementVis->getSize()<=index || propElementVis->getValues()[index])
                return 1;
            return 0;
        }
        return -1;
    }
    DocumentObject *linked = getTrueLinkedObject(true);
    if(linked)
        return linked->isElementVisible(element);
    return -1;
}

const DocumentObject *LinkBaseExtension::getContainer() const {
    auto ext = getExtendedContainer();
    if(!ext || !ext->isDerivedFrom(DocumentObject::getClassTypeId()))
        LINK_THROW(Base::RuntimeError,"Link: container not derived from document object");
    return static_cast<const DocumentObject *>(ext);
}

DocumentObject *LinkBaseExtension::getContainer(){
    auto ext = getExtendedContainer();
    if(!ext || !ext->isDerivedFrom(DocumentObject::getClassTypeId()))
        LINK_THROW(Base::RuntimeError,"Link: container not derived from document object");
    return static_cast<DocumentObject *>(ext);
}

DocumentObject *LinkBaseExtension::getLink(int depth) const{
    GetApplication().checkLinkDepth(depth);
    if(getLinkedObjectProperty())
        return getLinkedObjectValue();
    return 0;
}

int LinkBaseExtension::getArrayIndex(const char *subname, const char **psubname) {
    if(!subname) return -1;
    const char *dot = strchr(subname,'.');
    if(!dot) dot= subname+strlen(subname);
    if(dot == subname) return -1;
    int idx = 0;
    for(const char *c=subname;c!=dot;++c) {
        if(!isdigit(*c)) return -1;
        idx = idx*10 + *c -'0';
    }
    if(psubname) {
        if(*dot)
            *psubname = dot+1;
        else
            *psubname = dot;
    }
    return idx;
}

int LinkBaseExtension::getElementIndex(const char *subname, const char **psubname) const {
    if(!subname) return -1;
    int idx = -1;
    const char *dot = strchr(subname,'.');
    if(!dot) dot= subname+strlen(subname);

    if(isdigit(subname[0])) {
        // If the name start with digits, treat as index reference
        idx = getArrayIndex(subname,0);
        if(idx<0) return -1;
        if(getElementCountProperty()) {
            if(idx>=getElementCountValue())
                return -1;
        }else if(!getElementListProperty() || idx>=getElementListProperty()->getSize())
            return -1;
    }else if(!getShowElementValue() && getElementCountValue()) {
        // If elements are collapsed, we check first for LinkElement naming
        // pattern, which is the owner object name + "_i" + index
        const char *name = subname[0]=='$'?subname+1:subname;
        auto owner = getContainer();
        if(owner && owner->getNameInDocument()) {
            std::string ownerName(owner->getNameInDocument());
            ownerName += '_';
            if(boost::algorithm::starts_with(name,ownerName.c_str())) {
                for(const char *txt=dot-1;txt>=name+ownerName.size();--txt) {
                    if(*txt == 'i') {
                        idx = getArrayIndex(txt+1,0);
                        if(idx<0 || idx>=getElementCountValue())
                            idx = -1;
                        break;
                    }
                    if(!isdigit(*txt))
                        break;
                }
            }
        }
        if(idx<0) {
            // Then check for the actual linked object's name or label, and
            // redirect that reference to the first array element
            auto linked = getTrueLinkedObject(true);
            if(!linked || !linked->getNameInDocument()) return -1;
            if(subname[0]=='$') {
                if(std::string(subname+1,dot-subname-1) != linked->Label.getValue())
                    return -1;
            }else if(std::string(subname,dot-subname)!=linked->getNameInDocument())
                return -1;
            idx = 0;
        }
    }else if(subname[0]!='$') {
        // Try search by element objects' name
        auto prop = getElementListProperty();
        if(!prop || !prop->find(std::string(subname,dot-subname).c_str(),&idx))
            return -1;
    }else {
        // Try search by label if the reference name start with '$'
        ++subname;
        std::string name(subname,dot-subname);
        const auto &elements = getElementListValue();
        if(enableLabelCache) {
            auto it = myLabelCache.find(name);
            if(it == myLabelCache.end())
                return -1;
            idx = it->second;
        }else{
            idx = 0;
            for(auto element : elements) {
                if(element->Label.getStrValue() == name)
                    break;
                ++idx;
            }
        }
        if(idx<0 || idx>=(int)elements.size())
            return -1;
    }
    if(psubname)
        *psubname = dot[0]?dot+1:dot;
    return idx;
}

Base::Vector3d LinkBaseExtension::getScaleVector() const {
    if(getScaleVectorProperty())
        return getScaleVectorValue();
    double s = getScaleValue();
    return Base::Vector3d(s,s,s);
}

Base::Matrix4D LinkBaseExtension::getTransform(bool transform) const {
    Base::Matrix4D mat;
    if(transform) {
        if(getLinkPlacementProperty())
            mat = getLinkPlacementValue().toMatrix();
        else if(getPlacementProperty())
            mat = getPlacementValue().toMatrix();
    }
    if(getScaleProperty() || getScaleVectorProperty()) {
        Base::Matrix4D s;
        s.scale(getScaleVector());
        mat *= s;
    }
    return mat;
}

bool LinkBaseExtension::extensionGetSubObjects(std::vector<std::string> &ret) const {
    if(hasElements()) {
        for(auto obj : getElementListValue()) {
            if(obj && obj->getNameInDocument()) {
                std::string name(obj->getNameInDocument());
                name+='.';
                ret.push_back(name);
            }
        }
        return true;
    }
    if(mySubElement.empty() && getSubElementsValue().empty()) {
        DocumentObject *linked = getTrueLinkedObject(true);
        if(linked) {
            if(!getElementCountValue())
                ret = linked->getSubObjects();
            else{
                char index[30];
                for(int i=0,count=getElementCountValue();i<count;++i) {
                    snprintf(index,sizeof(index),"%d.",i);
                    ret.push_back(index);
                }
            }
        }
    }
    return true;
}

bool LinkBaseExtension::extensionGetSubObject(DocumentObject *&ret, const char *subname, 
        PyObject **pyObj, Base::Matrix4D *mat, bool transform, int depth) const 
{
    ret = 0;
    if(mat) *mat *= getTransform(transform);
    auto obj = getContainer();
    if(!subname || !subname[0]) {
        ret = const_cast<DocumentObject*>(obj);
        if(!hasElements() && !getElementCountValue() && pyObj) {
            Base::Matrix4D matNext;
            if(mat) matNext = *mat;
            auto linked = getTrueLinkedObject(true,mat?&matNext:0,depth);
            if(linked) {
                if(mat) *mat = matNext;
                linked->getSubObject(mySubElement.c_str(),pyObj,mat,false,depth+1);
            }
        }
        return true;
    }

    DocumentObject *element = 0;
    bool isElement = false;
    int idx = getElementIndex(subname,&subname);
    if(idx>=0) {
        if(hasElements()) {
            const auto &elements = getElementListValue();
            if(idx>=(int)elements.size() || !elements[idx] || !elements[idx]->getNameInDocument())
                return true;
            if(!subname || !subname[0])
                subname = mySubElement.c_str();
            ret = elements[idx]->getSubObject(subname,pyObj,mat,true,depth+1);
            // do not resolve the link if this element is the last referenced object
            if(!subname || !strchr(subname,'.'))
                ret = elements[idx];
            return true;
        }

        int elementCount = getElementCountValue();
        if(idx>=elementCount)
            return true;
        isElement = true;
        if(mat) {
            auto placementList = getPlacementListProperty();
            if(placementList && placementList->getSize()>idx)
                *mat *= (*placementList)[idx].toMatrix();
            auto scaleList = getScaleListProperty();
            if(scaleList && scaleList->getSize()>idx) {
                Base::Matrix4D s;
                s.scale((*scaleList)[idx]);
                *mat *= s;
            }
        }
    }

    auto linked = getTrueLinkedObject(true,mat,depth);
    if(!linked) 
        return true;

    Base::Matrix4D matNext;
    if(mat) matNext = *mat;
    if(!subname || !subname[0])
        subname = mySubElement.c_str();
    ret = linked->getSubObject(subname,pyObj,mat?&matNext:0,false,depth+1);
    if(ret) {
        // do not resolve the link if we are the last referenced object
        if(subname && strchr(subname,'.')) {
            if(mat)
                *mat = matNext;
        }else if(element)
            ret = element;
        else if(!isElement)
            ret = const_cast<DocumentObject*>(obj);
        else if(mat)
            *mat = matNext;
    }
    return true;
}

void LinkBaseExtension::onExtendedUnsetupObject() {
    if(!getElementListProperty())
        return;
    auto objs = getElementListValue();
    getElementListProperty()->setValue();
    for(auto obj : objs) 
        detachElement(obj);
}

DocumentObject *LinkBaseExtension::getTrueLinkedObject(
        bool recurse, Base::Matrix4D *mat, int depth) const
{
    auto ret = getLink(depth);
    if(!ret) return 0;
    bool transform = linkTransform();
    const char *subname = getSubName();
    if(subname) {
        ret = ret->getSubObject(subname,0,mat,transform,depth+1);
        transform = false;
    }
    if(ret && recurse)
        ret = ret->getLinkedObject(recurse,mat,transform,depth+1);
    if(ret && !ret->getNameInDocument())
        return 0;
    return ret;
}

bool LinkBaseExtension::extensionGetLinkedObject(DocumentObject *&ret, 
        bool recurse, Base::Matrix4D *mat, bool transform, int depth) const
{
    if(mat) 
        *mat *= getTransform(transform);
    ret = 0;
    if(!getElementCountValue())
        ret = getTrueLinkedObject(recurse,mat,depth);
    if(!ret)
        ret = const_cast<DocumentObject*>(getContainer());
    // always return true to indicate we've handled getLinkObject() call
    return true;
}

void LinkBaseExtension::extensionOnChanged(const Property *prop) {
    auto parent = getContainer();
    if(parent && !parent->isRestoring() && prop && !prop->testStatus(Property::User3))
        update(parent,prop);
    inherited::extensionOnChanged(prop);
}

void LinkBaseExtension::parseSubName() const {
    auto xlink = dynamic_cast<const PropertyXLink*>(getLinkedObjectProperty());
    const char* subname = xlink?xlink->getSubName():0;
    // For performance reason, we don't compare the content of the string. Just
    // check the pointer value as a heck. This is not reliable as the string may
    // not be re-allocated every time. However, we'll do force update in
    // onChanged()
    if(subname == lastSubname)
        return;
    lastSubname = subname;
    mySubName.clear();
    mySubElement.clear();
    if(!subname || !subname[0])
        return;
    const char *dot = strrchr(subname,'.');
    if(!dot)
        dot = subname;
    else
        ++dot;
    auto obj = xlink->getValue();
    if(!dot[0] || !obj || !obj->getNameInDocument()) {
        mySubName = subname;
        return;
    }

    // not ending with dot, check for sub element
    auto sobj = obj->getSubObject(subname);
    if(!sobj) {
        std::string sub(subname);
        sub += '.';
        if(obj->getSubObject(sub.c_str())) {
            mySubName = sub;
            return;
        }else
            FC_WARN("failed to find sub-element in '" << subname 
                    <<"' of object '" << obj->getNameInDocument() << "'");
    }
    mySubName = std::string(subname,dot-subname);
    mySubElement = dot;
}

void LinkBaseExtension::update(App::DocumentObject *parent, const Property *prop) {
    if(!prop) return;
    if(prop == getLinkPlacementProperty() || prop == getPlacementProperty()) {
        auto src = getLinkPlacementProperty();
        auto dst = getPlacementProperty();
        if(src!=prop) std::swap(src,dst);
        if(src && dst) {
            dst->setStatus(Property::User3,true);
            dst->setValue(src->getValue());
            dst->setStatus(Property::User3,false);
        }
    }else if(prop == getShowElementProperty()) {
        const auto &objs = getElementListValue();
        if(getShowElementValue()) 
            update(parent,getElementCountProperty());
        else if(objs.size()){
            // preseve element properties in ourself
            std::vector<Base::Placement> placements;
            placements.reserve(objs.size());
            std::vector<Base::Vector3d> scales;
            scales.reserve(objs.size());
            for(size_t i=0;i<objs.size();++i) {
                auto element = dynamic_cast<LinkElement*>(objs[i]);
                if(element) {
                    placements.push_back(element->Placement.getValue());
                    scales.push_back(element->getScaleVector());
                }else{
                    placements.emplace_back();
                    scales.emplace_back(1,1,1);
                }
            }
            if(getPlacementListProperty()) {
                getPlacementListProperty()->setStatus(Property::User3,getScaleListProperty()!=0);
                getPlacementListProperty()->setValue(placements);
                getPlacementListProperty()->setStatus(Property::User3,false);
            }
            if(getScaleListProperty())
                getScaleListProperty()->setValue(scales);

            // About to remove all elements
            //
            // NOTE: there is an assumption here that signalChangeObject will
            // be trigged before the call here (i.e. through
            // extensionOnChanged()), which is the default behavior on
            // DocumentObject::onChanged(). This ensures the view provider has
            // a chance to save the element view provider's properties.  This
            // assumption may be broken if someone override onChanged().
            getElementListProperty()->setValues(std::vector<App::DocumentObject*>());

            for(auto obj : objs) {
                if(obj && obj->getNameInDocument())
                    obj->getDocument()->removeObject(obj->getNameInDocument());
            }
        }
    }else if(prop == getElementCountProperty()) {
        size_t elementCount = getElementCountValue()<0?0:(size_t)getElementCountValue();

        auto propVis = getVisibilityListProperty();
        if(propVis) {
            if(propVis->getSize()>(int)elementCount)
                propVis->setSize(getElementCountValue(),true);
        }

        if(!getShowElementValue()) {
            if(getScaleListProperty()) {
                auto scales = getScaleListValue();
                scales.resize(elementCount,Base::Vector3d(1,1,1));
                getScaleListProperty()->setStatus(Property::User3,getPlacementListProperty()!=0);
                getScaleListProperty()->setValue(scales);
                getScaleListProperty()->setStatus(Property::User3,false);
            }
            if(getPlacementListProperty()) {
                auto placements = getPlacementListValue();
                if(placements.size()<elementCount) {
                    for(size_t i=placements.size();i<elementCount;++i)
                        placements.emplace_back(Base::Vector3d(i%10,(i/10)%10,i/100),Base::Rotation());
                }else
                    placements.resize(elementCount);
                getPlacementListProperty()->setValue(placements);
            }
        }else if(getElementListProperty()) {
            const auto &placementList = getPlacementListValue();
            const auto &scaleList = getScaleListValue();
            auto objs = getElementListValue();
            if(elementCount>objs.size()) {
                std::string name = parent->getNameInDocument();
                auto doc = parent->getDocument();
                name += "_i";
                name = doc->getUniqueObjectName(name.c_str());
                if(name[name.size()-1] != 'i')
                    name += "_i";
                auto offset = name.size();
                for(size_t i=objs.size();i<elementCount;++i) {
                    name.resize(offset);
                    name += std::to_string(i);

                    // It is possible to have orphan LinkElement here due to,
                    // for example, undo and redo. So we try to re-claim the
                    // children element first.
                    auto obj = dynamic_cast<LinkElement*>(doc->getObject(name.c_str()));
                    if(obj && (!obj->myOwner || obj->myOwner==this))
                        obj->Visibility.setValue(false);
                    else{
                        obj = new LinkElement;
                        Base::Placement pla(Base::Vector3d(i%10,(i/10)%10,i/100),Base::Rotation());
                        obj->Placement.setValue(pla);
                        parent->getDocument()->addObject(obj,name.c_str());
                    }
                    objs.push_back(obj);
                }
                if(getPlacementListProperty()) 
                    getPlacementListProperty()->setSize(0);
                if(getScaleListProperty())
                    getScaleListProperty()->setSize(0);

                getElementListProperty()->setValue(objs);

            }else if(elementCount<objs.size()){
                std::vector<App::DocumentObject*> tmpObjs;
                while(objs.size()>elementCount) {
                    auto element = dynamic_cast<LinkElement*>(objs.back());
                    if(element && element->myOwner==this)
                        tmpObjs.push_back(objs.back());
                    objs.pop_back();
                }
                getElementListProperty()->setValue(objs);
                for(auto obj : tmpObjs) {
                    if(obj && obj->getNameInDocument())
                        obj->getDocument()->removeObject(obj->getNameInDocument());
                }
            }
        }
    }else if(prop == getVisibilityListProperty()) {
        if(getShowElementValue()) {
            const auto &elements = getElementListValue();
            const auto &vis = getVisibilityListValue();
            myHiddenElements.clear();
            for(size_t i=0;i<vis.size();++i) {
                if(i>=elements.size())
                    break;
                if(!vis[i])
                    myHiddenElements.insert(elements[i]);
            }
        }
    }else if(prop == getElementListProperty()) {
        const auto &elements = getElementListValue();

        if(enableLabelCache)
            cacheChildLabel();

        // Element list changed, we need to sychrnoize VisibilityList.
        if(getShowElementValue() && getVisibilityListProperty()) {
            if(parent->getDocument()->isPerformingTransaction()) {
                update(parent,getVisibilityListProperty());
            }else{
                boost::dynamic_bitset<> vis;
                vis.resize(elements.size(),true);
                std::set<const App::DocumentObject *> hiddenElements;
                for(size_t i=0;i<elements.size();++i) {
                    if(myHiddenElements.find(elements[i])!=myHiddenElements.end()) {
                        hiddenElements.insert(elements[i]);
                        vis[i] = false;
                    }
                }
                myHiddenElements.swap(hiddenElements);
                if(vis != getVisibilityListValue()) {
                    auto propVis = getVisibilityListProperty();
                    propVis->setStatus(Property::User3,true);
                    propVis->setValue(vis);
                    propVis->setStatus(Property::User3,false);
                }
            }
        }
        syncElementList();
        if(getShowElementValue() && getElementCountProperty() && 
            getElementCountValue()!=(int)elements.size())
        {
            getElementCountProperty()->setValue(elements.size());
        }
    }else if(prop == getLinkedObjectProperty()) {
        lastSubname = 0;
        parseSubName();
        syncElementList();
    }else if(prop == getSubElementsProperty()) {
        syncElementList();
    }else if(prop == getLinkTransformProperty()) {
        auto linkPlacement = getLinkPlacementProperty();
        auto placement = getPlacementProperty();
        if(linkPlacement && placement) {
            bool transform = getLinkTransformValue();
            placement->setStatus(Property::Hidden,transform);
            linkPlacement->setStatus(Property::Hidden,!transform);
        }
        syncElementList();
    }
}

void LinkBaseExtension::cacheChildLabel(bool enable) {
    enableLabelCache = enable;
    myLabelCache.clear();
    if(!enable)
        return;

    const auto &elements = getElementListValue();
    int idx = 0;
    for(auto child : elements) {
        if(child && child->getNameInDocument())
            myLabelCache[child->Label.getStrValue()] = idx;
        ++idx;
    }
}

bool LinkBaseExtension::linkTransform() const {
    if(!getLinkTransformProperty() && 
       !getLinkPlacementProperty() && 
       !getPlacementProperty())
        return true;
    return getLinkTransformValue();
}

void LinkBaseExtension::syncElementList() {
    const auto &subElements = getSubElementsValue();
    auto sub = getSubElementsProperty();
    auto transform = getLinkTransformProperty();
    auto link = getLinkedObjectProperty();
    auto xlink = dynamic_cast<const PropertyXLink*>(link);
    std::string subname;
    if(xlink) 
        subname = xlink->getSubName();

    auto elements = getElementListValue();
    for(size_t i=0;i<elements.size();++i) {
        auto element = dynamic_cast<LinkElement*>(elements[i]);
        if(!element || (element->myOwner && element->myOwner!=this)) 
            continue;

        element->myOwner = this;

        element->SubElements.setStatus(Property::Hidden,sub!=0);
        element->SubElements.setStatus(Property::Immutable,sub!=0);

        element->LinkTransform.setStatus(Property::Hidden,transform!=0);
        element->LinkTransform.setStatus(Property::Immutable,transform!=0);
        if(transform && element->LinkTransform.getValue()!=transform->getValue())
            element->LinkTransform.setValue(transform->getValue());

        element->LinkedObject.setStatus(Property::Hidden,link!=0);
        element->LinkedObject.setStatus(Property::Immutable,link!=0);
        if(link) {
            if(element->LinkedObject.getValue()!=link->getValue() ||
               subname != element->LinkedObject.getSubName() ||
               subElements != element->SubElements.getValue())
            {
                element->setLink(-1,link->getValue(),subname.c_str(),subElements);
            }
        }
    }
}

void LinkBaseExtension::onExtendedDocumentRestored() {
    inherited::onExtendedDocumentRestored();
    auto parent = getContainer();
    myHiddenElements.clear();
    if(parent) {
        update(parent,getVisibilityListProperty());
        update(parent,getLinkedObjectProperty());
        update(parent,getElementListProperty());
    }
}

void LinkBaseExtension::setLink(int index, DocumentObject *obj, 
    const char *subname, const std::vector<std::string> &subElements) 
{
    auto parent = getContainer();
    if(!parent)
        LINK_THROW(Base::RuntimeError,"No parent container");

    auto linkProp = getLinkedObjectProperty();

    // If we are a group (i.e. no LinkObject property), and the index is
    // negative with a non-zero 'obj' assignment, we treat this as group
    // expansion by changing the index to one pass the existing group size
    if(index<0 && obj && !linkProp && getElementListProperty())
        index = getElementListProperty()->getSize();

    if(index>=0) {
        // LinkGroup assignment

        if(linkProp || !getElementListProperty())
            LINK_THROW(Base::RuntimeError,"Cannot set link element");

        DocumentObject *old = 0;
        const auto &elements = getElementListValue();
        if(!obj) {
            if(index>=(int)elements.size())
                LINK_THROW(Base::ValueError,"Link element index out of bound");
            std::vector<DocumentObject*> objs;
            old = elements[index];
            for(int i=0;i<(int)elements.size();++i) {
                if(i!=index)
                    objs.push_back(elements[i]);
            }
            getElementListProperty()->setValue(objs);
        }else if(!obj->getNameInDocument())
            LINK_THROW(Base::ValueError,"Invalid object");
        else{
            if(index>(int)elements.size())
                LINK_THROW(Base::ValueError,"Link element index out of bound");

            if(index < (int)elements.size())
                old = elements[index];

            if(getLinkModeValue()>=LinkModeAutoLink ||
               (subname && subname[0]) ||
               subElements.size() ||
               obj->getDocument()!=parent->getDocument() ||
               getElementListProperty()->find(obj->getNameInDocument()))
            {
                std::string name = parent->getDocument()->getUniqueObjectName("Link");
                auto link = new Link;
                link->myOwner = this;
                parent->getDocument()->addObject(link,name.c_str());
                link->setLink(-1,obj,subname,subElements);
                auto linked = link->getTrueLinkedObject(true);
                if(linked)
                    link->Label.setValue(linked->Label.getValue());
                if(getLinkModeValue()<LinkModeAutoLink)
                    link->LinkTransform.setValue(true);
                link->Visibility.setValue(false);
                obj = link;
            }

            if(old == obj)
                return;

            getElementListProperty()->set1Value(index,obj,true);
        }
        detachElement(old);
        return;
    }

    if(!linkProp) {
        // Reaching here means, we are group (i.e. no LinkedObject), and
        // index<0, and 'obj' is zero. We shall clear the whole group

        if(obj || getElementListProperty())
            LINK_THROW(Base::RuntimeError,"No PropertyLink or PropertyLinkList configured");

        auto objs = getElementListValue();
        getElementListProperty()->setValue();
        for(auto obj : objs) 
            detachElement(obj);
        return;
    }

    // Here means we are assigning a Link

    auto xlink = dynamic_cast<PropertyXLink*>(linkProp);
    auto subElementProp = getSubElementsProperty();
    if(subElements.size() && !subElementProp)
        LINK_THROW(Base::RuntimeError,"No SubElements Property configured");

    if(obj) {
        if(!obj->getNameInDocument())
            LINK_THROW(Base::ValueError,"Invalid document object");
        if(!xlink) {
            if(parent && obj->getDocument()!=parent->getDocument())
                LINK_THROW(Base::ValueError,"Cannot link to external object without PropertyXLink");
        }
    }

    if(subname && subname[0] && !xlink)
        LINK_THROW(Base::RuntimeError,"SubName link requires PropertyXLink");

    if(subElementProp && subElements.size()) {
        subElementProp->setStatus(Property::User3, true);
        subElementProp->setValue(subElements);
        subElementProp->setStatus(Property::User3, false);
    }
    if(xlink)
        xlink->setValue(obj,subname,true);
    else
        linkProp->setValue(obj);
}

void LinkBaseExtension::detachElement(DocumentObject *obj) {
    if(!obj || !obj->getNameInDocument() || obj->isRemoving())
        return;
    auto ext = obj->getExtensionByType<LinkBaseExtension>(true);
    if(getLinkModeValue()==LinkModeAutoUnlink) {
        if(!ext || ext->myOwner!=this)
            return;
    }else if(getLinkModeValue()!=LinkModeAutoDelete) {
        if(ext && ext->myOwner==this)
            ext->myOwner = 0;
        return;
    }
    obj->getDocument()->removeObject(obj->getNameInDocument());
}

///////////////////////////////////////////////////////////////////////////////////////////

namespace App {
EXTENSION_PROPERTY_SOURCE_TEMPLATE(App::LinkBaseExtensionPython, App::LinkBaseExtension)

// explicit template instantiation
template class AppExport ExtensionPythonT<LinkBaseExtension>;

}

//////////////////////////////////////////////////////////////////////////////

EXTENSION_PROPERTY_SOURCE(App::LinkExtension, App::LinkBaseExtension)

LinkExtension::LinkExtension(void)
{
    initExtensionType(LinkExtension::getExtensionClassTypeId());

    LINK_PROPS_ADD_EXTENSION(LINK_PARAMS_EXT);
}

LinkExtension::~LinkExtension()
{
}

///////////////////////////////////////////////////////////////////////////////////////////

namespace App {
EXTENSION_PROPERTY_SOURCE_TEMPLATE(App::LinkExtensionPython, App::LinkExtension)

// explicit template instantiation
template class AppExport ExtensionPythonT<App::LinkExtension>;

}

///////////////////////////////////////////////////////////////////////////////////////////

PROPERTY_SOURCE_WITH_EXTENSIONS(App::Link, App::DocumentObject)

Link::Link() {
    LINK_PROPS_ADD(LINK_PARAMS_LINK);
    LinkExtension::initExtension(this);
    static const PropertyIntegerConstraint::Constraints s_constraints = {0,INT_MAX,1};
    ElementCount.setConstraints(&s_constraints);
}

bool Link::canLinkProperties() const {
    auto prop = dynamic_cast<const PropertyXLink*>(getLinkedObjectProperty());
    const char *subname;
    if(prop && (subname=prop->getSubName()) && *subname) {
        auto len = strlen(subname);
        // Do not link properties when we are linking to a sub-element (i.e.
        // vertex, edge or face)
        return subname[len-1]=='.';
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

namespace App {
PROPERTY_SOURCE_TEMPLATE(App::LinkPython, App::Link)
template<> const char* App::LinkPython::getViewProviderName(void) const {
    return "Gui::ViewProviderLinkPython";
}
template class AppExport FeaturePythonT<App::Link>;
}

//////////////////////////////////////////////////////////////////////////////////////////

PROPERTY_SOURCE_WITH_EXTENSIONS(App::LinkElement, App::DocumentObject)

LinkElement::LinkElement() {
    LINK_PROPS_ADD(LINK_PARAMS_ELEMENT);
    LinkBaseExtension::initExtension(this);
}

//////////////////////////////////////////////////////////////////////////////////////////

PROPERTY_SOURCE_WITH_EXTENSIONS(App::LinkGroup, App::DocumentObject)

LinkGroup::LinkGroup() {
    LINK_PROPS_ADD(LINK_PARAMS_GROUP);
    LinkBaseExtension::initExtension(this);
}

//////////////////////////////////////////////////////////////////////////////////////////

namespace App {
PROPERTY_SOURCE_TEMPLATE(App::LinkGroupPython, App::LinkGroup)
template<> const char* App::LinkGroupPython::getViewProviderName(void) const {
    return "Gui::ViewProviderLinkPython";
}
template class AppExport FeaturePythonT<App::LinkGroup>;
}
