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

#include "MyCleanlooksStyle.h"

CMyCleanlooksStyle::CMyCleanlooksStyle()
{
}

void CMyCleanlooksStyle::polish(QPalette &pal)
{
    QCleanlooksStyle::polish(pal);

    pal.setColor(QPalette::Button, QColor(230,233,233));
    pal.setColor(QPalette::Background, QColor(240,243,243));
}

/*
void CMyCleanlooksStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                                QPainter *painter, const QWidget *widget) const
{
    if (option->state & State_MouseOver) {
        painter->setPen(QColor(250,0,0));
        painter->setBrush(QColor(250,0,0));
    }

    QCleanlooksStyle::drawPrimitive(element, option, painter, widget);
}
*/
