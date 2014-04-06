/****************************************************************************
**
** Copyright (C) 2012 Coode Software
** Contact: http://www.coodesoft.com/
**
** This file is part of Notee.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
****************************************************************************/

#include "stdafx.h"
#include "NoteItem.h"

#include "DatabaseManager.h"

#include "DayWidget.h"
#include "NoteItemWidget.h"
#include "CalendarScene.h"
#include "MainFrame.h"

CCalendarScene::CCalendarScene(QObject *parent/* = 0*/, CDatabaseManager* pDataManager)
    : QGraphicsScene(parent)
    , m_bDestroying(false)
    , m_pDataManager(pDataManager)
{
//    m_pMLayoutItem = new CMLayoutItem();
//    m_pMLayoutItem->SetLayoutMode(LAYOUT_VERTICAL);

    // left and calendar
//    CMLayoutItem* pLayoutItemTmp = new CMLayoutItem;
//    pLayoutItemTmp->SetLayoutMode(LAYOUT_HORIZONTAL);
//    m_pCalendar = new CMCalendar(this, pDataManager);
//    m_pCalendar->AddToScene(this);
//    pLayoutItemTmp->AddItem(m_pCalendar);

//    m_pMLayoutItem->AddItem(pLayoutItemTmp);

    // connect
    connect(this, SIGNAL(selectionChanged()), this, SLOT(OnSelectionChanged()));
    connect(this, SIGNAL(sceneRectChanged(const QRectF &)), this, SLOT(onSceneRectChanged(const QRectF &)));

    init();

    setBackgroundBrush(QBrush(QColor(COLOR_BKGND_SCENE)));
}

CCalendarScene::~CCalendarScene(void)
{
    m_bDestroying = true;

    delete m_pMonthHeaderWidget;

    foreach(CDayWidget* pItem, m_lstWeekHeaderWidgets)
    {
        delete pItem;
    }
    m_lstWeekHeaderWidgets.clear();

    foreach(CDayWidget* pItem, m_lstDayWidgets)
    {
        delete pItem;
    }
    m_lstDayWidgets.clear();
}

