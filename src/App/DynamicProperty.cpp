/***************************************************************************
 *   Copyright (c) 2009 Werner Mayer <wmayer[at]users.sourceforge.net>     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/


#include "PreCompiled.h"

#include <Base/Reader.h>
#include <Base/Tools.h>
#include <Base/Writer.h>

#include "Document.h"
#include "DocumentParams.h"
#include "DynamicProperty.h"
#include "Application.h"
#include "Property.h"
#include "PropertyContainer.h"


FC_LOG_LEVEL_INIT("Property",true,true)


using namespace App;


DynamicProperty::DynamicProperty() = default;

DynamicProperty::~DynamicProperty()
{
    clear();
}

void DynamicProperty::clear() {
    auto &index = props.get<0>();
    for(auto &v : index)
        delete v.property;
    index.clear();
}

void DynamicProperty::getPropertyList(std::vector<Property*> &List) const
{
    for (auto &v : props.get<0>())
        List.push_back(v.property);
}

void DynamicProperty::getPropertyNamedList(std::vector<std::pair<const char*, Property*> > &List) const
{
    for (auto &v : props.get<0>())
        List.emplace_back(v.getName(),v.property);
}

void DynamicProperty::getPropertyMap(std::map<std::string,Property*> &Map) const
{
    for (auto &v : props.get<0>())
        Map[v.name] = v.property;
}

Property *DynamicProperty::getDynamicPropertyByName(const char* name) const
{
    auto &index = props.get<0>();
    auto it = index.find(name);
    if (it != index.end())
        return it->property;
    return nullptr;
}

std::vector<std::string> DynamicProperty::getDynamicPropertyNames() const
{
    std::vector<std::string> names;
    auto &index = props.get<0>();
    names.reserve(index.size());
    for(auto &v : index)
        names.push_back(v.name);
    return names;
}

short DynamicProperty::getPropertyType(const Property* prop) const
{
    return prop?prop->getType():0;
}

short DynamicProperty::getPropertyType(const char *name) const
{
    auto &index = props.get<0>();
    auto it = index.find(name);
    if (it != index.end()) {
        short attr = it->attr;
        if (it->hidden)
            attr |= Prop_Hidden;
        if (it->readonly)
            attr |= Prop_ReadOnly;
        return attr;
    }
    return 0;
}

const char* DynamicProperty::getPropertyGroup(const Property* prop) const
{
    auto &index = props.get<1>();
    auto it = index.find(const_cast<Property*>(prop));
    if(it!=index.end())
        return it->group.c_str();
    return nullptr;
}

const char* DynamicProperty::getPropertyGroup(const char *name) const
{
    auto &index = props.get<0>();
    auto it = index.find(name);
    if (it != index.end())
        return it->group.c_str();
    return nullptr;
}

const char* DynamicProperty::getPropertyDocumentation(const Property* prop) const
{
    auto &index = props.get<1>();
    auto it = index.find(const_cast<Property*>(prop));
    if(it!=index.end())
        return it->getDoc();
    return nullptr;
}

const char* DynamicProperty::getPropertyDocumentation(const char *name) const
{
    auto &index = props.get<0>();
    auto it = index.find(name);
    if (it != index.end())
        return it->getDoc();
    return nullptr;
}

Property* DynamicProperty::addDynamicProperty(PropertyContainer &pc, const char* type,
        const char* name, const char* group, const char* doc, short attr, bool ro, bool hidden)
{
    return _addDynamicProperty(pc,type,name,group,doc,StringIDRef(),attr,ro,hidden);
}

Property* DynamicProperty::_addDynamicProperty(PropertyContainer &pc, const char* type, 
        const char* name, const char* group, const char* doc, StringIDRef docID, short attr, bool ro, bool hidden)
{
    if(!type)
        type = "<null>";

    std::string _name;

    if(DocumentParams::getAutoNameDynamicProperty()) {
        if(!name || !name[0])
            name = type;
        _name = getUniquePropertyName(pc,name);
        if(_name != name) {
            FC_WARN(pc.getFullName() << " rename dynamic property from '"
                    << name << "' to '" << _name << "'");
        }
        name = _name.c_str();
    } else if(!name)
        name = "<null>"; // setting a bad name to trigger exception

    auto prop = pc.getPropertyByName(name);
    if(prop && prop->getContainer()==&pc)
        FC_THROWM(Base::NameError, "Property " << pc.getFullName() << '.' << name << " already exists");

    if(Base::Tools::getIdentifier(name) != name) 
        FC_THROWM(Base::NameError, "Invalid property name '" << name << "'");

    Base::Type propType = Base::Type::getTypeIfDerivedFrom(type, App::Property::getClassTypeId(), true);
    if (propType.isBad()) {
        FC_THROWM(Base::TypeError, "Invalid type "
                << type << " for property " << pc.getFullName() << '.' << name);
    }

    void* propInstance = propType.createInstance();
    if (!propInstance) {
        FC_THROWM(Base::RuntimeError, "Failed to create property "
                << pc.getFullName() << '.' << name << " of type " << type);
    }

    Property* pcProperty = static_cast<Property*>(propInstance);


    if(!docID && doc && doc[0]) {
        auto document = pc.getOwnerDocument();
        if(document)
            docID = document->getHasher()->getID(doc);
    }
    auto res = props.get<0>().emplace(pcProperty,
            name, nullptr, group, doc, docID, attr, ro, hidden);

    pcProperty->setContainer(&pc);
    pcProperty->myName = res.first->name.c_str();

    if(ro)
        attr |= Prop_ReadOnly;
    if(hidden)
        attr |= Prop_Hidden;

    pcProperty->syncType(attr);
    pcProperty->_StatusBits.set((size_t)Property::PropDynamic);

    GetApplication().signalAppendDynamicProperty(*pcProperty);

    return pcProperty;
}

bool DynamicProperty::addProperty(Property *prop)
{
    if(!prop || !prop->hasName())
        return false;
    auto &index = props.get<0>();
    if(index.count(prop->getName()))
        return false;
    index.emplace(prop,std::string(),prop->getName(),
            prop->getGroup(),prop->getDocumentation(),
            StringIDRef(),prop->getType(),false,false);
    return true;
}

bool DynamicProperty::removeProperty(const Property *prop)
{
    auto &index = props.get<1>();
    auto it = index.find(const_cast<Property*>(prop));
    if (it != index.end()) {
        index.erase(it);
        return true;
    }
    return false;
}

bool DynamicProperty::removeDynamicProperty(const char* name)
{
    auto &index = props.get<0>();
    auto it = index.find(name);
    if (it != index.end()) {
        if(it->property->testStatus(Property::LockDynamic))
            throw Base::RuntimeError("property is locked");
        else if(!it->property->testStatus(Property::PropDynamic))
            throw Base::RuntimeError("property is not dynamic");
        Property *prop = it->property;
        GetApplication().signalRemoveDynamicProperty(*prop);

        // Handle possible recursive calls of removeDynamicProperty
        if (prop->myName) {
            Property::destroy(prop);
            index.erase(it);
            // memory of myName has been freed
            prop->myName = nullptr;
        }
        return true;
    }

    return false;
}

std::string DynamicProperty::getUniquePropertyName(PropertyContainer &pc, const char *Name) const
{
    std::string CleanName = Base::Tools::getIdentifier(Name);

    // name in use?
    std::map<std::string,Property*> objectProps;
    pc.getPropertyMap(objectProps);
    auto pos = objectProps.find(CleanName);

    if (pos == objectProps.end()) {
        // if not, name is OK
        return CleanName;
    }
    else {
        std::vector<std::string> names;
        names.reserve(objectProps.size());
        for (pos = objectProps.begin();pos != objectProps.end();++pos) {
            names.push_back(pos->first);
        }
        return Base::Tools::getUniqueName(CleanName, names);
    }
}

void DynamicProperty::save(const Property *prop, Base::Writer &writer) const
{
    auto &index = props.get<1>();
    auto it = index.find(const_cast<Property*>(prop));
    if(it != index.end()) {
        auto &data = *it;
        writer.Stream() << "\" group=\"" << Base::Persistence::encodeAttribute(data.group)
                        << "\" attr=\"" << data.attr 
                        << "\" ro=\"" << data.readonly
                        << "\" hide=\"" << data.hidden;
        if(writer.getFileVersion()>1 && data.docID)
            writer.Stream() << "\" docID=\"" << data.docID.value();
        else
            writer.Stream() << "\" doc=\"" << Base::Persistence::encodeAttribute(data.getDoc());
    }
}

Property *DynamicProperty::restore(PropertyContainer &pc,
        const char *PropName, const char *TypeName, Base::XMLReader &reader)
{
    if (!reader.hasAttribute("group"))
        return nullptr;

    short attribute = 0;
    bool readonly = false, hidden = false;
    const char *group=nullptr, *doc=nullptr, *attr=nullptr, *ro=nullptr, *hide=nullptr;
    StringIDRef docID;
    group = reader.getAttribute("group");
    if (reader.hasAttribute("doc"))
        doc = reader.getAttribute("doc");
    else if(reader.hasAttribute("docID")) {
        auto document = pc.getOwnerDocument();
        if(document)
            docID = document->getHasher()->getID(reader.getAttributeAsInteger("docID"));
    }
    if (reader.hasAttribute("attr")) {
        attr = reader.getAttribute("attr");
        if (attr) {
            std::istringstream str(attr);
            str >> attribute;
        }
    }
    if (reader.hasAttribute("ro")) {
        ro = reader.getAttribute("ro");
        if (ro) readonly = (ro[0]-48) != 0;
    }
    if (reader.hasAttribute("hide")) {
        hide = reader.getAttribute("hide");
        if (hide) hidden = (hide[0]-48) != 0;
    }

    return _addDynamicProperty(pc,TypeName, PropName, group, doc, docID, attribute, readonly, hidden);
}

DynamicProperty::PropData DynamicProperty::getDynamicPropertyData(const Property *prop) const
{
    auto &index = props.get<1>();
    auto it = index.find(const_cast<Property*>(prop));
    if(it != index.end())
        return *it;
    return PropData();
}

bool DynamicProperty::changeDynamicProperty(const Property *prop, const char *group, const char *doc) {
    auto &index = props.get<1>();
    auto it = index.find(const_cast<Property*>(prop));
    if (it == index.end())
        return false;
    if(group)
        it->group = group;
    if(doc) {
        auto container = prop->getContainer();
        if(container && container->getOwnerDocument()) {
            it->docID = container->getOwnerDocument()->getHasher()->getID(doc);
            it->doc.clear();
        } else {
            it->docID = StringIDRef();
            it->doc = doc;
        }
    }
    return true;
}

const char *DynamicProperty::getPropertyName(const Property *prop) const
{
    auto &index = props.get<1>();
    auto it = index.find(const_cast<Property*>(prop));
    if(it != index.end())
        return it->getName();
    return nullptr;
}

void DynamicProperty::beforeSave() const
{
    auto &index = props.get<0>();
    for(auto &v : index) 
        v.docID.mark();
}
