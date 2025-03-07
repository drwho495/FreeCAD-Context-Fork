/***************************************************************************
 *   Copyright (c) 2014 Abdullah Tahiri <abdullah.tahiri.yo@gmail.com>     *
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
# include <boost/core/ignore_unused.hpp>
# include <QContextMenuEvent>
# include <QMenu>
# include <QShortcut>
# include <QString>
# include <QImage>
# include <QPixmap>
#endif

#include <Base/Tools.h>
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <App/MappedElement.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/MenuManager.h>
#include <Gui/Notifications.h>
#include <Gui/Selection.h>
#include <Gui/ViewProvider.h>

#include <Mod/Sketcher/App/ExternalGeometryFacade.h>
#include <Mod/Sketcher/App/GeometryFacade.h>
#include <Mod/Sketcher/App/SketchObject.h>


#include "TaskSketcherElements.h"
#include "ui_TaskSketcherElements.h"
#include "ViewProviderSketch.h"
#include "Utils.h"

using namespace Sketcher;
using namespace SketcherGui;
using namespace Gui::TaskView;

enum ColumnIndex {
    ColType,
    ColName,
    ColReference,
    ColFlags,
    ColMapped,
};

class MultIcon {
    
public:
    MultIcon &operator=(const char *);
    QIcon Normal;
    QIcon Construction;
    QIcon External;
    QIcon Internal;
};

// helper class to store additional information about the treeWidget entry.
class ElementItem : public QTreeWidgetItem
{
public:
    ElementItem(QTreeWidget *parent, Sketcher::SketchObject *sketch,
            int elementnr, Part::Geometry *geo)
        : QTreeWidgetItem(parent)
        , ElementNbr(elementnr)
        , isLineSelected(false)
        , isStartingPointSelected(false)
        , isEndPointSelected(false)
        , isMidPointSelected(false)
    {
        StartingVertex = sketch->getVertexIndexGeoPos(elementnr,Sketcher::PointPos::start),
        MidVertex = sketch->getVertexIndexGeoPos(elementnr,Sketcher::PointPos::mid),
        EndVertex = sketch->getVertexIndexGeoPos(elementnr,Sketcher::PointPos::end),
        GeometryType = geo->getTypeId();
        isConstruction = GeometryFacade::getConstruction(geo);
        isInternalAligned = GeometryFacade::isInternalAligned(geo);

        static std::map<Base::Type,QString> typeMap;
        if(typeMap.empty()) {
            typeMap[Part::GeomPoint::getClassTypeId()] = QObject::tr("Point");
            typeMap[Part::GeomLineSegment::getClassTypeId()] = QObject::tr("Line");
            typeMap[Part::GeomArcOfCircle::getClassTypeId()] = QObject::tr("Arc");
            typeMap[Part::GeomCircle::getClassTypeId()] = QObject::tr("Circle");
            typeMap[Part::GeomEllipse::getClassTypeId()] = QObject::tr("Ellipse");
            typeMap[Part::GeomArcOfEllipse::getClassTypeId()] = QObject::tr("Elliptical Arc");
            typeMap[Part::GeomArcOfHyperbola::getClassTypeId()] = QObject::tr("Hyperbolic Arc");
            typeMap[Part::GeomArcOfParabola::getClassTypeId()] = QObject::tr("Parabolic Arc");
            typeMap[Part::GeomBSplineCurve::getClassTypeId()] = QObject::tr("BSpline");
        }
        auto it = typeMap.find(GeometryType);
        if(it == typeMap.end())
            setText(ColumnIndex::ColType, QObject::tr("Other") +
                    QLatin1Char('-') + QString::fromUtf8(GeometryType.getName()));
        else
            setText(ColumnIndex::ColType, it->second);
        if(ElementNbr>=0) {
            if(GeometryFacade::getConstruction(geo))
                setText(ColumnIndex::ColFlags,QObject::tr("Construction"));
            isMissing = false;
            isExternal = false;
        }else{
            auto egf = ExternalGeometryFacade::getFacade(geo);
            isMissing = egf->testFlag(ExternalGeometryExtension::Missing);
            isExternal = !egf->getRef().empty();

            QString text;
            if(egf->testFlag(ExternalGeometryExtension::Defining))
                text = QObject::tr("Defining");
            if(egf->testFlag(ExternalGeometryExtension::Frozen)) {
                if(text.size())
                    text += QLatin1Char('-');
                text += QObject::tr("Frozen");
            }
            setText(ColumnIndex::ColFlags,text);

            setText(ColumnIndex::ColReference, QString::fromUtf8(sketch->getGeometryReference(ElementNbr).c_str()));
        }
    }

    ~ElementItem()
    {
    }

    void setVisibility(int filterindex)
    {
        if (filterindex == 0)
            this->setHidden(false);
        else {
            if( (!this->isConstruction && !this->isExternal && filterindex == 1)  ||
                (this->isConstruction  && filterindex == 2)  ||
                (this->isExternal && filterindex == 3) ) {
                this->setHidden(false);
            }
            else {
                this->setHidden(true);
            }
        }
    }

    void setElement(Sketcher::SketchObject *sketch, int element, int filterIndex) {
        static std::map<std::pair<Base::Type,int>, MultIcon> iconMap;
        static QIcon none;
        if(iconMap.empty()) {
            none = QIcon(Gui::BitmapFactory().pixmap("Sketcher_Element_SelectionTypeInvalid"));

            iconMap[std::make_pair(Part::GeomPoint::getClassTypeId(),1)] =
                "Sketcher_Element_Point_StartingPoint";

            iconMap[std::make_pair(Part::GeomLineSegment::getClassTypeId(),0)] =
                "Sketcher_Element_Line_Edge";

            iconMap[std::make_pair(Part::GeomLineSegment::getClassTypeId(),1)] =
                "Sketcher_Element_Line_StartingPoint";

            iconMap[std::make_pair(Part::GeomLineSegment::getClassTypeId(),2)] =
                "Sketcher_Element_Line_EndPoint";

            iconMap[std::make_pair(Part::GeomArcOfCircle::getClassTypeId(),0)] =
                "Sketcher_Element_Arc_Edge";

            iconMap[std::make_pair(Part::GeomArcOfCircle::getClassTypeId(),1)] =
                "Sketcher_Element_Arc_StartingPoint";

            iconMap[std::make_pair(Part::GeomArcOfCircle::getClassTypeId(),2)] =
                "Sketcher_Element_Arc_EndPoint";

            iconMap[std::make_pair(Part::GeomArcOfCircle::getClassTypeId(),3)] =
                "Sketcher_Element_Arc_MidPoint";

            iconMap[std::make_pair(Part::GeomCircle::getClassTypeId(),0)] =
                "Sketcher_Element_Circle_Edge";

            iconMap[std::make_pair(Part::GeomCircle::getClassTypeId(),3)] =
                "Sketcher_Element_Circle_MidPoint";

            iconMap[std::make_pair(Part::GeomEllipse::getClassTypeId(),0)] =
                "Sketcher_Element_Ellipse_Edge_2";

            iconMap[std::make_pair(Part::GeomEllipse::getClassTypeId(),3)] =
                "Sketcher_Element_Ellipse_CentrePoint";

            iconMap[std::make_pair(Part::GeomArcOfEllipse::getClassTypeId(),0)] =
                "Sketcher_Element_Elliptical_Arc_Edge";

            iconMap[std::make_pair(Part::GeomArcOfEllipse::getClassTypeId(),1)] =
                "Sketcher_Element_Elliptical_Arc_Start_Point";

            iconMap[std::make_pair(Part::GeomArcOfEllipse::getClassTypeId(),2)] =
                "Sketcher_Element_Elliptical_Arc_End_Point";

            iconMap[std::make_pair(Part::GeomArcOfEllipse::getClassTypeId(),3)] =
                "Sketcher_Element_Elliptical_Arc_Centre_Point";

            iconMap[std::make_pair(Part::GeomArcOfHyperbola::getClassTypeId(),0)] =
                "Sketcher_Element_Hyperbolic_Arc_Edge";

            iconMap[std::make_pair(Part::GeomArcOfHyperbola::getClassTypeId(),1)] =
                "Sketcher_Element_Hyperbolic_Arc_Start_Point";

            iconMap[std::make_pair(Part::GeomArcOfHyperbola::getClassTypeId(),2)] =
                "Sketcher_Element_Hyperbolic_Arc_End_Point";

            iconMap[std::make_pair(Part::GeomArcOfHyperbola::getClassTypeId(),3)] =
                "Sketcher_Element_Hyperbolic_Arc_Centre_Point";

            iconMap[std::make_pair(Part::GeomArcOfParabola::getClassTypeId(),0)] =
                "Sketcher_Element_Parabolic_Arc_Edge";

            iconMap[std::make_pair(Part::GeomArcOfParabola::getClassTypeId(),1)] =
                "Sketcher_Element_Parabolic_Arc_Start_Point";

            iconMap[std::make_pair(Part::GeomArcOfParabola::getClassTypeId(),2)] =
                "Sketcher_Element_Parabolic_Arc_End_Point";

            iconMap[std::make_pair(Part::GeomArcOfParabola::getClassTypeId(),3)] =
                "Sketcher_Element_Parabolic_Arc_Centre_Point";

            iconMap[std::make_pair(Part::GeomBSplineCurve::getClassTypeId(),0)] =
                "Sketcher_Element_BSpline_Edge";

            iconMap[std::make_pair(Part::GeomBSplineCurve::getClassTypeId(),1)] =
                "Sketcher_Element_BSpline_StartPoint";

            iconMap[std::make_pair(Part::GeomBSplineCurve::getClassTypeId(),2)] =
                "Sketcher_Element_BSpline_EndPoint";
        }
        QIcon icon;
        if(isMissing)
            icon = none;
        else {
            auto it = iconMap.find(std::make_pair(GeometryType,element));
            if(it == iconMap.end()) {
                if(!element && GeometryType != Part::GeomPoint::getClassTypeId())
                    icon = none;
            }
            else if (isConstruction)
                icon = it->second.Construction;
            else if (isExternal)
                icon = it->second.External;
            else if (isInternalAligned)
                icon = it->second.Internal;
            else
                icon = it->second.Normal;
        }
        setIcon(0,icon);
        setVisibility(filterIndex);

        Data::IndexedName name = sketch->shapeTypeFromGeoId(ElementNbr, (Sketcher::PointPos)(element));
        std::string tmp;
        setText(ColumnIndex::ColName, QString::fromUtf8(name.toString(tmp)));
        std::string mapped = sketch->convertSubName(name,false);
        setText(ColumnIndex::ColMapped, QString::fromUtf8(mapped.c_str()));
    }

    int ElementNbr;
    int StartingVertex;
    int MidVertex;
    int EndVertex;
    bool isLineSelected;
    bool isStartingPointSelected;
    bool isEndPointSelected;
    bool isMidPointSelected;
    bool isMissing;
    Base::Type GeometryType;
    bool isConstruction;
    bool isExternal;
    bool isInternalAligned;
};

ElementView::ElementView(QWidget *parent)
    : QTreeWidget(parent)
{
}

ElementView::~ElementView()
{
}

void ElementView::contextMenuEvent (QContextMenuEvent* event)
{
    QMenu menu;
    auto items = selectedItems();
    Gui::MenuItem mitems;
    mitems << "Sketcher_ConstrainCoincident"
           << "Sketcher_ConstrainPointOnObject"
           << "Sketcher_ConstrainVertical"
           << "Sketcher_ConstrainHorizontal"
           << "Sketcher_ConstrainParallel"
           << "Sketcher_ConstrainPerpendicular"
           << "Sketcher_ConstrainTangent"
           << "Sketcher_ConstrainEqual"
           << "Sketcher_ConstrainSymmetric"
           << "Sketcher_ConstrainLock"
           << "Sketcher_ConstrainBlock"
           << "Sketcher_ConstrainDistanceX"
           << "Sketcher_ConstrainDistanceY"
           << "Sketcher_ConstrainDistance"
           << "Sketcher_ConstrainRadius"
           << "Sketcher_ConstrainDiameter"
           << "Sketcher_ConstrainRadiam"
           << "Sketcher_ConstrainAngle"

           << "Separator"

           << "Sketcher_ToggleConstruction"
           << "Sketcher_ExternalCmds"

           << "Separator"

           << "Sketcher_SelectConstraints"
           << "Sketcher_SelectOrigin"
           << "Sketcher_SelectHorizontalAxis"
           << "Sketcher_SelectVerticalAxis"

           << "Separator";

    Gui::MenuManager::getInstance()->setupContextMenu(&mitems, menu);

    QAction* remove = menu.addAction(tr("Delete"), this, &ElementView::deleteSelectedItems,
        QKeySequence(QKeySequence::Delete));
    remove->setEnabled(!items.isEmpty());

    menu.menuAction()->setIconVisibleInMenu(true);

    menu.exec(event->globalPos());
}

void ElementView::deleteSelectedItems()
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) return;

    doc->openTransaction("Delete element");
    std::vector<Gui::SelectionObject> sel = Gui::Selection().getSelectionEx(doc->getName());
    for (std::vector<Gui::SelectionObject>::iterator ft = sel.begin(); ft != sel.end(); ++ft) {
        Gui::ViewProvider* vp = Gui::Application::Instance->getViewProvider(ft->getObject());
        if (vp) {
            vp->onDelete(ft->getSubNames());
        }
    }
    doc->commitTransaction();
}

void ElementView::keyPressEvent(QKeyEvent * event)
{
    switch (event->key())
    {
      case Qt::Key_Z:
        // signal
        onFilterShortcutPressed();
        break;
      default:
        inherited::keyPressEvent( event );
        break;
    }
}

// ----------------------------------------------------------------------------

/* TRANSLATOR SketcherGui::TaskSketcherElements */

