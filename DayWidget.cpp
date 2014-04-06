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

#include <QPainter>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsWidget>
#include "CommonFunctions.h"
#include "NoteItem.h"
#include "NoteItemWidget.h"
#include "MainFrame.h"
#include "predefine.h"
#include "DayWidget.h"

#define DAY_MARGIN 5

#include <QLabel>

CDayWidget::CDayWidget(int nType)
{
    setMinimumSize(QSize(32,60));
 //   setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_pHLayout = NULL;
    //m_pLabelEmpty = NULL;

    m_date = QDate::currentDate();
    setType(nType);
}

void CDayWidget::setDate(const QDate& dt)
{
    m_date = dt;

    if(TYPE_DAY == m_nType)
    {
        QColor clrBk;
        if(m_date.dayOfWeek() > 5)
        {
            clrBk = QColor(CLR_BKGND_WEEK_END);
        }
        else
        {
            clrBk = QColor(CLR_BKGND_DAY);
        }

        QPalette p(palette());
        p.setColor(QPalette::Background, clrBk);
        setPalette(p);
    }

    repaint();
}

const QDate& CDayWidget::getDate() const
{
    return m_date;
}

void CDayWidget::setNoteItemsList(const QList<CNoteItem*>& lstNoteItems)
{
    CNoteItemWidget* pNoteItemWidget = NULL;
    QList<CNoteItemWidget*> lstNoteItemWidgets;
    foreach(CNoteItem* pNoteItem, lstNoteItems)
    {
        if(STATE_DELETE == pNoteItem->getState())
            continue;

        pNoteItemWidget = new CNoteItemWidget(this);
        pNoteItemWidget->setNoteItem(pNoteItem);
        lstNoteItemWidgets.push_back(pNoteItemWidget);
    }

    setNoteItemWidgetList(lstNoteItemWidgets);
}

void CDayWidget::setNoteItemWidgetList(const QList<CNoteItemWidget*>& lstNoteItemWidgets)
{
    bool bNeedRefresh = false;
    foreach(CNoteItemWidget* pWidget, m_lstNoteItemWidgets)
    {
       removeWidget(pWidget);
       bNeedRefresh = true;
    }
    m_lstNoteItemWidgets.clear();

    m_lstNoteItemWidgets = lstNoteItemWidgets;
    foreach(CNoteItemWidget* pWidget, m_lstNoteItemWidgets)
    {
       addWidget(qobject_cast<QWidget*>(pWidget));
       bNeedRefresh = true;
    }

    if(bNeedRefresh)
        update();
}

QList<CNoteItem*> CDayWidget::getNoteItemsList() const
{
    QList<CNoteItem*> lstNoteItems;
    foreach(CNoteItemWidget* pWidget, m_lstNoteItemWidgets)
    {
        if(pWidget->getNoteItem())
        {
            lstNoteItems.push_back(pWidget->getNoteItem());
        }
    }

    return lstNoteItems;
}

void CDayWidget::clearAllNoteWidgets()
{
    foreach(CNoteItemWidget* pWidget, m_lstNoteItemWidgets)
    {
        pWidget->hide();
        delete pWidget, pWidget = NULL;
    }
    m_lstNoteItemWidgets.clear();
}

