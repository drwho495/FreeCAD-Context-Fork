/***************************************************************************
 *   Copyright (c) 2007 Jürgen Riegel <juergen.riegel@web.de>              *
 *   Copyright (c) 2013 Luke Parry <l.parry@warwick.ac.uk>                 *
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
#include <QAction>
#include <QApplication>
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QPageLayout>
#include <QPageSize>
#include <QPaintEngine>
#include <QPainter>
#include <QPdfWriter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <boost_signals2.hpp>
#include <cmath>
#endif

#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Base/Console.h>
#include <Base/Stream.h>
#include <Base/Tools.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/FileDialog.h>
#include <Gui/MainWindow.h>
#include <Gui/MDIViewPy.h>
#include <Gui/Selection.h>
#include <Gui/SelectionObject.h>
#include <Gui/ViewProvider.h>
#include <Gui/WaitCursor.h>
#include <Gui/Window.h>
#include <Mod/TechDraw/App/DrawPage.h>
#include <Mod/TechDraw/App/DrawPagePy.h>
#include <Mod/TechDraw/App/DrawTemplate.h>
#include <Mod/TechDraw/App/DrawViewDetail.h>
#include <Mod/TechDraw/App/DrawHatch.h>
#include <Mod/TechDraw/App/DrawGeomHatch.h>
#include <Mod/TechDraw/App/DrawViewSection.h>
#include <Mod/TechDraw/App/DrawTile.h>
#include <Mod/TechDraw/App/Preferences.h>

#include "MDIViewPage.h"
#include "QGIEdge.h"
#include "QGIFace.h"
#include "QGIHighlight.h"
#include "QGITemplate.h"
#include "QGIVertex.h"
#include "QGIView.h"
#include "QGIViewBalloon.h"
#include "QGIViewDimension.h"
#include "QGISectionLine.h"
#include "QGITile.h"
#include "QGIViewPart.h"
#include "QGMText.h"
#include "QGSPage.h"
#include "QGVPage.h"
#include "Rez.h"
#include "ViewProviderPage.h"


using namespace TechDrawGui;
using namespace TechDraw;
namespace bp = boost::placeholders;

/* TRANSLATOR TechDrawGui::MDIViewPage */

TYPESYSTEM_SOURCE_ABSTRACT(TechDrawGui::MDIViewPage, Gui::MDIView)

MDIViewPage::MDIViewPage(ViewProviderPage* pageVp, Gui::Document* doc, QWidget* parent)
    : Gui::MDIView(doc, parent), m_vpPage(pageVp), m_orientation(QPageLayout::Landscape),
      m_paperSize(QPageSize::A4), m_pagewidth(0.0), m_pageheight(0.0)
{
    setMouseTracking(true);

    m_toggleKeepUpdatedAction = new QAction(tr("Toggle &Keep Updated"), this);
    connect(m_toggleKeepUpdatedAction, &QAction::triggered, this, &MDIViewPage::toggleKeepUpdated);

    m_toggleFrameAction = new QAction(tr("Toggle &Frames"), this);
    connect(m_toggleFrameAction, &QAction::triggered, this, &MDIViewPage::toggleFrame);

    m_exportSVGAction = new QAction(tr("&Export SVG"), this);
    connect(m_exportSVGAction, &QAction::triggered, this, qOverload<>(&MDIViewPage::saveSVG));

    m_exportDXFAction = new QAction(tr("Export DXF"), this);
    connect(m_exportDXFAction, &QAction::triggered, this, qOverload<>(&MDIViewPage::saveDXF));

    m_exportPDFAction = new QAction(tr("Export PDF"), this);
    connect(m_exportPDFAction, &QAction::triggered, this, qOverload<>(&MDIViewPage::savePDF));

    m_printAllAction = new QAction(tr("Print All Pages"), this);
    connect(m_printAllAction, &QAction::triggered, this, qOverload<>(&MDIViewPage::printAll));

    isSelectionBlocked = 0;

    QString tabText = QString::fromUtf8(pageVp->getDrawPage()->getNameInDocument());
    tabText += QString::fromUtf8("[*]");
    setWindowTitle(tabText);

    //get informed by App side about deleted DocumentObjects
    App::Document* appDoc = m_vpPage->getDocument()->getDocument();
    auto bnd = boost::bind(&MDIViewPage::onDeleteObject, this, bp::_1);
    connectDeletedObject = appDoc->signalDeletedObject.connect(bnd);
}

MDIViewPage::~MDIViewPage() { connectDeletedObject.disconnect(); }

void MDIViewPage::setScene(QGSPage* scene, QGVPage* viewWidget)
{
    m_scene = scene;
    setCentralWidget(viewWidget);//this makes viewWidget a Qt child of MDIViewPage
    QObject::connect(scene, &QGSPage::selectionChanged, this, &MDIViewPage::sceneSelectionChanged);
}

void MDIViewPage::setDocumentObject(const std::string& name)
{
    m_objectName = name;
    setObjectName(Base::Tools::fromStdString(name));
}

void MDIViewPage::setDocumentName(const std::string& name) { m_documentName = name; }

void MDIViewPage::closeEvent(QCloseEvent* event)
{
    //    Base::Console().Message("MDIVP::closeEvent()\n");
    MDIView::closeEvent(event);
    if (!event->isAccepted()) {
        return;
    }
    detachSelection();

    blockSceneSelection(true);
    // when closing the view from GUI notify the view provider to mark it invisible
    if (_pcDocument && !m_objectName.empty()) {
        App::Document* doc = _pcDocument->getDocument();
        if (doc) {
            App::DocumentObject* obj = doc->getObject(m_objectName.c_str());
            Gui::ViewProvider* vp = _pcDocument->getViewProvider(obj);
            if (vp) {
                vp->hide();
            }
        }
    }
    blockSceneSelection(false);
}

void MDIViewPage::onDeleteObject(const App::DocumentObject& obj)
{
    //if this page has a QView for this obj, delete it.
    blockSceneSelection(true);
    if (obj.isDerivedFrom(TechDraw::DrawView::getClassTypeId())) {
        (void)m_scene->removeQViewByName(obj.getNameInDocument());
    }
    blockSceneSelection(false);
}

