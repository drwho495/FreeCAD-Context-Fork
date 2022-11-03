/***************************************************************************
 *   Copyright (c) 2013 Jan Rheinländer                                    *
 *                                   <jrheinlaender@users.sourceforge.net> *
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


#ifndef GUI_TASKVIEW_TaskSketchBasedParameters_H
#define GUI_TASKVIEW_TaskSketchBasedParameters_H

#include <QStandardItemModel>
#include <QItemDelegate>

#include <Gui/Selection.h>
#include "ViewProvider.h"

#include "ReferenceSelection.h"
#include "TaskFeatureParameters.h"

class QComboBox;
class QCheckBox;
class QListWidget;

namespace App {
class Property;
}

namespace Gui {
class PrefQuantitySpinBox;
}

namespace PartDesignGui {

class ProfileWidgetDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    ProfileWidgetDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;
};

/// Convenience class to collect common methods for all SketchBased features
class TaskSketchBasedParameters : public PartDesignGui::TaskFeatureParameters,
                                  public Gui::SelectionObserver
{
    Q_OBJECT

public:
    TaskSketchBasedParameters(PartDesignGui::ViewProvider* vp, QWidget *parent,
                              const std::string& pixmapname, const QString& parname);
    ~TaskSketchBasedParameters();

protected:
    virtual void _onSelectionChanged(const Gui::SelectionChanges&) {}
    virtual bool _eventFilter(QObject *, QEvent *) {return false;}

    const QString onAddSelection(const Gui::SelectionChanges& msg);

    enum class SelectionMode { none, refAdd, refObjAdd, refProfile, refAxis };

    void onSelectReference(QWidget *blinkWidget,
                           const ReferenceSelection::Config &conf = ReferenceSelection::Config())
    {
        onSelectReference(blinkWidget, SelectionMode::refAdd, conf);
    }
    void onSelectReference(QWidget *blinkWidget,
                           SelectionMode mode,
                           const ReferenceSelection::Config &conf = ReferenceSelection::Config());
    virtual void exitSelectionMode();
    void setSelectionMode(SelectionMode mode);
    SelectionMode getSelectionMode() const {return selectionMode;}

    QVariant setUpToFace(const QString& text);
    /// Try to find the name of a feature with the given label.
    /// For faster access a suggested name can be tested, first.
    QVariant objectNameByLabel(const QString& label, const QVariant& suggest) const;

    QString getFaceReference(const QString& obj, const QString& sub) const;

    void saveHistory();

    void initUI(QWidget *);
    void addProfileEdit(QBoxLayout *boxLayout);
    void addFittingWidgets(QBoxLayout *parentLayout);
    void _refresh();

    virtual void onProfileButton(bool);
    virtual void onClearProfile();
    virtual void onDeleteProfile();

    bool setProfile(const std::vector<App::SubObjectT> &objs);
    bool addProfile(const App::SubObjectT &obj);

protected Q_SLOTS:
    void onFitChanged(double);
    void onFitJoinChanged(int);
    void onInnerFitChanged(double);
    void onInnerFitJoinChanged(int);

private:
    virtual void onSelectionChanged(const Gui::SelectionChanges& msg) final;
    virtual bool eventFilter(QObject *o, QEvent *ev) final;

protected:
    friend class ProfileWidgetDelegate;

    Gui::PrefQuantitySpinBox * fitEdit = nullptr;
    QComboBox *fitJoinType = nullptr;
    Gui::PrefQuantitySpinBox * innerFitEdit = nullptr;
    QComboBox *innerFitJoinType = nullptr;
    bool selectingReference = false;
    QListWidget *profileWidget = nullptr;
    QPushButton *profileButton = nullptr;
    QPushButton *clearProfileButton = nullptr;
    QWidget *blinkWidget = nullptr;

private:
    SelectionMode selectionMode = SelectionMode::none;
    boost::signals2::scoped_connection connProfile;
    App::SubObjectT lastProfile;
};

class TaskDlgSketchBasedParameters : public PartDesignGui::TaskDlgFeatureParameters
{
    Q_OBJECT

public:
    TaskDlgSketchBasedParameters(PartDesignGui::ViewProvider *vp);
    ~TaskDlgSketchBasedParameters();

public:
    /// is called by the framework if the dialog is accepted (Ok)
    virtual bool accept();
    /// is called by the framework if the dialog is rejected (Cancel)
    virtual bool reject();
};

} //namespace PartDesignGui

#endif // GUI_TASKVIEW_TaskSketchBasedParameters_H
