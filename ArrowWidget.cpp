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

#include "ArrowWidget.h"
#include <QPainter>

#define IMG_WIDTH 32

CArrowWidget::CArrowWidget(QWidget *parent, int nType) :
    CHsWidget(parent)
{
    m_nType = nType;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setHoverable(true);
    setPressable(true);

    switch(m_nType)
    {
    case TYPE_PREVIOUS:
        setToolTip(tr("Previous Month"));
        break;
    case TYPE_NEXT:
    default:
        setToolTip(tr("Next Month"));
        break;
    }
}

void CArrowWidget::paintEvent ( QPaintEvent * event )
{
    CHsWidget::paintEvent(event);
    QRect rcLeft = rect();
    QPainter painter(this);
    painter.save();

    QImage img;
    if(m_bHover)
    {
        switch(m_nType)
        {
        case TYPE_PREVIOUS:
            img.load(QString::fromUtf8(":/res/arrow_prev.png"));
            break;
        case TYPE_NEXT:
        default:
            img.load(QString::fromUtf8(":/res/arrow_next.png"));
            break;
        }
    }

    QRect rcImg = rcLeft;
    if(rcImg.width() > IMG_WIDTH)
    {
        rcImg.setLeft((rcLeft.width() - IMG_WIDTH) / 2);
        rcImg.setWidth(IMG_WIDTH);
    }

    if(rcImg.height() > IMG_WIDTH)
    {
        rcImg.setTop((rcLeft.height() - IMG_WIDTH) / 2);
        rcImg.setHeight(IMG_WIDTH);
    }
    painter.drawImage(rcImg, img);
    painter.restore();
}