void CDayWidget::paintEvent( QPaintEvent * event)
{
    CHsWidget::paintEvent(event);

    QRect rcLeft = rect();
    QPainter painter(this);
    painter.save();
    painter.setRenderHints(painter.renderHints() |
                           QPainter::Antialiasing |
                           QPainter::TextAntialiasing |
                           QPainter::HighQualityAntialiasing);

    QFont ftPainter = font();
    ftPainter.setFamily("Calibri");

    switch(m_nType)
    {
    case TYPE_DAY:
    {
        ftPainter.setPointSize(12);

        bool bIsCurrentDay = (QDate::currentDate() == m_date);
        if(bIsCurrentDay)
        {
            painter.setPen(QPen(QColor(CLR_FONT_DAY_CURRENT_DAY)));
            ftPainter.setBold(true);
        }
        else if(m_bIsCurrentMonth)
            painter.setPen(QPen(QColor(CLR_FONT_DAY_CURRENT_MONTH)));
        else
            painter.setPen(QPen(QColor(CLR_FONT_DAY)));

        painter.setFont(ftPainter);
        // bounding rectangle
  //      painter.fillRect(rcLeft, QBrush(QColor(112,146,190)));
        rcLeft.adjust(DAY_MARGIN, DAY_MARGIN, -DAY_MARGIN * 2, -DAY_MARGIN * 2);

        QString strDay;

        if(G_Settings()->isShowLunar())
        {
            strDay = QString("%1 %2").arg(m_date.day())
                    .arg(CLunDay::GetLunarCalendar(m_date, CLunDay::LUNDAY_DAY));
        }
        else
        {
            strDay = QString::number(m_date.day());
        }

        // text
        painter.drawText(rcLeft.left() + 5, rcLeft.top() + 25,
                          strDay);
        break;
    }
    case TYPE_WEEK_HEADER:
    {
        ftPainter.setPointSize(14);
        painter.setFont(ftPainter);
        painter.setPen(QPen(QColor(CLR_FONT_WEEK)));

        // bounding rectangle
  //      painter.fillRect(rcLeft, QBrush(QColor(200,191,231)));
 //       rcLeft.adjust(DAY_MARGIN, DAY_MARGIN, -DAY_MARGIN * 2, -DAY_MARGIN * 2);

        QString strWeekDay;
        switch(m_nDayOfWeek)
        {
        case 1:
            strWeekDay = tr("Monday");
            break;
        case 2:
            strWeekDay = tr("Tuesday");
            break;
        case 3:
            strWeekDay = tr("Wednesday");
            break;
        case 4:
            strWeekDay = tr("Thursday");
            break;
        case 5:
            strWeekDay = tr("Friday");
            break;
        case 6:
            strWeekDay = tr("Saturday");
            break;
        case 7:
            strWeekDay = tr("Sunday");
            break;
        default:
            break;
        }

        // text
        painter.drawText(rcLeft, Qt::AlignCenter, strWeekDay);
        break;
    }
    case TYPE_MONTH_HEADER:
    {
        ftPainter.setPointSize(16);
        painter.setFont(ftPainter);
        painter.setPen(QPen(QColor(CLR_FONT_MONTH)));
  //      painter.setBrush(QBrush(QColor(255,255,255)));

        // bounding rectangle
  //      painter.fillRect(rcLeft, QBrush(QColor(239,228,176)));
  //      rcLeft.adjust(DAY_MARGIN, DAY_MARGIN, -DAY_MARGIN * 2, -DAY_MARGIN * 2);

        QString strMonth;

        if(G_Settings()->isShowLunar())
        {
            strMonth = QString("%1 %2 %3 %4")
                            .arg(G_getMonthString(m_date.month()))
                            .arg(m_date.year())
                            .arg(CLunDay::GetLunarCalendar(m_date, CLunDay::LUNDAY_YEAR))
                            .arg(CLunDay::GetLunarCalendar(m_date, CLunDay::LUNDAY_MONTH));
        }
        else
        {
            strMonth = QString("%1 %2")
                            .arg(G_getMonthString(m_date.month()))
                            .arg(m_date.year());
        }
        // text
        painter.drawText(rcLeft, Qt::AlignCenter, strMonth);
        break;
    }
    default:
        break;
    }

    painter.restore();
    painter.end();
}

void CDayWidget::addWidget(QWidget* pWidget)
{
    if(m_pHLayout->indexOf(pWidget) < 0)
    {
 //       pWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_pHLayout->addWidget(pWidget);

        CNoteItemWidget* pNoteItem = qobject_cast<CNoteItemWidget*>(pWidget);
        if(pNoteItem)
        {
            connect(pNoteItem, SIGNAL(click(CHsWidget*)), this, SIGNAL(clickNoteItem(CHsWidget*)));
            connect(pNoteItem, SIGNAL(emitContexMenu(CHsWidget*)), this, SIGNAL(rightClickNoteItem(CHsWidget*)));
            connect(pNoteItem, SIGNAL(dbClick(CHsWidget*)), this, SIGNAL(dbClickNoteItem(CHsWidget*)));
        }
    }
}