TaskSketcherElements::TaskSketcherElements(ViewProviderSketch *sketchView)
    : TaskBox(Gui::BitmapFactory().pixmap("document-new"),tr("Elements"),true, 0)
    , sketchView(sketchView)
    , ui(new Ui_TaskSketcherElements())
    , focusItemIndex(-1)
    , previouslySelectedItemIndex(-1)
    , isautoSwitchBoxChecked(false)
    , inhibitSelectionUpdate(false)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui->setupUi(proxy);
#ifdef Q_OS_MAC
    QString cmdKey = QString::fromUtf8("\xe2\x8c\x98"); // U+2318
#else
    // translate the text (it's offered by Qt's translation files)
    // but avoid being picked up by lupdate
    const char* ctrlKey = "Ctrl";
    QString cmdKey = QShortcut::tr(ctrlKey);
#endif
    QString zKey = QStringLiteral("Z");
    ui->Explanation->setText(tr("<html><head/><body><p>&quot;%1&quot;: multiple selection</p>"
                                "<p>&quot;%2&quot;: switch to next valid type</p></body></html>")
                             .arg(cmdKey).arg(zKey));
    ui->elementsWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->elementsWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->elementsWidget->setMouseTracking(true);
    ui->elementsWidget->setColumnCount(5);