bool MDIViewPage::onMsg(const char *pMsg, const char **ppReturn)
{
    Gui::Document* doc(getGuiDocument());

    if (!doc) {
        return false;
    }
    else if (strcmp("ViewFit", pMsg) == 0) {
        viewAll();
        return true;
    }
    else if (strcmp("Save", pMsg) == 0) {
        doc->save();
        return true;
    }
    else if (strcmp("SaveAs", pMsg) == 0) {
        doc->saveAs();
        return true;
    }
    else if (strcmp("Undo", pMsg) == 0) {
        doc->undo(1);
        Gui::Command::updateActive();
        return true;
    }
    else if (strcmp("Redo", pMsg) == 0) {
        doc->redo(1);
        Gui::Command::updateActive();
        return true;
    }

    return MDIView::onMsg(pMsg,ppReturn);
}

bool MDIViewPage::onHasMsg(const char* pMsg) const
{
    if (strcmp("CanPan",pMsg) == 0) {
        return true;
    }
    else if (strcmp("ViewFit",pMsg) == 0) {
        return true;
    }
    else if (strcmp("Redo", pMsg) == 0 && getAppDocument()->getAvailableRedos() > 0) {
        return true;
    }
    else if (strcmp("Undo", pMsg) == 0 && getAppDocument()->getAvailableUndos() > 0) {
        return true;
    }
    else if (strcmp("Print", pMsg) == 0) {
        return true;
    }
    else if (strcmp("Save", pMsg) == 0) {
        return true;
    }
    else if (strcmp("SaveAs", pMsg) == 0) {
        return true;
    }
    else if (strcmp("PrintPreview", pMsg) == 0) {
        return true;
    }
    else if (strcmp("PrintPdf", pMsg) == 0) {
        return true;
    }
    else if (strcmp("PrintAll", pMsg) == 0) {
        return true;
    }
    return false;
}

//called by ViewProvider when Page feature Label changes
void MDIViewPage::setTabText(std::string tabText)
{
    if (!isPassive() && !tabText.empty()) {
        QString cap = QStringLiteral("%1 [*]")
            .arg(QString::fromUtf8(tabText.c_str()));
        setWindowTitle(cap);
    }
}

static inline QString _getDefaultName(const Gui::ViewProviderDocumentObject *vpd) {
    if(!vpd || !vpd->getObject())
        return QString();
    auto obj = vpd->getObject();
    if(obj->getDocument() 
           && obj->getDocument()->FileName.getStrValue().size())
    {
        QDir dir = QFileInfo(QString::fromUtf8(
                    obj->getDocument()->FileName.getValue())).dir();
        return QFileInfo(dir,QString::fromUtf8(obj->Label.getValue())).filePath();
    } else
        return QString::fromUtf8(obj->Label.getValue());
}

//**** printing routines

void MDIViewPage::getPaperAttributes()
{
    App::DocumentObject* obj = m_vpPage->getDrawPage()->Template.getValue();
    auto pageTemplate(dynamic_cast<TechDraw::DrawTemplate*>(obj));
    if (pageTemplate) {
        m_pagewidth = pageTemplate->Width.getValue();
        m_pageheight = pageTemplate->Height.getValue();
    }
    m_paperSize = QPageSize::id(QSizeF(m_pagewidth, m_pageheight), QPageSize::Millimeter,
                                QPageSize::FuzzyOrientationMatch);
    if (m_pagewidth > m_pageheight) {
        m_orientation = QPageLayout::Landscape;
    }
    else {
        m_orientation = QPageLayout::Portrait;
    }
}

void MDIViewPage::printPdf()
{
    QStringList filter;
    filter << QObject::tr("PDF (*.pdf)");
    filter << QObject::tr("All Files (*.*)");
    QString fn = Gui::FileDialog::getSaveFileName(Gui::getMainWindow(), QObject::tr("Export Page As PDF"),
                                                  _getDefaultName(m_vpPage), filter.join(QStringLiteral(";;")));
    if (fn.isEmpty()) {
        return;
    }

    Gui::WaitCursor wc;
    std::string utf8Content = fn.toUtf8().constData();
    printPdf(utf8Content);
}

void MDIViewPage::printPdf(std::string file)
{
    if (file.empty()) {
        Base::Console().Warning("MDIViewPage - no file specified\n");
        return;
    }
    getPaperAttributes();

    QString filename = QString::fromUtf8(file.data(), file.size());
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    printer.setOutputFileName(filename);

    if (m_paperSize == QPageSize::Ledger) {
        printer.setPageOrientation((QPageLayout::Orientation)(1 - m_orientation));//reverse 0/1
    }
    else {
        printer.setPageOrientation(m_orientation);
    }
    if (m_paperSize == QPageSize::Custom) {
        printer.setPageSize(QPageSize(QSizeF(m_pagewidth, m_pageheight), QPageSize::Millimeter));
    }
    else {
        printer.setPageSize(QPageSize(m_paperSize));
    }
    print(&printer);
}

void MDIViewPage::print()
{
    //    Base::Console().Message("MDIVP::print()\n");
    getPaperAttributes();

    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    if (m_paperSize == QPageSize::Custom) {
        printer.setPageSize(QPageSize(QSizeF(m_pagewidth, m_pageheight), QPageSize::Millimeter));
    }
    else {
        printer.setPageSize(QPageSize(m_paperSize));
    }
    printer.setPageOrientation(m_orientation);

    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        print(&printer);
    }
}

void MDIViewPage::printPreview()
{
    getPaperAttributes();

    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);
    if (m_paperSize == QPageSize::Custom) {
        printer.setPageSize(QPageSize(QSizeF(m_pagewidth, m_pageheight), QPageSize::Millimeter));
    }
    else {
        printer.setPageSize(QPageSize(m_paperSize));
    }
    printer.setPageOrientation(m_orientation);

    QPrintPreviewDialog dlg(&printer, this);
    connect(&dlg, &QPrintPreviewDialog::paintRequested, this, qOverload<QPrinter*>(&MDIViewPage::print));
    dlg.exec();
}