void CCalendarScene::init()
{
    QGraphicsWidget* pGraphicsWidget = NULL;
    CDayWidget* pDayWidget = NULL;
    int nRowIdx = 0;

    // main widget
    m_pMainWidget = new QGraphicsWidget;
    addItem(m_pMainWidget);

    m_pMainWidget->setContentsMargins(0,0,0,0);

    // layout
    QGraphicsGridLayout *layout = new QGraphicsGridLayout;
    m_pMainWidget->setLayout(layout);
    layout->setSpacing(2.0);
    layout->setContentsMargins(0,0,0,0);

    // month header
    m_pMonthHeaderWidget = new CDayWidget(TYPE_MONTH_HEADER);
    pGraphicsWidget = addWidget(m_pMonthHeaderWidget);
    m_pMonthHeaderWidget->setGraphicsWidget(pGraphicsWidget);
    layout->addItem(pGraphicsWidget, nRowIdx++, 0, 1, 7);
    connect(m_pMonthHeaderWidget, SIGNAL(click(CHsWidget*)), this, SLOT(onClickWidget(CHsWidget*)));
    connect(m_pMonthHeaderWidget, SIGNAL(dbClick(CHsWidget*)), this, SLOT(onDbClickWidget(CHsWidget*)));
    connect(m_pMonthHeaderWidget, SIGNAL(clickPrevious()), this, SIGNAL(clickPrevious()));
    connect(m_pMonthHeaderWidget, SIGNAL(clickNext()), this, SIGNAL(clickNext()));

    // week header
    for(int i = 0; i < 7; ++i)
    {
        // add widget
        pDayWidget = new CDayWidget(TYPE_WEEK_HEADER);
        pGraphicsWidget = addWidget(pDayWidget);
        pDayWidget->setGraphicsWidget(pGraphicsWidget);
        layout->addItem(pGraphicsWidget, nRowIdx, i % 7);
        m_lstWeekHeaderWidgets.push_back(pDayWidget);

        connect(pDayWidget, SIGNAL(click(CHsWidget*)), this, SLOT(onClickWidget(CHsWidget*)));
        connect(pDayWidget, SIGNAL(dbClick(CHsWidget*)), this, SLOT(onDbClickWidget(CHsWidget*)));
    }

    refreshWeekHeader();

    nRowIdx++;

    // add days
    for(int i = 0; i < 42; ++i)
    {
        // add widget
        pDayWidget = new CDayWidget;
        pGraphicsWidget = addWidget(pDayWidget);
        pDayWidget->setGraphicsWidget(pGraphicsWidget);
        pDayWidget->setSelectable(true);
        pDayWidget->setHoverable(true);
        pDayWidget->setPressable(true);
        layout->addItem(pGraphicsWidget, nRowIdx, i % 7);
        m_lstDayWidgets.push_back(pDayWidget);
        connect(pDayWidget, SIGNAL(click(CHsWidget*)), this, SLOT(onClickWidget(CHsWidget*)));
        connect(pDayWidget, SIGNAL(dbClick(CHsWidget*)), this, SLOT(onDbClickWidget(CHsWidget*)));
        connect(pDayWidget, SIGNAL(emitContexMenu(CHsWidget*)), this, SLOT(onRightClickWidget(CHsWidget*)));
        connect(pDayWidget, SIGNAL(clickNoteItem(CHsWidget*)), this, SLOT(onClickWidget(CHsWidget*)));
        connect(pDayWidget, SIGNAL(rightClickNoteItem(CHsWidget*)), this, SLOT(onRightClickWidget(CHsWidget*)));
        connect(pDayWidget, SIGNAL(dbClickNoteItem(CHsWidget*)), this, SLOT(onDbClickWidget(CHsWidget*)));

        connect(pDayWidget, SIGNAL(click(CHsWidget*)), this, SIGNAL(itemSelectionChanged()));
        connect(pDayWidget, SIGNAL(emitContexMenu(CHsWidget*)), this, SIGNAL(itemSelectionChanged()));
        connect(pDayWidget, SIGNAL(dbClick(CHsWidget*)), this, SIGNAL(itemSelectionChanged()));
        connect(pDayWidget, SIGNAL(clickNoteItem(CHsWidget*)), this, SIGNAL(itemSelectionChanged()));
        connect(pDayWidget, SIGNAL(rightClickNoteItem(CHsWidget*)), this, SIGNAL(itemSelectionChanged()));
        connect(pDayWidget, SIGNAL(dbClickNoteItem(CHsWidget*)), this, SIGNAL(itemSelectionChanged()));

        // row++
        if((i % 7) == 6) {
            nRowIdx++;
        }
    }

    // set current date
    setMonth(QDate::currentDate(), false);
}

void CCalendarScene::setMonth(const QDate& dt, bool bGetNote)
{
    m_dtMonth = dt;

    // month header
    m_pMonthHeaderWidget->setDate(m_dtMonth);

    // get first day
    QDate dtFirstDayOfMonth(m_dtMonth.year(), m_dtMonth.month(), 1);
    int nFirstDayOfWeek = dtFirstDayOfMonth.dayOfWeek() - G_Settings()->getFirstDayOfWeek();
    if(nFirstDayOfWeek < 0)
        nFirstDayOfWeek += 7;

    dtFirstDayOfMonth = dtFirstDayOfMonth.addDays(-nFirstDayOfWeek);

    // set day widget
    CDayWidget* pDayWidget = NULL;
    QList<CNoteItem*> lstNoteItems;
    for(int i = 0; i < 42; ++i)
    {
        // set date
        pDayWidget = m_lstDayWidgets.at(i);
        pDayWidget->setDate(dtFirstDayOfMonth);

        pDayWidget->setIsCurrentMonth(m_dtMonth.month() == dtFirstDayOfMonth.month());

        // get note
        if(bGetNote)
        {
            lstNoteItems.clear();
            m_pDataManager->GetNoteItems(dtFirstDayOfMonth, lstNoteItems);
            pDayWidget->setNoteItemsList(lstNoteItems);
        }

        // ++
        dtFirstDayOfMonth = dtFirstDayOfMonth.addDays(1);
    }
}