#if QT_VERSION >= 0x050000
    ui->elementsWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
#else
    ui->elementsWidget->header()->setResizeMode(0, QHeaderView::ResizeToContents);
#endif
    ui->elementsWidget->header()->setStretchLastSection(false);
    ui->elementsWidget->headerItem()->setText(ColumnIndex::ColType, tr("Type"));
    ui->elementsWidget->headerItem()->setText(ColumnIndex::ColName, tr("Name"));
    ui->elementsWidget->headerItem()->setText(ColumnIndex::ColReference, tr("Reference"));
    ui->elementsWidget->headerItem()->setText(ColumnIndex::ColFlags, tr("Flags"));
    ui->elementsWidget->headerItem()->setText(ColumnIndex::ColMapped, tr("Mapped"));

    // connecting the needed signals
    QObject::connect(
        ui->elementsWidget, SIGNAL(itemSelectionChanged()),
        this                     , SLOT  (on_elementsWidget_itemSelectionChanged())
       );
    QObject::connect(
        ui->elementsWidget, SIGNAL(itemEntered(QTreeWidgetItem *, int)),
        this                     , SLOT  (on_elementsWidget_itemEntered(QTreeWidgetItem *))
       );
    QObject::connect(
        ui->elementsWidget, SIGNAL(onFilterShortcutPressed()),
        this                     , SLOT  (on_elementsWidget_filterShortcutPressed())
       );
    QObject::connect(
        ui->comboBoxElementFilter, SIGNAL(currentIndexChanged(int)),
        this                     , SLOT  (on_elementsWidget_currentFilterChanged(int))
       );
    QObject::connect(
        ui->comboBoxModeFilter, SIGNAL(currentIndexChanged(int)),
        this                  , SLOT  (on_elementsWidget_currentModeFilterChanged(int))
        );
    QObject::connect(
        ui->autoSwitchBox, SIGNAL(stateChanged(int)),
        this                     , SLOT  (on_autoSwitchBox_stateChanged(int))
       );

    connectionElementsChanged = sketchView->getSketchObject()->signalElementsChanged.connect(
        boost::bind(&SketcherGui::TaskSketcherElements::slotElementsChanged, this));

    this->groupLayout()->addWidget(proxy);

    ui->comboBoxElementFilter->setCurrentIndex(0);
    ui->comboBoxModeFilter->setCurrentIndex(0);

    ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Sketcher/Elements");

    ui->autoSwitchBox->setChecked(hGrp->GetBool("Auto-switch to edge", true));

    ui->comboBoxElementFilter->setEnabled(true);
    ui->comboBoxModeFilter->setEnabled(true);

    slotElementsChanged();
}