void MDIViewPage::print(QPrinter* printer)
{
    //    Base::Console().Message("MDIVP::print(printer)\n");
    getPaperAttributes();

    // As size of the render area paperRect() should be used. When performing a real
    // print pageRect() may also work but the output is cropped at the bottom part.
    // So, independent whether pageRect() or paperRect() is used there is no scaling effect.
    // However, when using a different paper size as set in the drawing template (e.g.
    // DIN A5 instead of DIN A4) then the output is scaled.
    //
    // When creating a PDF file there seems to be no difference between pageRect() and
    // paperRect().
    //
    // When showing the preview of a print paperRect() must be used because with pageRect()
    // a certain scaling effect can be observed and the content becomes smaller.
    QPaintEngine::Type paintType = printer->paintEngine()->type();
    if (printer->outputFormat() == QPrinter::NativeFormat) {
        QPageSize::PageSizeId psPrtSetting = printer->pageLayout().pageSize().id();

        // for the preview a 'Picture' paint engine is used which we don't
        // care if it uses wrong printer settings
        bool doPrint = paintType != QPaintEngine::Picture;

        if (doPrint && printer->pageLayout().orientation() != m_orientation) {
            int ret = QMessageBox::warning(
                this, tr("Different orientation"),
                tr("The printer uses a different orientation  than the drawing.\n"
                   "Do you want to continue?"),
                QMessageBox::Yes | QMessageBox::No);
            if (ret != QMessageBox::Yes) {
                return;
            }
        }
        if (doPrint && psPrtSetting != m_paperSize) {
            int ret = QMessageBox::warning(
                this, tr("Different paper size"),
                tr("The printer uses a different paper size than the drawing.\n"
                   "Do you want to continue?"),
                QMessageBox::Yes | QMessageBox::No);
            if (ret != QMessageBox::Yes) {
                return;
            }
        }
    }

    QPainter p(printer);
    if (!p.isActive() && !printer->outputFileName().isEmpty()) {
        qApp->setOverrideCursor(Qt::ArrowCursor);
        QMessageBox::critical(
            this, tr("Opening file failed"),
            tr("Can not open file %1 for writing.").arg(printer->outputFileName()));
        qApp->restoreOverrideCursor();
        return;
    }

    QRect targetRect = printer->pageLayout().fullRectPixels(printer->resolution());

#ifdef Q_OS_WIN32
    // On Windows the preview looks broken when using paperRect as render area.
    // Although the picture is scaled when using pageRect, it looks just fine.
    if (paintType == QPaintEngine::Picture) {
        targetRect = printer->pageLayout().paintRectPixels(printer->resolution());
    }
#endif

    //bool block =
    static_cast<void>(blockSelection(true));// avoid to be notified by itself
    Gui::Selection().clearSelection();

    bool saveState = m_vpPage->getFrameState();
    m_vpPage->setFrameState(false);
    m_vpPage->setTemplateMarkers(false);
    m_scene->refreshViews();

    Gui::Selection().clearSelection();

    App::DocumentObject* obj = m_vpPage->getDrawPage()->Template.getValue();
    auto pageTemplate(dynamic_cast<TechDraw::DrawTemplate*>(obj));
    double width = 0.0;
    double height = 0.0;
    if (pageTemplate) {
        width = Rez::guiX(pageTemplate->Width.getValue());
        height = Rez::guiX(pageTemplate->Height.getValue());
    }
    QRectF sourceRect(0.0, -height, width, height);

    //scene might be drawn in light text.  we need to redraw in normal text.
    bool saveLightOnDark = Preferences::lightOnDark();
    if (Preferences::lightOnDark()) {
        Preferences::lightOnDark(false);
        m_vpPage->getQGSPage()->redrawAllViews();
        m_vpPage->getQTemplate()->updateView();
    }
    m_scene->render(&p, targetRect, sourceRect);

    // Reset
    m_vpPage->setFrameState(saveState);
    m_vpPage->setTemplateMarkers(saveState);
    Preferences::lightOnDark(saveLightOnDark);
    m_scene->refreshViews();
    m_vpPage->getQTemplate()->updateView();
    //bool block =
    static_cast<void>(blockSelection(false));
}

//static routine to print all pages in a document
void MDIViewPage::printAll(QPrinter* printer, App::Document* doc)
{
    //    Base::Console().Message("MDIVP::printAll()\n");
    QPainter painter(printer);
    QPageLayout pageLayout = printer->pageLayout();
    bool firstTime = true;
    std::vector<App::DocumentObject*> docObjs =
        doc->getObjectsOfType(TechDraw::DrawPage::getClassTypeId());
    for (auto& obj : docObjs) {
        Gui::ViewProvider* vp = Gui::Application::Instance->getViewProvider(obj);
        if (!vp) {
            continue;// can't print this one
        }
        TechDrawGui::ViewProviderPage* vpp = dynamic_cast<TechDrawGui::ViewProviderPage*>(vp);
        if (!vpp) {
            continue;// can't print this one
        }

        TechDraw::DrawPage* dp = static_cast<TechDraw::DrawPage*>(obj);
        double width = 297.0;//default to A4 Landscape 297 x 210
        double height = 210.0;
        setPageLayout(pageLayout, dp, width, height);
        printer->setPageLayout(pageLayout);

        //for some reason the first page doesn't obey the pageLayout, so we have to print
        //a sacrificial blank page, but we make it a feature instead of a bug by printing a
        //table of contents on the sacrificial page.
        if (firstTime) {
            firstTime = false;
            printBannerPage(printer, painter, pageLayout, doc, docObjs);
        }

        printer->newPage();
        QRectF sourceRect(0.0, Rez::guiX(-height), Rez::guiX(width), Rez::guiX(height));
        QRect targetRect = printer->pageLayout().fullRectPixels(printer->resolution());

        renderPage(vpp, painter, sourceRect, targetRect);
    }
    painter.end();
}

