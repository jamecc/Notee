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

#ifndef CALENDARSCENE_H
#define CALENDARSCENE_H

#include <QGraphicsScene>
#include <QDate>

class CNoteItem;
class CDatabaseManager;
class CDayWidget;
class CHsWidget;
class CNoteItemWidget;
class CCalendarScene : public QGraphicsScene
{
	Q_OBJECT
public:
	CCalendarScene(QObject *parent = 0, CDatabaseManager* pDataManager = NULL);
	virtual ~CCalendarScene(void);

    void init();

    void setMonth(const QDate& dt, bool bGetNote = true);

    void refreshWeekHeader();
    bool isCurrentMonth() const;

    CDayWidget* getDayWidget(const QDate& date) const;
    void refreshDayWidget(const QDate& date);
    void refreshAllDayWidgets();
    void clearAllNoteWidgets();
    QList<CNoteItem*> getNoteItemList() const;

    CNoteItemWidget* getNoteItemWidget(CNoteItem* pNoteItem);
    void selectNoteItem(CNoteItem* pNoteItemWidget, bool bClear = true);
    void selectNoteItem(CNoteItemWidget* pNoteItemWidget, bool bClear = true);

    CNoteItem* getSelectedNoteItem() const;
    QDate getSelectedDate() const;
    QDate getCurrentMonth() const;

    void prevMonth(bool bGetNote = true);
    void nextMonth(bool bGetNote = true);

Q_SIGNALS:
    void newNote(const QDate& date);
    void editNote(CNoteItem* pNoteItem);
    void newNote();
    void editNote();
    void delNote();
    void itemSelectionChanged();
    void clickPrevious();
    void clickNext();

protected Q_SLOTS:
	void OnSelectionChanged();
    void onSceneRectChanged(const QRectF& rect);
    void onClickWidget(CHsWidget* pWidget);
    void onRightClickWidget(CHsWidget* pWidget);
    void onDbClickWidget(CHsWidget* pWidget);

protected:


private:
	bool m_bDestroying;

    QGraphicsWidget* m_pMainWidget;
    CDayWidget* m_pMonthHeaderWidget;
    QList<CDayWidget*> m_lstWeekHeaderWidgets;
    QList<CDayWidget*> m_lstDayWidgets;

    QDate m_dtMonth;

    CDatabaseManager* m_pDataManager;
};

#endif // #ifndef CALENDARSCENE_H