TaskSketcherElements::~TaskSketcherElements()
{
    try {
        ParameterGrp::handle hGrp = App::GetApplication().GetParameterGroupByPath("User parameter:BaseApp/Preferences/Mod/Sketcher/Elements");
        hGrp->SetBool("Auto-switch to edge", ui->autoSwitchBox->isChecked());
    }
    catch (const Base::Exception&) {
    }

    connectionElementsChanged.disconnect();
}

void TaskSketcherElements::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    std::string temp;
    if (msg.Type == Gui::SelectionChanges::ClrSelection) {
        clearWidget();
    }
    else if (msg.Type == Gui::SelectionChanges::AddSelection ||
             msg.Type == Gui::SelectionChanges::RmvSelection) {
        bool select = (msg.Type == Gui::SelectionChanges::AddSelection);
        App::DocumentObject *selObj = msg.Object.getObject();
        // is it this object??
        if (selObj && selObj->getLinkedObject() == sketchView->getObject()) {
            if (!msg.pSubName)
                return;
            int GeoId;
            Sketcher::PointPos PosId;
            if(!sketchView->getSketchObject()->geoIdFromShapeType(msg.pSubName,GeoId,PosId))
                return;

            auto it = itemMap.find(GeoId);
            if(it == itemMap.end())
                return;

            ElementItem* ite = static_cast<ElementItem*>(it->second);

            switch(PosId) {
            case Sketcher::PointPos::start:
                ite->isStartingPointSelected=select;
                break;
            case Sketcher::PointPos::end:
                ite->isEndPointSelected=select;
                break;
            case Sketcher::PointPos::mid:
                ite->isMidPointSelected=select;
                break;
            default:
                ite->isLineSelected=select;
                break;
            }

            // update the listwidget
            ui->elementsWidget->blockSignals(true);

            int element=ui->comboBoxElementFilter->currentIndex();
            switch(element){
            case 0:
                ite->setSelected(ite->isLineSelected);
                break;
            case 1:
                ite->setSelected(ite->isStartingPointSelected);
                break;
            case 2:
                ite->setSelected(ite->isEndPointSelected);
                break;
            case 3:
                ite->setSelected(ite->isMidPointSelected);
                break;
            }
            if(select)
                ui->elementsWidget->scrollToItem(ite);
            ui->elementsWidget->blockSignals(false);
        }
    }
    else if (msg.Type == Gui::SelectionChanges::SetSelection) {
        // do nothing here
    }
}