//static routine to print all pages in a document to pdf
void MDIViewPage::printAllPdf(QPrinter* printer, App::Document* doc)
{
    //    Base::Console().Message("MDIVP::printAllPdf()\n");
    QString outputFile = printer->outputFileName();
    QString documentName = QString::fromUtf8(doc->getName());
    QPdfWriter pdfWriter(outputFile);
    pdfWriter.setTitle(documentName);
    pdfWriter.setResolution(printer->resolution());
    QPainter painter(&pdfWriter);
    QPageLayout pageLayout = printer->pageLayout();

    double dpmm = printer->resolution() / 25.4;
    bool firstTime = true;
    std::vector<App::DocumentObject*> docObjs =
        doc->getObjectsOfType(TechDraw::DrawPage::getClassTypeId());
    for (auto& obj : docObjs) {
        Gui::ViewProvider* vp = Gui::Application::Instance->getViewProvider(obj);
        if (!vp) {
            continue;// can't print this one
        }
        TechDrawGui::ViewProviderPage* vpp = dynamic_cast<TechDrawGui::ViewProviderPage*>(vp);
        if (!vpp) {
            continue;// can't print this one
        }

        TechDraw::DrawPage* dp = static_cast<TechDraw::DrawPage*>(obj);
        double width = 297.0;//default to A4 Landscape 297 x 210
        double height = 210.0;
        setPageLayout(pageLayout, dp, width, height);
        pdfWriter.setPageLayout(pageLayout);

        //for some reason the first page doesn't obey the pageLayout, so we have to print
        //a sacrificial blank page, but we make it a feature instead of a bug by printing a
        //table of contents on the sacrificial page.
        if (firstTime) {
            firstTime = false;
            printBannerPage(printer, painter, pageLayout, doc, docObjs);
        }
        pdfWriter.newPage();

        QRectF sourceRect(0.0, Rez::guiX(-height), Rez::guiX(width), Rez::guiX(height));
        QRect targetRect(0, 0, width * dpmm, height * dpmm);

        renderPage(vpp, painter, sourceRect, targetRect);
    }
    painter.end();
}

//static
void MDIViewPage::printBannerPage(QPrinter* printer, QPainter& painter, QPageLayout& pageLayout,
                                  App::Document* doc, std::vector<App::DocumentObject*>& docObjs)
{
    QFont savePainterFont = painter.font();
    QFont painterFont;
    painterFont.setFamily(Preferences::labelFontQString());
    int fontSizeMM = Preferences::labelFontSizeMM();
    double dpmm = printer->resolution() / 25.4;
    int fontSizePx = fontSizeMM * dpmm;
    painterFont.setPixelSize(fontSizePx);
    painter.setFont(painterFont);

    //print a header
    QString docLine = QObject::tr("Document Name: ") + QString::fromUtf8(doc->getName());
    int leftMargin = pageLayout.margins().left() * dpmm + 5 * dpmm; //layout margin + 5mm
    int verticalPos = pageLayout.margins().top() * dpmm + 20 * dpmm;//layout margin + 20mm
    int verticalSpacing = 2;                                        //double space
    painter.drawText(leftMargin, verticalPos, docLine);

    //leave some blank space between document name and page entries
    verticalPos += 2 * verticalSpacing * fontSizePx;
    for (auto& obj : docObjs) {
        //print a line for each page
        QString pageLine = QString::fromUtf8(obj->getNameInDocument()) + QString::fromUtf8(" / ")
            + QString::fromUtf8(obj->Label.getValue());
        painter.drawText(leftMargin, verticalPos, pageLine);
        verticalPos += verticalSpacing * fontSizePx;
    }
    painter.setFont(savePainterFont);//restore the original font
}

//static
void MDIViewPage::renderPage(ViewProviderPage* vpp, QPainter& painter, QRectF& sourceRect,
                             QRect& targetRect)
{
    //turn off view frames for print
    bool saveState = vpp->getFrameState();
    vpp->setFrameState(false);
    vpp->setTemplateMarkers(false);

    //scene might be drawn in light text.  we need to redraw in normal text.
    bool saveLightOnDark = Preferences::lightOnDark();
    if (Preferences::lightOnDark()) {
        Preferences::lightOnDark(false);
        vpp->getQGSPage()->redrawAllViews();
    }

    vpp->getQGSPage()->refreshViews();
    vpp->getQGSPage()->render(&painter, targetRect, sourceRect);

    // Reset
    vpp->setFrameState(saveState);
    vpp->setTemplateMarkers(saveState);
    Preferences::lightOnDark(saveLightOnDark);

    vpp->getQGSPage()->refreshViews();
}

//static
void MDIViewPage::setPageLayout(QPageLayout& pageLayout, TechDraw::DrawPage* dPage, double& width,
                                double& height)
{
    auto pageTemplate(dynamic_cast<TechDraw::DrawTemplate*>(dPage->Template.getValue()));
    if (pageTemplate) {
        width = pageTemplate->Width.getValue();
        height = pageTemplate->Height.getValue();
    }
    //Qt's page size determination assumes Portrait orientation. To get the right paper size
    //we need to ask in the proper form.
    QPageSize::PageSizeId paperSizeID =
        QPageSize::id(QSizeF(std::min(width, height), std::max(width, height)),
                      QPageSize::Millimeter, QPageSize::FuzzyOrientationMatch);
    if (paperSizeID == QPageSize::Custom) {
        pageLayout.setPageSize(QPageSize(QSizeF(std::min(width, height), std::max(width, height)),
                                         QPageSize::Millimeter));
    }
    else {
        pageLayout.setPageSize(QPageSize(paperSizeID));
    }
    pageLayout.setOrientation((QPageLayout::Orientation)dPage->getOrientation());
}

PyObject* MDIViewPage::getPyObject()
{
    if (pythonObject.is(Py::_None())) {
        // ref counter is set to 1
        pythonObject = Py::Object(new MDIViewPagePy(this), true);
    }
    return Py::new_reference_to(pythonObject);
}

void MDIViewPage::contextMenuEvent(QContextMenuEvent* event)
{
    //    Base::Console().Message("MDIVP::contextMenuEvent() - reason: %d\n", event->reason());
    QMenu menu;
    menu.addAction(m_toggleFrameAction);
    menu.addAction(m_toggleKeepUpdatedAction);
    menu.addAction(m_exportSVGAction);
    menu.addAction(m_exportDXFAction);
    menu.addAction(m_exportPDFAction);
    menu.addAction(m_printAllAction);
    menu.exec(event->globalPos());
}

