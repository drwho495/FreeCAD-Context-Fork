/***************************************************************************
 *   Copyright (c) 2015 Stefan Tröger <stefantroeger@gmx.net>              *
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
# include <sstream>
# include <QAction>
# include <QRegExp>
# include <QTextStream>
# include <QMessageBox>
# include <Precision.hxx>
# include <QKeyEvent>
#endif

#include "ui_TaskLoftParameters.h"
#include "TaskLoftParameters.h"
#include <App/Application.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Base/Console.h>
#include <Base/Tools.h>
#include <Gui/Selection.h>
#include <Gui/CommandT.h>
#include <Gui/MetaTypes.h>
#include <Mod/PartDesign/App/FeatureLoft.h>
#include <Mod/Sketcher/App/SketchObject.h>
#include <Mod/PartDesign/App/Body.h>
#include "TaskSketchBasedParameters.h"
#include "ReferenceSelection.h"
#include "Utils.h"

namespace {
struct SubInfo
{
    App::SubObjectT objT;
    std::vector<std::string> subs;
    SubInfo(){}
    SubInfo(const App::SubObjectT &objT,
            const std::vector<std::string> &subs)
        :objT(objT)
        ,subs(subs)
    {}
};
} //anonymous namespace
Q_DECLARE_METATYPE(SubInfo)

using namespace PartDesignGui;
using namespace Gui;

static bool _Busy;

class LoftSectionSelectionGate : public Gui::SelectionGate
{
public:
    LoftSectionSelectionGate(TaskLoftParameters *master,
                         const Gui::ViewProviderDocumentObject *vp)
        :master(master)
    {
        if (vp) {
            objT = App::DocumentObjectT(vp->getObject());
            inList = vp->getObject()->getInListEx(true);
            inList.insert(vp->getObject());
        }
    }

    ~LoftSectionSelectionGate()
    {
        master->exitSelectionMode();
    }

    bool allow(App::Document*, App::DocumentObject* pObj, const char*)
    {
        auto loft = static_cast<PartDesign::Loft*>(objT.getObject());
        if (!loft) {
            this->notAllowedReason = QT_TR_NOOP("Loft feature not found.");
            return false;
        }
        if (inList.count(pObj)) {
            this->notAllowedReason = QT_TR_NOOP("Selecting this will cause circular dependency.");
            return false;
        }
        if (loft->Profile.getValue() == pObj) {
            this->notAllowedReason = QT_TR_NOOP("Profile object cannot be used as section.");
            return false;
        }
        return true;
    }

private:
    App::DocumentObjectT objT;
    std::set<App::DocumentObject*> inList;
    TaskLoftParameters *master;
};


/* TRANSLATOR PartDesignGui::TaskLoftParameters */

TaskLoftParameters::TaskLoftParameters(ViewProviderLoft *LoftView,bool /*newObj*/, QWidget *parent)
    : TaskSketchBasedParameters(LoftView, parent, "PartDesign_AdditiveLoft", tr("Loft parameters"))
    , ui(new Ui_TaskLoftParameters)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    if (LoftView) {
        PartDesign::Loft* pcLoft = static_cast<PartDesign::Loft*>(LoftView->getObject());
        ui->checkBoxRuled->setToolTip(QApplication::translate(
                    "Property", pcLoft->Ruled.getDocumentation()));
        ui->checkBoxClosed->setToolTip(QApplication::translate(
                    "Property", pcLoft->Closed.getDocumentation()));
        ui->checkBoxSplitProfile->setToolTip(QApplication::translate(
                    "Property", pcLoft->SplitProfile.getDocumentation()));
    }

    ui->listWidgetReferences->installEventFilter(this);
    ui->listWidgetReferences->setMouseTracking(true);

    connect(ui->buttonRefAdd, SIGNAL(clicked(bool)),
            this, SLOT(onRefButtonAdd(bool)));
    connect(ui->checkBoxRuled, SIGNAL(toggled(bool)),
            this, SLOT(onRuled(bool)));
    connect(ui->checkBoxClosed, SIGNAL(toggled(bool)),
            this, SLOT(onClosed(bool)));
    connect(ui->checkBoxSplitProfile, SIGNAL(toggled(bool)),
            this, SLOT(onSplitProfile(bool)));
    connect(ui->listWidgetReferences, SIGNAL(itemEntered(QListWidgetItem*)),
            this, SLOT(onItemEntered(QListWidgetItem*)));
    connect(ui->listWidgetReferences, SIGNAL(itemSelectionChanged()),
            this, SLOT(onItemSelectionChanged()));

    // Create context menu
    QAction* remove = new QAction(tr("Remove"), this);
    remove->setShortcut(QKeySequence::Delete);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    // display shortcut behind the context menu entry
    remove->setShortcutVisibleInContextMenu(true);