void CDayWidget::removeWidget(QWidget* pWidget)
{
    pWidget->hide();
    m_pHLayout->removeWidget(pWidget);
    pWidget->deleteLater(), pWidget = NULL;
}

void CDayWidget::setIsCurrentMonth(bool bIsCurrentMonth)
{
    m_bIsCurrentMonth = bIsCurrentMonth;
}

void CDayWidget::clearNoteSelectedStatus()
{
    foreach(CNoteItemWidget* pItem, m_lstNoteItemWidgets)
    {
        pItem->setSelected(false);
    }
}

void CDayWidget::setType(int nType)
{
    m_nType = nType;

    QVBoxLayout* pMainLayout = new QVBoxLayout(this);
    pMainLayout->setSpacing(0);
    pMainLayout->setMargin(0);

    QColor clr;
    switch(m_nType)
    {
    case TYPE_WEEK_HEADER:
        clr = QColor(CLR_BKGND_HEADER_WEEK);
        break;
    case TYPE_MONTH_HEADER:
    {
        m_pHLayout = new QHBoxLayout;
        pMainLayout->addLayout(m_pHLayout);

  //      m_pLabelEmpty = new QLabel(this);
     //   pLabel->setText("test");
  //      m_pLabelEmpty->setMaximumSize(QSize(1,100));
     //   pLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
 //       m_pHLayout->addWidget(m_pLabelEmpty);

        m_pArrowPrevious = new CArrowWidget(this);
        m_pArrowNext = new CArrowWidget(this, CArrowWidget::TYPE_NEXT);
        m_pHLayout->addWidget(m_pArrowPrevious);
        m_pHLayout->addWidget(m_pArrowNext);
        clr = QColor(CLR_BKGND_HEADER_MONTH);

        connect(m_pArrowPrevious, SIGNAL(click(CHsWidget*)), this, SIGNAL(clickPrevious()));
        connect(m_pArrowNext, SIGNAL(click(CHsWidget*)), this, SIGNAL(clickNext()));
        break;
    }
    case TYPE_DAY:
    {
            QSpacerItem* pVerticalSpacerItem = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding);
            pMainLayout->addItem(pVerticalSpacerItem);

           m_pHLayout = new QHBoxLayout;
           pMainLayout->addLayout(m_pHLayout);
           pMainLayout->setSpacing(0);
           pMainLayout->setMargin(0);
     //      pMainLayout->setContentsMargins(0, 0, 0, 5);

 //          m_pLabelEmpty = new QLabel(this);
        //   pLabel->setText("test");
  //         m_pLabelEmpty->setMaximumSize(QSize(1,100));
        //   pLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
 //          m_pHLayout->addWidget(m_pLabelEmpty);
    }
    default:
        clr = QColor(CLR_BKGND_DAY);
        break;
    }

    QPalette p(palette());
    p.setColor(QPalette::Background, clr);
    setPalette(p);
}

void CDayWidget::setDayOfWeek(int nDayOfWeek)
{
    m_nDayOfWeek = nDayOfWeek;

    QColor clr;
    if(m_nDayOfWeek > 5)
        clr = QColor(CLR_BKGND_HEADER_WEEK_END);
    else
        clr = QColor(CLR_BKGND_HEADER_WEEK);

    QPalette p(palette());
    p.setColor(QPalette::Background, clr);
    setPalette(p);
}

bool CDayWidget::isDay() const
{
    return m_nType == TYPE_DAY;
}

bool CDayWidget::isWeekHeader() const
{
    return m_nType == TYPE_WEEK_HEADER;
}

bool CDayWidget::isMonthHeader() const
{
    return m_nType == TYPE_MONTH_HEADER;
}

CNoteItem* CDayWidget::getSelectedNoteItem() const
{
    foreach(CNoteItemWidget* pWidget, m_lstNoteItemWidgets)
    {
        if(pWidget->isSelected())
            return pWidget->getNoteItem();
    }

    return NULL;
}

CNoteItemWidget* CDayWidget::getNoteItemWidget(CNoteItem* pNoteItem)
{
    foreach(CNoteItemWidget* pWidget, m_lstNoteItemWidgets)
    {
        if(pWidget->getNoteItem() == pNoteItem)
            return pWidget;
    }

    return NULL;
}