void MDIViewPage::toggleFrame() { m_vpPage->toggleFrameState(); }

void MDIViewPage::toggleKeepUpdated()
{
    bool state = m_vpPage->getDrawPage()->KeepUpdated.getValue();
    m_vpPage->getDrawPage()->KeepUpdated.setValue(!state);
}

void MDIViewPage::viewAll()
{
    m_vpPage->getQGVPage()->fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MDIViewPage::saveSVG()
{
    QStringList filter;
    filter << QObject::tr("SVG (*.svg)");
    filter << QObject::tr("All Files (*.*)");
    QString fn = Gui::FileDialog::getSaveFileName(Gui::getMainWindow(), QObject::tr("Export page as SVG"),
                                                  _getDefaultName(m_vpPage), filter.join(QStringLiteral(";;")));
    if (fn.isEmpty()) {
        return;
    }
    static_cast<void>(blockSelection(true));// avoid to be notified by itself

    m_scene->saveSvg(fn);
}

void MDIViewPage::saveSVG(std::string file)
{
    if (file.empty()) {
        Base::Console().Warning("MDIViewPage - no file specified\n");
        return;
    }
    QString filename = QString::fromUtf8(file.data(), file.size());
    m_scene->saveSvg(filename);
}

void MDIViewPage::saveDXF()
{
    QString fileName = Gui::FileDialog::getSaveFileName(Gui::getMainWindow(),
                                                   QString::fromUtf8(QT_TR_NOOP("Save DXF file")),
                                                   _getDefaultName(m_vpPage),
                                                   QString::fromUtf8(QT_TR_NOOP("DXF (*.dxf)")));
    if (fileName.isEmpty()) {
        return;
    }

    std::string sFileName = fileName.toUtf8().constData();
    saveDXF(sFileName);
}

void MDIViewPage::saveDXF(std::string fileName)
{
    TechDraw::DrawPage* page = m_vpPage->getDrawPage();
    std::string PageName = page->getNameInDocument();
    fileName = Base::Tools::escapeEncodeFilename(fileName);
    Gui::Command::openCommand(QT_TRANSLATE_NOOP("Command", "Save page to dxf"));
    Gui::Command::doCommand(Gui::Command::Doc, "import TechDraw");
    Gui::Command::doCommand(Gui::Command::Doc,
                            "TechDraw.writeDXFPage(App.activeDocument().%s, u\"%s\")",
                            PageName.c_str(), (const char*)fileName.c_str());
    Gui::Command::updateActive();
    Gui::Command::commitCommand();
}

void MDIViewPage::savePDF() { printPdf(); }

void MDIViewPage::savePDF(std::string file) { printPdf(file); }

//mdiviewpage method for printAll action
void MDIViewPage::printAll()
{
    //    Base::Console().Message("MDIVP::printAll()\n");
    printAllPages();
}

//static routine for PrintAll command
void MDIViewPage::printAllPages()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);

    QPrintDialog dlg(&printer, Gui::getMainWindow());
    if (dlg.exec() == QDialog::Accepted) {
        App::Document* doc = App::GetApplication().getActiveDocument();
        if (!doc) {
            return;
        }
        if (printer.outputFileName().isEmpty()) {
            printAll(&printer, doc);
        }
        else {
            printAllPdf(&printer, doc);
        }
    }
}

//flag to prevent selection activity within mdivp
void MDIViewPage::blockSceneSelection(const bool isBlocked)
{
    if (isBlocked)
        ++isSelectionBlocked;
    else if (isSelectionBlocked)
        --isSelectionBlocked;
}


//Set all QGIViews to unselected state
void MDIViewPage::clearSceneSelection()
{
    //    Base::Console().Message("MDIVP::clearSceneSelection()\n");
    blockSceneSelection(true);
    m_qgSceneSelected.clear();
    m_scene->clearSelection();
    blockSceneSelection(false);
}

QGIHighlight *MDIViewPage::findHighlight(const App::DocumentObject *obj)
{
    if (auto details = Base::freecad_dynamic_cast<const DrawViewDetail>(obj)) {
        if (auto baseView = m_scene->findQViewForDocObj(details->BaseView.getValue())) {
            for (auto child : baseView->childItems()) {
                if (auto highlight = qgraphicsitem_cast<QGIHighlight*>(child)) {
                    if (highlight->getFeature() == details)
                        return highlight;
                }
            }
        }

    }
    return nullptr;
}

QGISectionLine *MDIViewPage::findSection(const App::DocumentObject *obj)
{
    if (auto section = Base::freecad_dynamic_cast<const DrawViewSection>(obj)) {
        if (auto baseView = m_scene->findQViewForDocObj(section->BaseView.getValue())) {
            for (auto child : baseView->childItems()) {
                if (auto sectionLine = qgraphicsitem_cast<QGISectionLine*>(child)) {
                    if (sectionLine->getFeature() == section)
                        return sectionLine;
                }
            }
        }

    }
    return nullptr;
}

//!Update QGIView's selection state based on Selection made outside Drawing Interface
bool MDIViewPage::selectQGIView(App::DocumentObject* obj, const bool isSelected)
{

    if (!Base::freecad_dynamic_cast<DrawView>(obj))
        return false;

    blockSceneSelection(true);
    if (auto highlight = findHighlight(obj)) {
        highlight->setSelected(isSelected);
        highlight->update();
    }
    else if (auto section = findSection(obj)) {
        section->setSelected(isSelected);
        section->update();
    }

    QGIView* view = m_scene->findQViewForDocObj(obj);
    if (view) {
        view->setGroupSelection(isSelected);
        // view->updateView();
        view->update();
    }
    blockSceneSelection(false);
    return true;
}