#endif
    ui->listWidgetReferences->addAction(remove);
    ui->listWidgetReferences->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(remove, SIGNAL(triggered()), this, SLOT(onDeleteSection()));

    connect(ui->listWidgetReferences->model(),
        SIGNAL(rowsMoved(QModelIndex, int, int, QModelIndex, int)), this, SLOT(indexesMoved()));

    this->initUI(proxy);
    this->groupLayout()->addWidget(proxy);
    refresh();

    PartDesign::Loft* loft = static_cast<PartDesign::Loft*>(vp->getObject());
    connSections = loft->Sections.signalChanged.connect(
        [this](const App::Property &) {toggleShowOnTop(vp, lastSections, "Sections", true);});

    // call updateUI until Gui::Document::setEdit() finishes;
    QMetaObject::invokeMethod(this, "updateUI", Qt::QueuedConnection);
}

TaskLoftParameters::~TaskLoftParameters()
{
    exitSelectionMode();
}

void TaskLoftParameters::updateUI()
{
    toggleShowOnTop(vp, lastSections, "Sections", true);
}

void TaskLoftParameters::addItem(App::DocumentObject *obj, const std::vector<std::string> &subs, bool select)
{
    App::SubObjectT objT(obj);
    QString label = QString::fromUtf8(objT.getObjectFullName(objT.getDocumentName().c_str()).c_str());
    if (subs.size() && !subs[0].empty()) {
        bool first = true;
        for (const auto &sub : subs) {
            if (first) {
                first = false;
                label += QStringLiteral("(");
            }
            else
                label += QStringLiteral(", ");
            label += QString::fromUtf8(sub.c_str());
        }
        label += QStringLiteral(")");
    }

    QListWidgetItem *item = nullptr;
    for (int i=0; i<ui->listWidgetReferences->count(); ++i) {
        auto listItem = ui->listWidgetReferences->item(i);
        if (qvariant_cast<SubInfo>(listItem->data(Qt::UserRole)).objT == objT) {
            item = listItem;
            break;
        }
    }
    if (!item) {
        item = new QListWidgetItem(ui->listWidgetReferences);
        auto vp = Gui::Application::Instance->getViewProvider(obj);
        if (vp)
            item->setIcon(vp->getIcon());
    }
    item->setData(Qt::UserRole, QVariant::fromValue(SubInfo(objT, subs)));
    item->setText(label);
    if (select) {
        QSignalBlocker blocker(ui->listWidgetReferences);
        item->setSelected(true);
        ui->listWidgetReferences->scrollToItem(item);
    }
}

void TaskLoftParameters::refresh()
{
    if (!vp || !vp->getObject())
        return;

    // Temporarily prevent unnecessary feature recomputes
    for (QWidget* child : proxy->findChildren<QWidget*>())
        child->blockSignals(true);

    //add the profiles
    PartDesign::Loft* loft = static_cast<PartDesign::Loft*>(vp->getObject());

    for (const auto &v : loft->Sections.getSubListValues())
        addItem(v.first, v.second);

    // get options
    ui->checkBoxRuled->setChecked(loft->Ruled.getValue());
    ui->checkBoxClosed->setChecked(loft->Closed.getValue());
    ui->checkBoxSplitProfile->setChecked(loft->SplitProfile.getValue());

    for (QWidget* child : proxy->findChildren<QWidget*>())
        child->blockSignals(false);
}