void CCalendarScene::refreshWeekHeader()
{
    int nTmp = 0, i = -1;
    foreach(CDayWidget* pDayWidget, m_lstWeekHeaderWidgets)
    {
        nTmp = (i + G_Settings()->getFirstDayOfWeek()) % 7 + 1;
        pDayWidget->setDayOfWeek(nTmp);
        i++;
    }
}

bool CCalendarScene::isCurrentMonth() const
{
    return m_dtMonth.year() == QDate::currentDate().year() &&
            m_dtMonth.month() == QDate::currentDate().month();
}

void CCalendarScene::OnSelectionChanged()
{
    if(m_bDestroying)
            return;
}

void CCalendarScene::onSceneRectChanged(const QRectF& rect)
{
//    Layout(rect.toRect());
    m_pMainWidget->resize(rect.width(), rect.height());
}

void CCalendarScene::onClickWidget(CHsWidget* pWidget)
{
    CDayWidget* pDayWidget = qobject_cast<CDayWidget*>(pWidget);
    if(pDayWidget)
    {
        // select current widget
        if(!pDayWidget->isSelected())
        {
            foreach(CDayWidget* pWidget, m_lstDayWidgets)
            {
                pWidget->setSelected(false);
            }

            pDayWidget->setSelected(true);
        }
    }
    else
    {
        CNoteItemWidget* pNoteItemWidget = qobject_cast<CNoteItemWidget*>(pWidget);
        if(pNoteItemWidget)
        {
            selectNoteItem(pNoteItemWidget);
        }
    }
}

void CCalendarScene::onRightClickWidget(CHsWidget* pWidget)
{
    onClickWidget(pWidget);

    QList<QGraphicsView *> lstGrpViews = views();
    if(!lstGrpViews.length())
        return;

    // right-click menu
    QMenu menu(lstGrpViews.at(0));

    bool bDlgNoteEditing = G_MainFrame()->isDlgNoteEditing();

    // new
    QAction* pActionNew = menu.addAction(tr("New..."), this, SIGNAL(newNote()));
    QIcon icon1(QString::fromUtf8(":/res/calendar.png"));
    pActionNew->setIcon(icon1);
    // edit
    QAction* pActionEdit = menu.addAction(tr("Edit..."), this, SIGNAL(editNote()));
    QIcon icon2(QString::fromUtf8(":/res/edit.png"));
    pActionEdit->setIcon(icon2);
    // delete
    QAction* pActionDelete = menu.addAction(tr("Delete"), this, SIGNAL(delNote()));
    QIcon icon3(QString::fromUtf8(":/res/delete.png"));
    pActionDelete->setIcon(icon3);
    pActionEdit->setEnabled(getSelectedNoteItem());
    pActionDelete->setEnabled(getSelectedNoteItem());

    if(bDlgNoteEditing)
    {
        pActionNew->setEnabled(false);
        pActionEdit->setEnabled(false);
        pActionDelete->setEnabled(false);
    }

    menu.exec(QCursor::pos());
}

void CCalendarScene::onDbClickWidget(CHsWidget* pWidget)
{
    CDayWidget* pDayWidget = qobject_cast<CDayWidget*>(pWidget);
    if(pDayWidget)
    {
        // select current widget
        if(!pDayWidget->isSelected())
        {
            foreach(CDayWidget* pWidget, m_lstDayWidgets)
            {
                pWidget->setSelected(false);
            }

            pDayWidget->setSelected(true);
        }

        if(pDayWidget->isDay())
            emit newNote(pDayWidget->getDate());
    }
    else
    {
        CNoteItemWidget* pNoteItemWidget = qobject_cast<CNoteItemWidget*>(pWidget);
        if(pNoteItemWidget)
        {
            selectNoteItem(pNoteItemWidget);

            emit editNote(pNoteItemWidget->getNoteItem());
        }
    }
}