bool MDIViewPage::selectTile(App::DocumentObject *obj, bool isSelected)
{
    if (!Base::freecad_dynamic_cast<DrawTile>(obj))
        return false;
    for (auto item : m_scene->items()) {
        if (auto tileItem = qgraphicsitem_cast<QGITile*>(item)){
            if (tileItem->getFeature() == obj) {
                tileItem->setSelected(isSelected);
                break;
            }
        }
    }
    return true;
}

QGIViewPart *MDIViewPage::findViewOfHatch(const App::DocumentObject *obj)
{
    DrawViewPart *view = nullptr;
    if (auto hatch = Base::freecad_dynamic_cast<DrawHatch>(obj))
        view = hatch->getSourceView();
    else if (auto hatch = Base::freecad_dynamic_cast<DrawGeomHatch>(obj))
        view = hatch->getSourceView();
    else
        return nullptr;
    return dynamic_cast<QGIViewPart*>(m_scene->findQViewForDocObj(view));
}

//! invoked by selection change made in Tree via father MDIView
//really "onTreeSelectionChanged"
void MDIViewPage::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    //    Base::Console().Message("MDIVP::onSelectionChanged()\n");
    if (msg.Type == Gui::SelectionChanges::ClrSelection) {
        clearSceneSelection();
    }
    else if (msg.Type == Gui::SelectionChanges::SetSelection) {//replace entire selection set
        clearSceneSelection();
        blockSceneSelection(true);
        std::vector<Gui::SelectionSingleton::SelObj> selObjs =
            Gui::Selection().getSelection(msg.pDocName);
        for (auto& so : selObjs) {
            if (selectQGIView(so.pObject, true))
                continue;
            else if (selectTile(so.pObject, true))
                continue;
        }
        blockSceneSelection(false);
    }
    else if (msg.Type == Gui::SelectionChanges::AddSelection
            || msg.Type == Gui::SelectionChanges::RmvSelection) {
        auto selObj = msg.Object.getObject();
        bool selected = msg.Type == Gui::SelectionChanges::AddSelection;
        if (auto view = findViewOfHatch(selObj)) {
            blockSceneSelection(true);
            for (auto face : view->getHatchedFaces(selObj)) {
                face->setSelected(selected);
            }
            blockSceneSelection(false);
        }
        else if (selectQGIView(selObj, selected))
            return;
        else if (selectTile(selObj, selected))
            return;
    }
    else if (msg.Type == Gui::SelectionChanges::SetPreselect
            || msg.Type == Gui::SelectionChanges::RmvPreselect) {
        auto selObj = msg.Object.getObject();
        auto resetHighlight = [this]{
            if (m_preselection) {
                m_preselection->setPreselect(false);
                m_preselection = nullptr;
                if (auto obj = m_preselectOther.getObject()) {
                    if (auto highlight = findHighlight(obj)) {
                        highlight->setPreselect(false);
                    }
                    else if (auto section = findSection(obj)) {
                        section->setPreselect(false);
                    }
                }
                m_preselectOther = App::DocumentObjectT();
            }
        };
        bool highlighted = msg.Type == Gui::SelectionChanges::SetPreselect;
        if (auto view = findViewOfHatch(selObj)) {
            for (auto face : view->getHatchedFaces(selObj)) {
                face->setPreselect(highlighted);
            }
        }
        else if (Base::freecad_dynamic_cast<TechDraw::DrawTile>(selObj)) {
            for (auto item : m_scene->items()) {
                if (auto tileItem = qgraphicsitem_cast<QGITile*>(item)){
                    if (tileItem->getFeature() == selObj) {
                        tileItem->setPreselect(highlighted);
                        break;
                    }
                }
            }
        }
        else if (auto drawView = Base::freecad_dynamic_cast<TechDraw::DrawView>(selObj)) {
            if (QGIView* view = m_scene->findQViewForDocObj(drawView)) {
                if (msg.Type == Gui::SelectionChanges::RmvPreselect || m_preselection != view) {
                    resetHighlight();
                    if (highlighted) {
                        m_preselection = view;
                        view->setPreselect(true);
                        if (auto highlight = findHighlight(drawView)) {
                            m_preselectOther = drawView;
                            highlight->setPreselect(true);
                        }
                        else if (auto section = findSection(drawView)) {
                            m_preselectOther = drawView;
                            section->setPreselect(true);
                        }
                    }
                    return;
                }
            }
        }
        resetHighlight();
    }
}

//! maintain QGScene selected items in selection order
void MDIViewPage::sceneSelectionManager()
{
    //    Base::Console().Message("MDIVP::sceneSelectionManager()\n");
    QList<QGraphicsItem*> sceneSel = m_scene->selectedItems();

    if (sceneSel.isEmpty()) {
        m_qgSceneSelected.clear();//TODO: need to signal somebody?  Tree? handled elsewhere
        //clearSelection
        return;
    }

    if (m_qgSceneSelected.isEmpty() && !sceneSel.isEmpty()) {
        m_qgSceneSelected.push_back(sceneSel.front());
        return;
    }

    //add to m_qgSceneSelected anything that is in q_sceneSel
    for (auto qts : sceneSel) {
        bool found = false;
        for (auto ms : qAsConst(m_qgSceneSelected)) {
            if (qts == ms) {
                found = true;
                break;
            }
        }
        if (!found) {
            m_qgSceneSelected.push_back(qts);
            break;
        }
    }

    //remove items from m_qgSceneSelected that are not in q_sceneSel
    QList<QGraphicsItem*> m_new;
    for (auto m : qAsConst(m_qgSceneSelected)) {
        for (auto q : sceneSel) {
            if (m == q) {
                m_new.push_back(m);
                break;
            }
        }
    }
    m_qgSceneSelected = m_new;
}

//! update Tree Selection from QGraphicsScene selection
//triggered by m_scene signal
void MDIViewPage::sceneSelectionChanged()
{
    sceneSelectionManager();

    if (isSelectionBlocked) {
        return;
    }

    std::vector<Gui::SelectionObject> treeSel = Gui::Selection().getSelectionEx();
    QList<QGraphicsItem*> sceneSel = m_qgSceneSelected;

    //check if really need to change selection
    bool sameSel = compareSelections(treeSel, sceneSel);
    if (sameSel) {
        return;
    }

    setTreeToSceneSelect();
}