bool TaskLoftParameters::addSections(const std::vector<App::SubObjectT> &objs)
{
    if (!vp)
        return false;
    PartDesign::Loft* loft = static_cast<PartDesign::Loft*>(vp->getObject());
    try {
        auto sections = loft->Sections.getSubListValues();
        bool touched = false;
        std::set<App::DocumentObjectT> newSections;
        for (auto ref : objs) {
            ref = PartDesignGui::importExternalObject(ref);
            auto refObj = ref.getObject();
            if (!refObj)
                continue;
            bool found = false;
            for (auto &v : sections) {
                if (v.first != refObj)
                    continue;
                if (ref.getSubName().empty()) {
                    if (v.second.size() != 1 || v.second.front().size()) {
                        v.second.clear();
                        v.second.emplace_back();
                        touched = true;
                    }
                    found = true;
                    break;
                }
                for (auto it=v.second.begin(); it!=v.second.end();) {
                    if (it->empty()) {
                        it = v.second.erase(it);
                        touched = true;
                    }
                    else if (*it == ref.getSubName()) {
                        found = true;
                        break;
                    } else
                        ++it;
                }
                if (!found) {
                    v.second.push_back(ref.getSubName());
                    touched = found = true;
                }
                break;
            }
            if (!found) {
                newSections.emplace(refObj);
                std::vector<std::string> subs;
                if (!ref.getSubName().empty())
                    subs.push_back(ref.getSubName());
                sections.emplace_back(refObj, std::move(subs));
                touched = true;
            }
        }
        if (!touched)
            return false;
        setupTransaction();
        loft->Sections.setSubListValues(sections);
        recomputeFeature();
        for (const auto &o : newSections) {
            if (auto obj = o.getObject())
                obj->Visibility.setValue(false);
        }
        refresh();
        return true;
    } catch (Base::Exception &e) {
        e.ReportException();
    }
    return false;
}

void TaskLoftParameters::_onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (!vp || _Busy)
        return;

    if (msg.Type == Gui::SelectionChanges::ClrSelection) {
        QSignalBlocker blocker(ui->listWidgetReferences);
        ui->listWidgetReferences->selectionModel()->clearSelection();
        return;
    }

    if (msg.Type != Gui::SelectionChanges::AddSelection)
        return;

    auto obj = msg.Object.getObject();
    if (!obj)
        return;

    switch(getSelectionMode()) {
    case SelectionMode::refAdd: {
        App::SubObjectT ref(msg.pOriginalMsg ? msg.pOriginalMsg->Object : msg.Object);
        addSections({ref});
        break;
    }
    default:
        break;
    }
}

void TaskLoftParameters::onDeleteSection()
{
    if (!vp)
        return;

    PartDesign::Loft* loft = static_cast<PartDesign::Loft*>(vp->getObject());

    // Delete the selected profile
    for(auto item : ui->listWidgetReferences->selectedItems())
        delete item;

    std::vector<App::PropertyLinkSubList::SubSet> subset;
    for (int i=0; i<ui->listWidgetReferences->count(); ++i) {
        auto data = qvariant_cast<SubInfo>(ui->listWidgetReferences->item(i)->data(Qt::UserRole));
        if (auto obj = data.objT.getObject())
            subset.emplace_back(obj, data.subs);
    }
    try {
        setupTransaction();
        loft->Sections.setSubListValues(subset);
        recomputeFeature();
    } catch (Base::Exception &e) {
        e.ReportException();
    }
}

void TaskLoftParameters::indexesMoved()
{
    if (!vp)
        return;

    QAbstractItemModel* model = qobject_cast<QAbstractItemModel*>(sender());
    if (!model)
        return;

    PartDesign::Loft* loft = static_cast<PartDesign::Loft*>(vp->getObject());
    std::vector<App::PropertyLinkSubList::SubSet> subset;
    int rows = model->rowCount();
    for (int i = 0; i < rows; i++) {
        QModelIndex index = model->index(i, 0);
        auto data = qvariant_cast<SubInfo>(index.data(Qt::UserRole));
        if (auto obj = data.objT.getObject())
            subset.emplace_back(obj, data.subs);
    }
    try {
        setupTransaction();
        loft->Sections.setSubListValues(subset);
        recomputeFeature();
    } catch (Base::Exception &e) {
        e.ReportException();
    }
}

void TaskLoftParameters::exitSelectionMode() {

    if (getSelectionMode() == SelectionMode::none)
        return;
    setSelectionMode(SelectionMode::none);
    ui->buttonRefAdd->setChecked(false);
    Gui::Selection().clearSelection();
    Gui::Selection().rmvSelectionGate();
}