CNoteItem* CCalendarScene::getSelectedNoteItem() const
{
    CNoteItem* pNoteItem = NULL;
    foreach(CDayWidget* pWidget, m_lstDayWidgets)
    {
        pNoteItem = pWidget->getSelectedNoteItem();
        if(pNoteItem)
            return pNoteItem;
    }

    return NULL;
}

QDate CCalendarScene::getSelectedDate() const
{
    foreach(CDayWidget* pWidget, m_lstDayWidgets)
    {
        if(pWidget->isSelected())
            return pWidget->getDate();
    }

    return QDate();
}

QDate CCalendarScene::getCurrentMonth() const
{
    return m_dtMonth;
}

void CCalendarScene::prevMonth(bool bGetNote)
{
    setMonth(m_dtMonth.addMonths(-1), bGetNote);
}

void CCalendarScene::nextMonth(bool bGetNote)
{
    setMonth(m_dtMonth.addMonths(1), bGetNote);
}

CDayWidget* CCalendarScene::getDayWidget(const QDate& date) const
{
    foreach(CDayWidget* pWidget, m_lstDayWidgets)
    {
        if(pWidget->getDate() == date)
        {
            return pWidget;
        }
    }

    return NULL;
}

void CCalendarScene::refreshDayWidget(const QDate& date)
{
    CDayWidget* pDayWidget = getDayWidget(date);
    if(pDayWidget)
    {
        QList<CNoteItem*> lstNoteItems;
        m_pDataManager->GetNoteItems(date, lstNoteItems);

        pDayWidget->setNoteItemsList(lstNoteItems);
    }
}

void CCalendarScene::refreshAllDayWidgets()
{
    setMonth(m_dtMonth);
}

void CCalendarScene::clearAllNoteWidgets()
{
    foreach(CDayWidget* pWidget, m_lstDayWidgets)
    {
        pWidget->clearAllNoteWidgets();
    }
}

QList<CNoteItem*> CCalendarScene::getNoteItemList() const
{
    QList<CNoteItem*> lstNoteItems;
    QList<CNoteItem*> lstNoteItems2;
    foreach(CDayWidget* pWidget, m_lstDayWidgets)
    {
        lstNoteItems2 = pWidget->getNoteItemsList();
        foreach(CNoteItem* pNoteItem, lstNoteItems2)
        {
            lstNoteItems.push_front(pNoteItem);
        }
    }

    return lstNoteItems;
}

CNoteItemWidget* CCalendarScene::getNoteItemWidget(CNoteItem* pNoteItem)
{
    CNoteItemWidget* pNoteItemWidget = NULL;
    foreach(CDayWidget* pWidget, m_lstDayWidgets)
    {
        pNoteItemWidget = pWidget->getNoteItemWidget(pNoteItem);
        if(pNoteItemWidget)
            return pNoteItemWidget;
    }

    return NULL;
}

void CCalendarScene::selectNoteItem(CNoteItem* pNoteItem, bool bClear)
{
    CNoteItemWidget* pNoteItemWidget = getNoteItemWidget(pNoteItem);
    selectNoteItem(pNoteItemWidget, bClear);
}

void CCalendarScene::selectNoteItem(CNoteItemWidget* pNoteItemWidget, bool bClear)
{
    if(bClear)
    {
        foreach(CDayWidget* pWidget, m_lstDayWidgets)
        {
            pWidget->clearNoteSelectedStatus();
        }
    }

    if(pNoteItemWidget)
        pNoteItemWidget->setSelected(true);
}