//Note: Qt says: "no guarantee of selection order"!!!
void MDIViewPage::setTreeToSceneSelect()
{
    //    Base::Console().Message("MDIVP::setTreeToSceneSelect()\n");
    bool saveBlock = blockSelection(true);// block selectionChanged signal from Tree/Observer
    blockSceneSelection(true);
    Gui::Selection().clearSelection();
    QList<QGraphicsItem*> sceneSel = m_qgSceneSelected;

    auto findHighlightObject = [](QGraphicsItem *item) -> QGIHighlight* {
        if (item) {
            if (auto res = qgraphicsitem_cast<QGIHighlight*>(item))
                return res;

            if (auto res = qgraphicsitem_cast<QGIHighlight*>(item->parentItem()))
                return res;
        }
        return nullptr;
    };
    auto findSectionLine = [](QGraphicsItem *item) -> QGISectionLine* {
        if (item) {
            if (auto res = qgraphicsitem_cast<QGISectionLine*>(item))
                return res;

            if (auto res = qgraphicsitem_cast<QGISectionLine*>(item->parentItem()))
                return res;
        }
        return nullptr;
    };

    for (QList<QGraphicsItem*>::iterator it = sceneSel.begin(); it != sceneSel.end(); ++it) {
        if (auto itemView = dynamic_cast<QGIView*>(*it)) {
            TechDraw::DrawView* viewObj = itemView->getViewObject();
            if (viewObj && !viewObj->isRemoving()) {
                std::string doc_name = viewObj->getDocument()->getName();
                std::string obj_name = viewObj->getNameInDocument();

                Gui::Selection().addSelection(doc_name.c_str(), obj_name.c_str());
                showStatusMsg(doc_name.c_str(), obj_name.c_str(), "");
            }
        }
        else if (auto highlight = findHighlightObject(*it)) {
            Gui::Selection().addSelection(
                    highlight->getFeatureT().getDocumentName().c_str(),
                    highlight->getFeatureT().getObjectName().c_str());
        }
        else if (auto section = findSectionLine(*it)) {
            Gui::Selection().addSelection(
                    section->getFeatureT().getDocumentName().c_str(),
                    section->getFeatureT().getObjectName().c_str());
        }
        else if (auto deco = dynamic_cast<QGIDecoration*>(*it)) {
            Gui::Selection().addSelection(
                    deco->getFeatureT().getDocumentName().c_str(),
                    deco->getFeatureT().getObjectName().c_str());
        }
        else {
            if (QGIEdge* edge = qgraphicsitem_cast<QGIEdge*>(*it)) {
                QGraphicsItem* parent = edge->parentItem();
                if (!parent) {
                    continue;
                }

                QGIView* viewItem = dynamic_cast<QGIView*>(parent);
                if (!viewItem) {
                    continue;
                }

                TechDraw::DrawView* viewObj = viewItem->getViewObject();

                std::stringstream ss;
                ss << "Edge" << edge->getProjIndex();
                //bool accepted =
                static_cast<void>(Gui::Selection().addSelection(viewObj->getDocument()->getName(),
                                                                viewObj->getNameInDocument(),
                                                                ss.str().c_str()));
                showStatusMsg(viewObj->getDocument()->getName(), viewObj->getNameInDocument(),
                              ss.str().c_str());
                continue;
            }
            else if (QGIVertex* vert = qgraphicsitem_cast<QGIVertex*>(*it)) {
                QGraphicsItem* parent = vert->parentItem();
                if (!parent) {
                    continue;
                }

                QGIView* viewItem = dynamic_cast<QGIView*>(parent);
                if (!viewItem) {
                    continue;
                }

                TechDraw::DrawView* viewObj = viewItem->getViewObject();

                std::stringstream ss;
                ss << "Vertex" << vert->getProjIndex();
                //bool accepted =
                static_cast<void>(Gui::Selection().addSelection(viewObj->getDocument()->getName(),
                                                                viewObj->getNameInDocument(),
                                                                ss.str().c_str()));
                showStatusMsg(viewObj->getDocument()->getName(), viewObj->getNameInDocument(),
                              ss.str().c_str());
                continue;
            }
            else if (QGIFace* face = qgraphicsitem_cast<QGIFace*>(*it)) {
                QGraphicsItem* parent = face->parentItem();
                if (!parent) {
                    continue;
                }

                QGIView* viewItem = dynamic_cast<QGIView*>(parent);
                if (!viewItem) {
                    continue;
                }

                TechDraw::DrawView* viewObj = viewItem->getViewObject();

                std::stringstream ss;
                ss << "Face" << face->getProjIndex();
                //bool accepted =
                static_cast<void>(Gui::Selection().addSelection(viewObj->getDocument()->getName(),
                                                                viewObj->getNameInDocument(),
                                                                ss.str().c_str()));
                showStatusMsg(viewObj->getDocument()->getName(), viewObj->getNameInDocument(),
                              ss.str().c_str());
                continue;
            }
            else if (QGIDatumLabel* dimLabel = qgraphicsitem_cast<QGIDatumLabel*>(*it)) {
                QGraphicsItem* dimParent = dimLabel->QGraphicsItem::parentItem();
                if (!dimParent) {
                    continue;
                }

                QGIView* dimItem = dynamic_cast<QGIView*>(dimParent);

                if (!dimItem) {
                    continue;
                }

                TechDraw::DrawView* dimObj = dimItem->getViewObject();
                if (!dimObj) {
                    continue;
                }
                const char* name = dimObj->getNameInDocument();
                if (!name) {//can happen during undo/redo if Dim is selected???
                    //Base::Console().Log("INFO - MDIVP::sceneSelectionChanged - dimObj name is null!\n");
                    continue;
                }

                //bool accepted =
                static_cast<void>(Gui::Selection().addSelection(dimObj->getDocument()->getName(),
                                                                dimObj->getNameInDocument()));
            }
            else if (QGMText* mText = qgraphicsitem_cast<QGMText*>(*it)) {
                QGraphicsItem* textParent = mText->QGraphicsItem::parentItem();
                if (!textParent) {
                    continue;
                }

                QGIView* parent = dynamic_cast<QGIView*>(textParent);

                if (!parent) {
                    continue;
                }

                TechDraw::DrawView* parentFeat = parent->getViewObject();
                if (!parentFeat) {
                    continue;
                }
                const char* name = parentFeat->getNameInDocument();
                if (!name) {//can happen during undo/redo if Dim is selected???
                    continue;
                }

                //bool accepted =
                static_cast<void>(Gui::Selection().addSelection(
                    parentFeat->getDocument()->getName(), parentFeat->getNameInDocument()));
            }
        }
    }

    blockSceneSelection(false);
    blockSelection(saveBlock);
}