void TaskSketcherElements::on_elementsWidget_itemSelectionChanged(void)
{
    ui->elementsWidget->blockSignals(true);


    // selection changed because we acted on the current entered item
    // we can not do this with ItemPressed because that signal is triggered after this one
    int element=ui->comboBoxElementFilter->currentIndex();

    ElementItem * itf;

    if(focusItemIndex>-1 && focusItemIndex<ui->elementsWidget->topLevelItemCount())
      itf=static_cast<ElementItem*>(ui->elementsWidget->topLevelItem(focusItemIndex));
    else
      itf=nullptr;

    bool multipleselection=true; // ctrl type of selection in listWidget
    bool multipleconsecutiveselection=false; // shift type of selection in listWidget

    if (!inhibitSelectionUpdate) {
        if(itf) {
            switch(element){
            case 0:
                itf->isLineSelected=!itf->isLineSelected;
                break;
            case 1:
                itf->isStartingPointSelected=!itf->isStartingPointSelected;
                break;
            case 2:
                itf->isEndPointSelected=!itf->isEndPointSelected;
                break;
            case 3:
                itf->isMidPointSelected=!itf->isMidPointSelected;
                break;
            }
        }

        if (QApplication::keyboardModifiers()==Qt::ControlModifier)// multiple ctrl selection?
            multipleselection=true;
        else
            multipleselection=false;

        if (QApplication::keyboardModifiers()==Qt::ShiftModifier)// multiple shift selection?
            multipleconsecutiveselection=true;
        else
            multipleconsecutiveselection=false;

        if (multipleselection && multipleconsecutiveselection) { // ctrl takes priority over shift functionality
            multipleselection=true;
            multipleconsecutiveselection=false;
        }
    }

    std::string doc_name = sketchView->getSketchObject()->getDocument()->getName();
    std::string obj_name = sketchView->getSketchObject()->getNameInDocument();

    bool block = this->blockSelection(true); // avoid to be notified by itself
    Gui::Selection().clearSelection();


    for (int i=0;i<ui->elementsWidget->topLevelItemCount(); i++) {
        ElementItem * ite=static_cast<ElementItem*>(ui->elementsWidget->topLevelItem(i));

        if(multipleselection==false && multipleconsecutiveselection==false && ite!=itf) {
            ite->isLineSelected=false;
            ite->isStartingPointSelected=false;
            ite->isEndPointSelected=false;
            ite->isMidPointSelected=false;
        }

        if( multipleconsecutiveselection) {
            if ((( i>focusItemIndex && i<previouslySelectedItemIndex ) ||
                 ( i<focusItemIndex && i>previouslySelectedItemIndex )) &&
                previouslySelectedItemIndex>=0){
              // select the element of the Item
                      switch(element){
                  case 0:
                      ite->isLineSelected=true;
                      break;
                  case 1:
                      ite->isStartingPointSelected=true;
                      break;
                  case 2:
                      ite->isEndPointSelected=true;
                      break;
                  case 3:
                      ite->isMidPointSelected=true;
                      break;
                }
            }
        }

        // first update the listwidget
        switch(element){
          case 0:
              ite->setSelected(ite->isLineSelected);
              break;
          case 1:
              ite->setSelected(ite->isStartingPointSelected);
              break;
          case 2:
              ite->setSelected(ite->isEndPointSelected);
              break;
          case 3:
              ite->setSelected(ite->isMidPointSelected);
              break;
        }

        // now the scene
        std::stringstream ss;
        int vertex;

        if (ite->isLineSelected
                || (isautoSwitchBoxChecked 
                    && (ite->isStartingPointSelected
                        || ite->isMidPointSelected
                        || ite->isEndPointSelected)))
        {
            if (ite->GeometryType == Part::GeomPoint::getClassTypeId()) {
                vertex= ite->StartingVertex;
                if (vertex!=-1) {
                    ss << "Vertex" << vertex + 1;
                    sketchView->selectElement(ss.str().c_str());
                }
            } 
            else if(ite->ElementNbr>=0)
                ss << "Edge" << ite->ElementNbr + 1;
            else
                ss << "ExternalEdge" << -ite->ElementNbr - 2;
            sketchView->selectElement(ss.str().c_str());
        }
        else if (ite->isStartingPointSelected) {
            ss.str(std::string());
            vertex= ite->StartingVertex;
            if (vertex!=-1) {
                ss << "Vertex" << vertex + 1;
                sketchView->selectElement(ss.str().c_str());
            }
        }
        else if (ite->isEndPointSelected) {
            ss.str(std::string());
            vertex= ite->EndVertex;
            if (vertex!=-1) {
                ss << "Vertex" << vertex + 1;
                sketchView->selectElement(ss.str().c_str());
            }
        }
        else if (ite->isMidPointSelected) {
            ss.str(std::string());
            vertex= ite->MidVertex;
            if (vertex!=-1) {
                ss << "Vertex" << vertex + 1;
                sketchView->selectElement(ss.str().c_str());
            }
        }
    }

    this->blockSelection(block);
    ui->elementsWidget->blockSignals(false);

    if (focusItemIndex>-1 && focusItemIndex<ui->elementsWidget->topLevelItemCount())
        previouslySelectedItemIndex=focusItemIndex;
}