void TaskLoftParameters::changeEvent(QEvent * /*e*/)
{
}

void TaskLoftParameters::onClosed(bool val) {
    if (!vp)
        return;
    try {
        setupTransaction();
        static_cast<PartDesign::Loft*>(vp->getObject())->Closed.setValue(val);
        recomputeFeature();
    } catch (Base::Exception &e) {
        e.ReportException();
    }
}

void TaskLoftParameters::onRuled(bool val) {
    if (!vp)
        return;
    try {
        setupTransaction();
        static_cast<PartDesign::Loft*>(vp->getObject())->Ruled.setValue(val);
        recomputeFeature();
    } catch (Base::Exception &e) {
        e.ReportException();
    }
}

void TaskLoftParameters::onSplitProfile(bool val) {
    if (!vp)
        return;
    try {
        setupTransaction();
        static_cast<PartDesign::Loft*>(vp->getObject())->SplitProfile.setValue(val);
        recomputeFeature();
    } catch (Base::Exception &e) {
        e.ReportException();
    }
}

void TaskLoftParameters::onRefButtonAdd(bool checked) {
    if (!vp)
        return;
    if (checked) {
        auto sels = Gui::Selection().getSelectionT("*", 0);
        if (sels.size()) {
            if (addSections(sels)) {
                ui->buttonRefAdd->setChecked(false);
                return;
            }
        }
        Gui::Selection().clearSelection();
        Gui::Selection().addSelectionGate(new LoftSectionSelectionGate(this, vp));
        setSelectionMode(SelectionMode::refAdd);
    } else {
        exitSelectionMode();
    }
}

void TaskLoftParameters::onItemEntered(QListWidgetItem *item)
{
    if (!vp)
        return;
    auto objT = qvariant_cast<App::SubObjectT>(item->data(Qt::UserRole));
    PartDesignGui::highlightObjectOnTop(objT);
}

bool TaskLoftParameters::_eventFilter(QObject *o, QEvent *ev)
{
    switch(ev->type()) {
    case QEvent::Leave:
        Gui::Selection().rmvPreselect();
        break;
    case QEvent::ShortcutOverride:
    case QEvent::KeyPress: {
        QKeyEvent * kevent = static_cast<QKeyEvent*>(ev);
        if (o == ui->listWidgetReferences && kevent->modifiers() == Qt::NoModifier) {
            if (kevent->key() == Qt::Key_Delete) {
                kevent->accept();
                if (ev->type() == QEvent::KeyPress)
                    onDeleteSection();
            }
        }
        break;
    }
    default:
        break;
    }
    return false;
}

void TaskLoftParameters::onItemSelectionChanged()
{
    if (!vp)
        return;
    auto items = ui->listWidgetReferences->selectedItems();
    Base::StateLocker lock(_Busy);
    if (items.isEmpty() || items.size() == 1) {
        Gui::Selection().selStackPush();
        Gui::Selection().clearSelection();
    }
    for (auto item : items) {
        PartDesignGui::selectObjectOnTop(
                qvariant_cast<App::SubObjectT>(item->data(Qt::UserRole)), true);
    }
}


//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgLoftParameters::TaskDlgLoftParameters(ViewProviderLoft *LoftView,bool newObj)
   : TaskDlgSketchBasedParameters(LoftView)
{
    assert(LoftView);
    parameter  = new TaskLoftParameters(LoftView,newObj);

    Content.push_back(parameter);
}

TaskDlgLoftParameters::~TaskDlgLoftParameters()
{
}


bool TaskDlgLoftParameters::accept()
{
    PartDesign::Loft* pcLoft = static_cast<PartDesign::Loft*>(getLoftView()->getObject());
    try {
        parameter->setupTransaction();
        Gui::cmdGuiDocument(pcLoft, "resetEdit()");
        Gui::cmdAppDocument(pcLoft, "recompute()");
        if (!vp->getObject()->isValid())
            throw Base::RuntimeError(vp->getObject()->getStatusString());
        Gui::Command::commitCommand();
    }
    catch (const Base::Exception& e) {
        QMessageBox::warning(parameter, tr("Input error"), QString::fromUtf8(e.what()));
        return false;
    }

    return true;
}

//==== calls from the TaskView ===============================================================


#include "moc_TaskLoftParameters.cpp"