bool MDIViewPage::compareSelections(std::vector<Gui::SelectionObject> treeSel,
                                    QList<QGraphicsItem*> sceneSel)
{
    bool result = true;

    if (treeSel.empty() && sceneSel.empty()) {
        return true;
    }
    else if (treeSel.empty() && !sceneSel.empty()) {
        return false;
    }
    else if (!treeSel.empty() && sceneSel.empty()) {
        return false;
    }

    int treeCount = 0;
    int sceneCount = 0;
    int ppCount = 0;
    std::vector<std::string> treeNames;
    std::vector<std::string> sceneNames;

    for (auto tn : treeSel) {
        if (tn.getObject()->isDerivedFrom(TechDraw::DrawView::getClassTypeId())) {
            std::string s = tn.getObject()->getNameInDocument();
            treeNames.push_back(s);
        }
    }
    std::sort(treeNames.begin(), treeNames.end());
    treeCount = treeNames.size();

    for (auto sn : sceneSel) {
        if (auto itemView = dynamic_cast<QGIView*>(sn)) {
            sceneNames.push_back(itemView->getViewNameAsString());
        } else if (auto dl = qgraphicsitem_cast<QGIDatumLabel*>(sn)) {
            //get dim associated with this label
            if (auto vd = qgraphicsitem_cast<QGIViewDimension*>(dl->parentItem())) {
                sceneNames.push_back(vd->getViewNameAsString());
            }
        } else if (dynamic_cast<QGIPrimPath*>(sn)) {//count Vertex/Edge/Face
            ppCount++;
        }
    }
    std::sort(sceneNames.begin(), sceneNames.end());
    sceneCount = sceneNames.size();

    //different # of DrawView* vs QGIV*
    if (sceneCount != treeCount) {
        return false;
    }

    // even of counts match, have to check that names in scene == names in tree
    auto treePtr = treeNames.begin();
    for (auto& s : sceneNames) {
        if (s == (*treePtr)) {
            treePtr++;
            continue;
        }
        else {
            return false;
        }
    }

    //Objects all match, check subs
    if (treeCount != ppCount) {
        return false;
    }

    return result;
}

///////////////////end Selection Routines //////////////////////

void MDIViewPage::showStatusMsg(const char* string1, const char* string2, const char* string3) const
{
    QString msg = QStringLiteral("%1 %2.%3.%4 ")
            .arg(tr("Selected:"),
                 QString::fromUtf8(string1),
                 QString::fromUtf8(string2),
                 QString::fromUtf8(string3));
    if (Gui::getMainWindow()) {
        Gui::getMainWindow()->showMessage(msg, 6000);
    }
}

// ----------------------------------------------------------------------------

void MDIViewPagePy::init_type()
{
    behaviors().name("MDIViewPagePy");
    behaviors().doc("Python binding class for the MDI view page class");
    // you must have overwritten the virtual functions
    behaviors().supportRepr();
    behaviors().supportGetattr();
    behaviors().supportSetattr();

    add_varargs_method("getPage", &MDIViewPagePy::getPage,
                       "getPage() returns the page being displayed");
    add_varargs_method("cast_to_base", &MDIViewPagePy::cast_to_base,
                       "cast_to_base() cast to MDIView class");
    behaviors().readyType();
}

MDIViewPagePy::MDIViewPagePy(MDIViewPage *mdi)
  : base(mdi->getMDIViewPyObject())
{
}

MDIViewPagePy::~MDIViewPagePy()
{
    Py::_XDECREF(base);
}

Py::Object MDIViewPagePy::repr()
{
    std::ostringstream s_out;
    if (!getMDIViewPagePtr()) {
        throw Py::RuntimeError("Cannot print representation of deleted object");
    }
    s_out << "MDI view page";
    return Py::String(s_out.str());
}

// Since with PyCXX it's not possible to make a sub-class of MDIViewPy
// a trick is to use MDIViewPy as class member and override getattr() to
// join the attributes of both classes. This way all methods of MDIViewPy
// appear for SheetViewPy, too.
Py::Object MDIViewPagePy::getattr(const char* attrName)
{
    if (!getMDIViewPagePtr()) {
        std::ostringstream s_out;
        s_out << "Cannot access attribute '" << attrName << "' of deleted object";
        throw Py::RuntimeError(s_out.str());
    }
    std::string name(attrName);
    if (name == "__dict__" || name == "__class__") {
        Py::Dict dict_self(BaseType::getattr("__dict__"));
        Py::Dict dict_base(Py::asObject(base->_getattr("__dict__")));
        for (const auto& it : dict_base) {
            dict_self.setItem(it.first, it.second);
        }
        return dict_self;
    }

    try {
        return BaseType::getattr(attrName);
    }
    catch (Py::AttributeError& e) {
        e.clear();
        return Py::asObject(base->_getattr(attrName));
    }
}

MDIViewPage* MDIViewPagePy::getMDIViewPagePtr()
{
    return qobject_cast<MDIViewPage*>(base->getMDIViewPtr());
}

Py::Object MDIViewPagePy::getPage(const Py::Tuple& args)
{
    if (!PyArg_ParseTuple(args.ptr(), "")) {
        throw Py::Exception();
    }
    return Py::asObject(new TechDraw::DrawPagePy(getMDIViewPagePtr()->getPage()));
}

Py::Object MDIViewPagePy::cast_to_base(const Py::Tuple&)
{
    return Py::asObject(base);
}

#include <Mod/TechDraw/Gui/moc_MDIViewPage.cpp>