void TaskSketcherElements::on_elementsWidget_itemEntered(QTreeWidgetItem *item)
{
    ElementItem *it = dynamic_cast<ElementItem*>(item);
    if (!it) return;

    Gui::Selection().rmvPreselect();

    ui->elementsWidget->setFocus();

    int tempitemindex=ui->elementsWidget->indexOfTopLevelItem(item);

    std::string doc_name = sketchView->getSketchObject()->getDocument()->getName();
    std::string obj_name = sketchView->getSketchObject()->getNameInDocument();

    /* 0 - Lines
     * 1 - Starting Points
     * 2 - End Points
     * 3 - Middle Points
     */
    std::stringstream ss;


    int element=ui->comboBoxElementFilter->currentIndex();

    focusItemIndex=tempitemindex;

    int vertex;

    switch(element)
    {
    case 0:
        if (it->GeometryType == Part::GeomPoint::getClassTypeId()) {
            vertex= it->StartingVertex;
            if (vertex!=-1) {
                ss << "Vertex" << vertex + 1;
                sketchView->selectElement(ss.str().c_str(), true);
            }
        } 
        else {
            if(it->ElementNbr>=0)
                ss << "Edge" << it->ElementNbr + 1;
            else
                ss << "ExternalEdge" << -it->ElementNbr - 2;
            sketchView->selectElement(ss.str().c_str(), true);
        }
        break;
    case 1:
    case 2:
    case 3:
        vertex= sketchView->getSketchObject()->getVertexIndexGeoPos(it->ElementNbr,static_cast<Sketcher::PointPos>(element));
        if (vertex!=-1) {
            ss << "Vertex" << vertex + 1;
            sketchView->selectElement(ss.str().c_str(), true);
        }
        break;
    }
}

