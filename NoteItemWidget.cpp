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
#include "predefine.h"
#include "NoteItem.h"
#include "NoteItemWidget.h"

CNoteItemWidget::CNoteItemWidget(QWidget *parent)
    : CHsWidget(parent)
{
    setMinimumSize(QSize(10, 10));
    setMaximumSize(QSize(100, 16));
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    /*
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    setSizePolicy(sizePolicy);
    setMaximumSize(QSize(16, 16));
    */

    setHoverable(true);
    setPressable(true);
    setSelectable(true);

    setColorHover(QColor(CLR_BKGND_HOVER_NOTE_ITEM));
    setColorPress(QColor(CLR_BKGND_PRESS_NOTE_ITEM));
    setColorSelected(QColor(CLR_BKGND_SELECTED_NOTE_ITEM));

    connect(this, SIGNAL(selectedChanged()), this, SLOT(onWidgetSelectedChanged()));
}

CNoteItemWidget::~CNoteItemWidget()
{
}

CNoteItem* CNoteItemWidget::getNoteItem() const
{
    return m_pNoteItem;
}

void CNoteItemWidget::setNoteItem(CNoteItem* pNoteItem)
{
    m_pNoteItem = pNoteItem;
    connect(m_pNoteItem, SIGNAL(selectedChanged()), this, SLOT(onNoteItemSelectedChanged()));

    setSelected(m_pNoteItem->isSelected());

    setToolTip(m_pNoteItem->getTitle());
}

void CNoteItemWidget::onWidgetSelectedChanged()
{
    m_pNoteItem->setSelected(isSelected());
}

void CNoteItemWidget::onNoteItemSelectedChanged()
{
    setSelected(m_pNoteItem->isSelected());
}

void CNoteItemWidget::paintEvent( QPaintEvent * event)
{
    CHsWidget::paintEvent(event);

    if(!m_pNoteItem)
        return;

    QRect rcLeft = rect();
    QPainter painter(this);
    painter.save();
    QFont ft = font();
    ft.setPointSize(10);
    ft.setFamily("Calibri");
    painter.setFont(ft);

    QImage img = m_pNoteItem->getImg();
    QRect rtImg = rcLeft;
    if(rtImg.width() > 16)
        rtImg.setWidth(16);

    painter.drawImage(rtImg, img);
    if(m_pNoteItem)
    {
        rcLeft.setLeft(17);
        painter.setPen(QPen(QColor(CLR_FONT_NOTE_TEXT)));
        painter.drawText(rcLeft, m_pNoteItem->getTitle());
    }

    painter.restore();
    painter.end();
}
