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

#ifndef CALENDARVIEW_H
#define CALENDARVIEW_H

#include "GraphicsViewLayoutScene.h"

class CDatabaseManager;
class CCalendarScene;
class CNoteItem;

class CCalendarView : public CGraphicsViewLayoutScene
{
    Q_OBJECT
public:
    CCalendarView(CDatabaseManager* pDataManager = NULL, QWidget *parent = 0);

    void refreshDayWidget(const QDate& date);
    void refreshWeekHeader();
    void refreshAllDayWidgets();
    void clearAllNoteWidgets();

    CNoteItem* getSelectedNoteItem() const;
    QDate getSelectedDate() const;
    void selectNoteItem(CNoteItem* pNoteItem, bool bClearSelection = true);

    QList<CNoteItem*> getNoteItemList() const;

    bool isCurrentMonth() const;
    void setMonth(const QDate& dt, bool bGetNote = true);
    void prevMonth(bool bGetNote = true);
    void nextMonth(bool bGetNote = true);

Q_SIGNALS:
    void newNote(const QDate& date);
    void editNote(CNoteItem* pNoteItem);
    void newNote();
    void editNote();
    void delNote();
    void itemSelectionChanged();
    void noteItemSelectionChanged();

public Q_SLOTS:
    void onItemSelectionChanged();

private:
    bool m_bSender;
    CCalendarScene* m_pCalendarScene;
};

#endif // CALENDARVIEW_H