void TaskSketcherElements::leaveEvent (QEvent * event)
{
    Q_UNUSED(event);
    Gui::Selection().rmvPreselect();
    ui->elementsWidget->clearFocus();
}

void TaskSketcherElements::slotElementsChanged(void)
{
    assert(sketchView);
    // Build up ListView with the elements
    const std::vector< Part::Geometry * > &vals = sketchView->getSketchObject()->Geometry.getValues();

    int currentRow = -1;
    auto currentIndex = ui->elementsWidget->currentIndex();
    if (currentIndex.isValid() && ui->elementsWidget->currentItem()->isSelected())
        currentRow = currentIndex.row();

    ui->elementsWidget->blockSignals(true);
    ui->elementsWidget->clear();
    itemMap.clear();

    int element = ui->comboBoxElementFilter->currentIndex();
    int filterindex = ui->comboBoxModeFilter->currentIndex();

    auto sketch = sketchView->getSketchObject();
    for(int i=0;i<(int)vals.size();++i) {
        auto item = new ElementItem(ui->elementsWidget,sketch, i, vals[i]);
        item->setElement(sketch,element, filterindex);
        itemMap[item->ElementNbr] = item;
    }

    const std::vector< Part::Geometry * > &ext_vals = sketchView->getSketchObject()->getExternalGeometry();
    for(int i=2;i<(int)ext_vals.size();++i) {
        auto item = new ElementItem(ui->elementsWidget,sketch, -i-1, ext_vals[i]);
        item->setElement(sketch,element, filterindex);
        itemMap[item->ElementNbr] = item;
    }

    for(int i = 0; i < ui->elementsWidget->columnCount(); i++)
        ui->elementsWidget->resizeColumnToContents(i);

    ui->elementsWidget->blockSignals(false);

    if (currentRow >= ui->elementsWidget->topLevelItemCount())
        currentRow = ui->elementsWidget->topLevelItemCount()-1;
    if (currentRow >= 0) {
        if (auto item = ui->elementsWidget->topLevelItem(currentRow)) {
            ui->elementsWidget->setCurrentItem(item);
        }
    }
}


void TaskSketcherElements::on_elementsWidget_filterShortcutPressed()
{
    int element;

    previouslySelectedItemIndex=-1; // Shift selection on list widget implementation

    // calculate next element type on shift press according to entered/preselected element
    // This is the aka fast-forward functionality
    if(focusItemIndex>-1 && focusItemIndex<ui->elementsWidget->topLevelItemCount()){

      ElementItem * itf=static_cast<ElementItem*>(ui->elementsWidget->topLevelItem(focusItemIndex));

      Base::Type type = itf->GeometryType;

      element = ui->comboBoxElementFilter->currentIndex(); // currently selected type index

      switch(element)
      {

        case 0: // Edge
          element =        ( type == Part::GeomCircle::getClassTypeId() || type == Part::GeomEllipse::getClassTypeId() ) ? 3 : 1;
          break;
        case 1: // StartingPoint
          element =        ( type == Part::GeomCircle::getClassTypeId() || type == Part::GeomEllipse::getClassTypeId() ) ? 3 :
                            ( type == Part::GeomPoint::getClassTypeId()  ) ? 1 : 2;
          break;
        case 2: // EndPoint
          element =        ( type == Part::GeomLineSegment::getClassTypeId() ) ? 0 :
                            ( type == Part::GeomPoint::getClassTypeId()  ) ? 1 : 3;
          break;
        case 3: // MidPoint
          element =        ( type == Part::GeomPoint::getClassTypeId()  ) ? 1 : 0;
          break;
        default:
          element = 0;
      }

      ui->comboBoxElementFilter->setCurrentIndex(element);

      Gui::Selection().rmvPreselect();

      on_elementsWidget_itemEntered(itf);
    }
    else{
      element = (ui->comboBoxElementFilter->currentIndex()+1) %
                ui->comboBoxElementFilter->count();

      ui->comboBoxElementFilter->setCurrentIndex(element);

      Gui::Selection().rmvPreselect();
    }

    //update the icon
    updateIcons(element);

    updatePreselection();
}

