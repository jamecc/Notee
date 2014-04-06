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

#include "CalendarScene.h"
#include "NoteItemWidget.h"
#include "CalendarView.h"

CCalendarView::CCalendarView(CDatabaseManager* pDataManager, QWidget *parent)
    : CGraphicsViewLayoutScene(parent)
{
    m_bSender = false;
    m_pCalendarScene = new CCalendarScene(this, pDataManager);
    setScene(m_pCalendarScene);

    connect(m_pCalendarScene, SIGNAL(newNote(QDate)), this, SIGNAL(newNote(QDate)));
    connect(m_pCalendarScene, SIGNAL(editNote(CNoteItem*)), this, SIGNAL(editNote(CNoteItem*)));
    connect(m_pCalendarScene, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));

    connect(m_pCalendarScene, SIGNAL(newNote()), this, SIGNAL(newNote()));
    connect(m_pCalendarScene, SIGNAL(editNote()), this, SIGNAL(editNote()));
    connect(m_pCalendarScene, SIGNAL(delNote()), this, SIGNAL(delNote()));
}

void CCalendarView::refreshDayWidget(const QDate& date)
{
    m_pCalendarScene->refreshDayWidget(date);
}

void CCalendarView::refreshWeekHeader()
{
    m_pCalendarScene->refreshWeekHeader();
}

void CCalendarView::refreshAllDayWidgets()
{
    m_pCalendarScene->refreshAllDayWidgets();
}

void CCalendarView::clearAllNoteWidgets()
{
    m_pCalendarScene->clearAllNoteWidgets();
}

CNoteItem* CCalendarView::getSelectedNoteItem() const
{
    return m_pCalendarScene->getSelectedNoteItem();
}

QDate CCalendarView::getSelectedDate() const
{
    return m_pCalendarScene->getSelectedDate();
}

void CCalendarView::selectNoteItem(CNoteItem* pNoteItem, bool bClearSelection)
{
    m_bSender = true;
    m_pCalendarScene->selectNoteItem(pNoteItem, bClearSelection);
    m_bSender = false;
}

void CCalendarView::onItemSelectionChanged()
{
    if(m_bSender)
        return;

    CNoteItemWidget* pNoteItemWidget = qobject_cast<CNoteItemWidget*>(sender());
    if(pNoteItemWidget)
    {
        emit noteItemSelectionChanged();
    }

    emit itemSelectionChanged();
}

QList<CNoteItem*> CCalendarView::getNoteItemList() const
{
    return m_pCalendarScene->getNoteItemList();
}

bool CCalendarView::isCurrentMonth() const
{
    return m_pCalendarScene->isCurrentMonth();
}

void CCalendarView::setMonth(const QDate& dt, bool bGetNote)
{
    m_pCalendarScene->setMonth(dt, bGetNote);
}

void CCalendarView::prevMonth(bool bGetNote)
{
    m_pCalendarScene->prevMonth(bGetNote);
}

void CCalendarView::nextMonth(bool bGetNote)
{
    m_pCalendarScene->nextMonth(bGetNote);
}
