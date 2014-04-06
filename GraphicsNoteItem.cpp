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

#include "GraphicsNoteItem.h"

CGraphicsNoteItem::CGraphicsNoteItem(QGraphicsItem *parent, CNoteItem* pNoteItem)
    : CGraphicsHsItem(parent)
    , m_pNoteItem(pNoteItem)
{
}

void CGraphicsNoteItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    /*
        QPoint ptStartText(GetRectLeft().left() + m_nMarginIcon, GetRectLeft().top() + m_nMarginIcon);
        if(!m_pixmap.isNull())
        {
                QRect rcTarget(ptStartText, m_sizeIcon);
                painter->drawPixmap(rcTarget, m_pixmap, m_pixmap.rect());
                ptStartText.setX(ptStartText.x() + m_nMarginIcon * 2 + m_pixmap.width());
                ptStartText.setY(ptStartText.y() + m_nMarginIcon * 2 + m_pixmap.height());
        }

        if(m_strTitle.length())
        {
                ptStartText = QPoint(GetRectLeft().left() + m_sizeIcon.width() + m_nMarginIcon * 2, GetRectLeft().top() + m_nMarginIcon);
                painter->setFont(m_pDay->GetCalendar()->GetFontNoteeItem());
                painter->drawText(ptStartText.x(), ptStartText.y(), GetRectLeft().right() - ptStartText.x(), GetRectLeft().bottom() - ptStartText.y(), Qt::AlignLeft, m_strTitle);
        }

        CMLayoutGraphItem::paint(painter, option, widget);
        */
}