void TaskSketcherElements::on_autoSwitchBox_stateChanged(int state)
{
      isautoSwitchBoxChecked=(state==Qt::Checked);
}

void TaskSketcherElements::on_elementsWidget_currentFilterChanged ( int index )
{
    previouslySelectedItemIndex=-1; // Shift selection on list widget implementation

    Gui::Selection().rmvPreselect();

    updateIcons(index);

    updatePreselection();

}

void TaskSketcherElements::on_elementsWidget_currentModeFilterChanged ( int index )
{
    updateVisibility(index);
}

void TaskSketcherElements::updatePreselection()
{
    inhibitSelectionUpdate=true;
    on_elementsWidget_itemSelectionChanged();
    inhibitSelectionUpdate=false;
}

void TaskSketcherElements::clearWidget()
{
    QSignalBlocker sigblk(ui->elementsWidget);
    ui->elementsWidget->clearSelection ();

    // update widget
    int countItems = ui->elementsWidget->topLevelItemCount();
    for (int i=0; i < countItems; i++) {
      ElementItem* item = static_cast<ElementItem*> (ui->elementsWidget->topLevelItem(i));
      item->isLineSelected=false;
      item->isStartingPointSelected=false;
      item->isEndPointSelected=false;
      item->isMidPointSelected=false;
    }
}

void TaskSketcherElements::setItemVisibility(int elementindex,int filterindex)
{
    // index
    // 0 => all
    // 1 => Normal
    // 2 => Construction
    // 3 => External

    ElementItem* item = static_cast<ElementItem*> (ui->elementsWidget->topLevelItem(elementindex));
    item->setVisibility(filterindex);
}

void TaskSketcherElements::updateVisibility(int filterindex)
{
    for (int i=0;i<ui->elementsWidget->topLevelItemCount(); i++) {
        setItemVisibility(i,filterindex);
    }
}

void TaskSketcherElements::updateIcons(int element)
{
    int filterindex = ui->comboBoxModeFilter->currentIndex();
    auto sketch = sketchView->getSketchObject();
    for (int i=0;i<ui->elementsWidget->topLevelItemCount(); i++)
      static_cast<ElementItem *>(ui->elementsWidget->topLevelItem(i))->setElement(sketch,element,filterindex);
}

void TaskSketcherElements::changeEvent(QEvent *e)
{
    TaskBox::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(proxy);
    }
}

MultIcon & MultIcon::operator=(const char* name)
{
    int hue, sat, val, alp;
    Normal = Gui::BitmapFactory().iconFromTheme(name);
    QImage imgConstr(Normal.pixmap(qAsConst(Normal).availableSizes()[0]).toImage());
    QImage imgExt(imgConstr);
    QImage imgInt(imgConstr);

    //Create construction/external/internal icons by changing colors.
    for(int ix=0 ; ix<imgConstr.width() ; ix++) {
        for(int iy=0 ; iy<imgConstr.height() ; iy++) {
            QColor clr(imgConstr.pixelColor(ix,iy));
            clr.getHsv(&hue, &sat, &val, &alp);
            if (alp > 127 && hue >= 0) {
                if (sat > 127 && (hue > 330 || hue < 30)) { //change the color of red points.
                    clr.setHsv((hue + 240) % 360, sat, val, alp);
                    imgConstr.setPixelColor(ix, iy, clr);
                    clr.setHsv((hue + 300) % 360, sat, val, alp);
                    imgExt.setPixelColor(ix, iy, clr);
                    clr.setHsv((hue + 60) % 360, (int) (sat / 3), std::min((int) (val * 8 / 7), 255), alp);
                    imgInt.setPixelColor(ix, iy, clr);
                }
                else if (sat < 64 && val > 192) { //change the color of white edges.
                    clr.setHsv(240, (255-sat), val, alp);
                    imgConstr.setPixel(ix, iy, clr.rgba());
                    clr.setHsv(300, (255-sat), val, alp);
                    imgExt.setPixel(ix, iy, clr.rgba());
                    clr.setHsv(60, (int) (255-sat) / 2, val, alp);
                    imgInt.setPixel(ix, iy, clr.rgba());
                }
            }
        }
    }
    Construction = QIcon(QPixmap::fromImage(imgConstr));
    External = QIcon(QPixmap::fromImage(imgExt));
    Internal = QIcon(QPixmap::fromImage(imgInt));
    return *this;
}


#include "moc_TaskSketcherElements.cpp"
