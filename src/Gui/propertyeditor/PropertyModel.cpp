/***************************************************************************
 *   Copyright (c) 2004 Werner Mayer <wmayer[at]users.sourceforge.net>     *
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

#ifndef _PreComp_
# include <boost/algorithm/string/predicate.hpp>
#endif

#include "PropertyItem.h"
#include "PropertyModel.h"
#include "PropertyView.h"


using namespace Gui;
using namespace Gui::PropertyEditor;


/* TRANSLATOR Gui::PropertyEditor::PropertyModel */

PropertyModel::PropertyModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    rootItem = static_cast<PropertyItem*>(PropertyItem::create());
}

PropertyModel::~PropertyModel()
{
    delete rootItem;
}

QModelIndex PropertyModel::buddy ( const QModelIndex & index ) const
{
    if (index.column() == 1)
        return index;
    return index.sibling(index.row(), 1);
}

int PropertyModel::columnCount ( const QModelIndex & parent ) const
{
    // <property, value>, hence always 2
    if (parent.isValid())
        return static_cast<PropertyItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant PropertyModel::data ( const QModelIndex & index, int role ) const
{
    if (!index.isValid())
        return QVariant();

    auto item = static_cast<PropertyItem*>(index.internalPointer());
    return item->data(index.column(), role);
}

bool PropertyModel::setData(const QModelIndex& index, const QVariant & value, int role)
{
    if (!index.isValid())
        return false;

    // we check whether the data has really changed, otherwise we ignore it
    if (role == Qt::EditRole) {
        auto item = static_cast<PropertyItem*>(index.internalPointer());
        QVariant data = item->data(index.column(), role);
        if (data.userType() == QMetaType::Double && value.userType() == QMetaType::Double) {
            // since we store some properties as floats we get some round-off
            // errors here. Thus, we use an epsilon here.
            // NOTE: Since 0.14 PropertyFloat uses double precision, so this is maybe unnecessary now?
            double d = data.toDouble();
            double v = value.toDouble();
            if (fabs(d-v) > DBL_EPSILON)
                return item->setData(value);
        }
        // Special case handling for quantities
        else if (data.canConvert<Base::Quantity>() && value.canConvert<Base::Quantity>()) {
            const Base::Quantity& val1 = data.value<Base::Quantity>();
            const Base::Quantity& val2 = value.value<Base::Quantity>();
            if (!(val1 == val2))
                return item->setData(value);
        }
        else if (data != value)
            return item->setData(value);
    }

    return true;
}

Qt::ItemFlags PropertyModel::flags(const QModelIndex &index) const
{
    auto item = static_cast<PropertyItem*>(index.internalPointer());
    return item->flags(index.column());
}

QModelIndex PropertyModel::index ( int row, int column, const QModelIndex & parent ) const
{
    PropertyItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<PropertyItem*>(parent.internalPointer());

    PropertyItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex PropertyModel::parent ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return QModelIndex();

    auto childItem = static_cast<PropertyItem*>(index.internalPointer());
    PropertyItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int PropertyModel::rowCount ( const QModelIndex & parent ) const
{
    PropertyItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<PropertyItem*>(parent.internalPointer());

    return parentItem->childCount();
}

QVariant PropertyModel::headerData (int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal) {
        if (role != Qt::DisplayRole)
            return QVariant();
        if (section == 0)
            return tr("Property");
        if (section == 1)
            return tr("Value");
    }

    return QVariant();
}

bool PropertyModel::setHeaderData (int, Qt::Orientation, const QVariant &, int)
{
    return false;
}

QStringList PropertyModel::propertyPathFromIndex(const QModelIndex& index) const
{
    QStringList path;
    if (index.isValid()) {
        auto item = static_cast<PropertyItem*>(index.internalPointer());
        while (item && item != this->rootItem) {
            path.push_front(item->propertyName());
            item = item->parent();
        }
    }

    return path;
}

QModelIndex PropertyModel::propertyIndexFromPath(const QStringList& path) const
{
    if (path.size() < 2)
        return QModelIndex();

    auto findItem = [this](const QStringList &path, PropertyItem *item) -> QModelIndex {
        QModelIndex index = this->index(item->row(), 0, QModelIndex());
        for (int j=1; j<path.size(); ++j) {
            bool found = false;
            for (int i=0, c = item->childCount(); i<c; ++i) {
                auto child = item->child(i);
                if (child->propertyName() == path[j]) {
                    index = this->index(i, 1, index);
                    item = child;
                    found = true;
                    break;
                }
            }
            if (!found)
                return j==1 ? QModelIndex() : index;
        }
        return index;
    };

    auto it = groupItems.find(path.front());
    if (it != groupItems.end()) {
        QModelIndex index = findItem(path, it->second.groupItem);
        if (index.isValid())
            return index;
    }

    // No property with the same name found in the given group. So try to find
    // one in a different group.
    for (auto &v : groupItems) {
        QModelIndex index = findItem(path, v.second.groupItem);
        if (index.isValid())
            return index;
    }
    return QModelIndex();
}

static void setPropertyItemName(PropertyItem *item, const char *propName, QString groupName) {
    QString name = QString::fromUtf8(propName);
    QString realName = name;
    groupName.replace(QStringLiteral(" "), QString());
    QString prefix = groupName + QStringLiteral("_");;
    if(name.size() > prefix.size()) {
        if(name.startsWith(prefix)) {
            // For property name with format <group_name>_<name>, it will be displayed as <name>
            name = name.right(name.size()-prefix.size());
        } else if(name.startsWith(QStringLiteral("_"))
                    && name.midRef(1, prefix.size()) == prefix) {
            // For property name with format _<group_name>_<name>, it will be displayed as _<name>
            name = QStringLiteral("_") + name.right(name.size() - 1 - prefix.size());
        }
    }
    item->setPropertyName(name, realName);
}

static PropertyItem *createPropertyItem(App::Property *prop)
{
    const char *editor = prop->getEditorName();
    if (!editor || !editor[0]) {
        if (PropertyView::showAll())
            editor = "Gui::PropertyEditor::PropertyItem";
        else
            return nullptr;
    }
    auto item = static_cast<PropertyItem*>(
            PropertyItemFactory::instance().createPropertyItem(editor));
    if (!item) {
        qWarning("No property item for type %s found\n", editor);
    }
    return item;
}

PropertyModel::GroupInfo &PropertyModel::getGroupInfo(App::Property *prop)
{
    const char* group = prop->getGroup();
    bool isEmpty = (!group || group[0] == '\0');
    QString groupName = QString::fromUtf8(
            isEmpty ? QT_TRANSLATE_NOOP("App::Property", "Base") : group);

    auto res = groupItems.insert(std::make_pair(groupName, GroupInfo()));
    if (res.second) {
        auto &groupInfo = res.first->second;
        groupInfo.groupItem = static_cast<PropertySeparatorItem*>(PropertySeparatorItem::create());
        groupInfo.groupItem->setReadOnly(true);
        groupInfo.groupItem->setExpanded(true);
        groupInfo.groupItem->setParent(rootItem);
        groupInfo.groupItem->setPropertyName(groupName);

        auto it = res.first;
        int row = 0;
        if (it != groupItems.begin()) {
            --it;
            row = it->second.groupItem->_row + 1;
            ++it;
        }
        groupInfo.groupItem->_row = row;
        int revision = _revision;
        beginInsertRows(QModelIndex(), row, row);
        if (revision == _revision) {
            rootItem->insertChild(row, groupInfo.groupItem);
            // update row index for all group items behind
            for (++it; it!=groupItems.end(); ++it)
                ++it->second.groupItem->_row;
        }
        endInsertRows();
    }

    return res.first->second;
}

void PropertyModel::buildUp(const PropertyModel::PropertyList& props)
{
    ++_revision;

    // If props empty, then simply reset all property items, but keep the group
    // items.
    if (props.empty()) {
        resetGroups();
        return;
    }

    // First step, init group info
    initGroups();

    // Second step, either find existing items or create new items for the given
    // properties. There is no signaling of model change at this stage. The
    // change information is kept pending in GroupInfo::children
    findOrCreateChildren(props);

    // Third step, signal item insertion and movement.
    insertOrMoveChildren();


    // Final step, signal item removal. This is separated from the above because
    // of the possibility of moving items between groups.
    removeChildren();
}

void PropertyModel::resetGroups()
{
    beginResetModel();
    for(auto &v : groupItems) {
        auto &groupInfo = v.second;
        groupInfo.groupItem->reset();
        groupInfo.children.clear();
    }
    itemMap.clear();
    endResetModel();
}

void PropertyModel::initGroups()
{
    for (auto &v : groupItems) {
        auto &groupInfo = v.second;
        groupInfo.children.clear();
    }
}

void PropertyModel::findOrCreateChildren(const PropertyModel::PropertyList& props)
{
    for (const auto & jt : props) {
        App::Property* prop = jt.second.front();

        PropertyItem *item = nullptr;
        for (auto prop : jt.second) {
            auto it = itemMap.find(prop);
            if (it == itemMap.end() || !it->second) {
                continue;
            }
            item = it->second;
            break;
        }

        if (!item) {
            item = createPropertyItem(prop);
            if (!item) {
                continue;
            }
        }

        GroupInfo &groupInfo = getGroupInfo(prop);
        groupInfo.children.push_back(item);

        const char *propName = jt.first.c_str();
        std::string _name;
        if (boost::ends_with(propName,"*")) {
            item->setLinked(true);
            _name = propName;
            _name.resize(_name.size()-1);
            propName = _name.c_str();
        }
        setPropertyItemName(item, propName, groupInfo.groupItem->propertyName());

        if (jt.second != item->getPropertyData()) {
            for (auto prop : item->getPropertyData()) {
                itemMap.erase(prop);
            }
            for (auto prop : jt.second) {
                itemMap[prop] = item;
            }
            // TODO: is it necessary to make sure the item has no pending commit?
            item->setPropertyData(jt.second);
        }
        else {
            item->updateData();
        }
    }
}

void PropertyModel::insertOrMoveChildren()
{
    int revision = _revision;

    for (auto &v : groupItems) {
        auto &groupInfo = v.second;
        int beginChange = -1;
        int endChange = 0;
        int beginInsert = -1;
        int endInsert = 0;
        int row = -1;
        QModelIndex midx = this->index(groupInfo.groupItem->_row, 0, QModelIndex());

        auto flushInserts = [&]() {
            if (beginInsert < 0)
                return true;
            beginInsertRows(midx, beginInsert, endInsert);
            if (revision != _revision) {
                endInsertRows();
                return false;
            }
            for (int i=beginInsert; i<=endInsert; ++i)
                groupInfo.groupItem->insertChild(i, groupInfo.children[i]);
            endInsertRows();
            beginInsert = -1;
            return true;
        };

        auto flushChanges = [&]() {
            if (beginChange < 0)
                return;
            (void)endChange;
            // There is no need to signal dataChange(), because PropertyEditor
            // will call PropertyModel::updateProperty() on any property
            // changes.
            //
            // dataChanged(this->index(beginChange,0,midx), this->index(endChange,1,midx));
            beginChange = -1;
        };

        for (auto item : groupInfo.children) {
            ++row;
            if (!item->parent()) {
                flushChanges();
                item->setParent(groupInfo.groupItem);
                if (beginInsert < 0)
                    beginInsert = row;
                endInsert = row;
            } else {
                if (!flushInserts())
                    return;
                int oldRow = item->row();
                // Dynamic property can rename group, so must check
                auto groupItem = item->parent();
                assert(groupItem);
                if (oldRow == row && groupItem == groupInfo.groupItem) {
                    if (beginChange < 0)
                        beginChange = row;
                    endChange = row;
                } else {
                    flushChanges();
                    beginMoveRows(createIndex(groupItem->row(),0,groupItem),
                            oldRow, oldRow, midx, row);
                    if (revision != _revision) {
                        endMoveRows();
                        return;
                    }
                    if (groupItem == groupInfo.groupItem)
                        groupInfo.groupItem->moveChild(oldRow, row);
                    else {
                        groupItem->takeChild(oldRow);
                        item->setParent(groupInfo.groupItem);
                        groupInfo.groupItem->insertChild(row, item);
                    }
                    endMoveRows();
                }
            }
        }
        flushChanges();
        if (!flushInserts())
            return;
    }
}

void PropertyModel::removeChildren()
{
    int revision = _revision;

    for (auto &v : groupItems) {
        auto &groupInfo = v.second;
        int first, last;
        getRange(groupInfo, first, last);
        if (last > first) {
            QModelIndex midx = this->index(groupInfo.groupItem->_row, 0, QModelIndex());
            // This can trigger a recursive call of PropertyView::onTimer()
            beginRemoveRows(midx, first, last - 1);
            if (revision == _revision) {
                groupInfo.groupItem->removeChildren(first, last - 1);
            }
            endRemoveRows();
        }
        else {
            assert(last == first);
        }
    }
}

void PropertyModel::getRange(const PropertyModel::GroupInfo& groupInfo, int& first, int& last) const
{
    first = static_cast<int>(groupInfo.children.size());
    last = groupInfo.groupItem->childCount();
}

void PropertyModel::updateProperty(const App::Property& prop, bool committing)
{
    auto it = itemMap.find(const_cast<App::Property*>(&prop));
    if (it == itemMap.end() || !it->second || !it->second->parent())
        return;

    int column = 1;
    PropertyItem *item = it->second;
    item->updateData();
    if (committing)
        return;
    QModelIndex parent = this->index(item->parent()->row(), 0, QModelIndex());
    item->assignProperty(&prop);
    QModelIndex data = this->index(item->row(), column, parent);
    Q_EMIT dataChanged(data, data);
    updateChildren(item, column, data);
}

void PropertyModel::appendProperty(const App::Property& _prop)
{
    auto prop = const_cast<App::Property*>(&_prop);
    if (!prop->getName())
        return;
    auto it = itemMap.find(prop);
    if (it == itemMap.end() || !it->second)
        return;

    PropertyItem *item = createPropertyItem(prop);
    GroupInfo &groupInfo = getGroupInfo(prop);

    int row = 0;
    for (int c=groupInfo.groupItem->childCount(); row<c; ++row) {
        PropertyItem *child = groupInfo.groupItem->child(row);
        App::Property *firstProp = item->getFirstProperty();
        if (firstProp && firstProp->testStatus(App::Property::PropDynamic)
                      && item->propertyName() >= child->propertyName())
            break;
    }

    QModelIndex midx = this->index(groupInfo.groupItem->_row,0,QModelIndex());
    int revision = ++_revision;
    beginInsertRows(midx, row, row);
    if (revision == _revision) {
        groupInfo.groupItem->insertChild(row, item);
        setPropertyItemName(item, prop->getName(), groupInfo.groupItem->propertyName());
        item->setPropertyData({prop});
    }
    endInsertRows();
}

void PropertyModel::removeProperty(const App::Property& _prop)
{
    auto prop = const_cast<App::Property*>(&_prop);
    auto it = itemMap.find(prop);
    if (it == itemMap.end() || !it->second)
        return;

    PropertyItem *item = it->second;
    if (item->removeProperty(prop)) {
        PropertyItem *parent = item->parent();
        int row = item->row();
        int revision = ++_revision;
        beginRemoveRows(this->index(parent->row(), 0, QModelIndex()), row, row);
        if (revision == _revision)
            parent->removeChildren(row,row);
        endRemoveRows();
    }
}

void PropertyModel::updateChildren(PropertyItem* item, int column, const QModelIndex& parent)
{
    int numChild = item->childCount();
    if (numChild > 0) {
        QModelIndex topLeft = this->index(0, column, parent);
        QModelIndex bottomRight = this->index(numChild, column, parent);
        Q_EMIT dataChanged(topLeft, bottomRight);
    }
}

bool PropertyModel::removeRows(int row, int count, const QModelIndex& parent)
{
    PropertyItem* item;
    if (!parent.isValid())
        item = rootItem;
    else
        item = static_cast<PropertyItem*>(parent.internalPointer());

    int start = row;
    int end = row+count-1;
    int revision = ++_revision;
    beginRemoveRows(parent, start, end);
    if (revision == _revision)
        item->removeChildren(start, end);
    endRemoveRows();
    return true;
}

void PropertyModel::updateDecimals(const QModelIndex &idx)
{
    PropertyItem *item;
    if (!idx.isValid())
        item = rootItem;
    else
        item = static_cast<PropertyItem*>(idx.internalPointer());
    if (item->updateDecimals())
        dataChanged(idx, idx);
    for (int i=0, c=item->childCount(); i<c; ++i)
        updateDecimals(this->index(i, 0, idx));
}

#include "moc_PropertyModel.cpp"
