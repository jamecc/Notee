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

#ifndef DAYWIDGET_H
#define DAYWIDGET_H

#include <QDate>
#include <QFont>
//#include <QLabel>
#include "HsWidget.h"
#include "ArrowWidget.h"

enum {
    TYPE_DAY = 0,
    TYPE_WEEK_HEADER,
    TYPE_MONTH_HEADER
};

class CNoteItemWidget;
class QVBoxLayout;
class QHBoxLayout;
class QGraphicsWidget;
class CNoteItem;
class CDayWidget : public CHsWidget
{
    Q_OBJECT
public:
    CDayWidget(int nType = TYPE_DAY);

    void setDate(const QDate& dt);
    const QDate& getDate() const;
    void setNoteItemsList(const QList<CNoteItem*>& lstNoteItems);
    void setNoteItemWidgetList(const QList<CNoteItemWidget*>& lstNoteItemWidgets);
    QList<CNoteItem*> getNoteItemsList() const;
    void clearAllNoteWidgets();

    void addWidget(QWidget* pWidget);
    void removeWidget(QWidget* pWidget);

    void setIsCurrentMonth(bool bIsCurrentMonth);

    void clearNoteSelectedStatus();


    void setDayOfWeek(int nDayOfWeek);
    bool isDay() const;
    bool isWeekHeader() const;
    bool isMonthHeader() const;

    CNoteItem* getSelectedNoteItem() const;
    CNoteItemWidget* getNoteItemWidget(CNoteItem* pNoteItem);

Q_SIGNALS:
    void clickNoteItem(CHsWidget* pNoteItem);
    void rightClickNoteItem(CHsWidget* pNoteItem);
    void dbClickNoteItem(CHsWidget* pNoteItem);
    void clickPrevious();
    void clickNext();

protected:
    void setType(int nType);

protected:
    virtual void paintEvent ( QPaintEvent * event );

private:
    QDate m_date;
    QList<CNoteItemWidget*> m_lstNoteItemWidgets;

//    QLabel* m_pLabelEmpty;

//    QVBoxLayout* m_pMainLayout;
    QHBoxLayout* m_pHLayout;
    CArrowWidget* m_pArrowPrevious;
    CArrowWidget* m_pArrowNext;

    QFont m_font;
    bool m_bIsCurrentMonth;
    int m_nType;
    int m_nDayOfWeek; // 1~7
};

#endif // DAYWIDGET_H
