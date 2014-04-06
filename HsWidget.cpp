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
#include <QGraphicsWidget>
#include "predefine.h"
#include "HsWidget.h"

CHsWidget::CHsWidget(QWidget *parent)
    : QWidget(parent)
{
    m_bSelected = false;
    m_bHover = false;
    m_bPressed = false;

    m_bIsSelectable = false;
    m_bIsHoverable = false;
    m_bIsPressable = false;

    m_pGraphicsWidget = 0;

    m_clrHover = QColor(CLR_BKGND_HOVER);
    m_clrPress = QColor(CLR_BKGND_PRESS);
    m_clrSelected = QColor(CLR_BKGND_SELECTED);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onContextMenu(const QPoint&)));
}

CHsWidget::~CHsWidget()
{
    emitRemoveMe();
}

void CHsWidget::paintEvent ( QPaintEvent * event )
{
    QWidget::paintEvent(event);

    if(m_bPressed)
    {
        QRect rect2 = rect();
        QPainter painter(this);
        painter.fillRect(rect2, QBrush(m_clrPress));
        painter.end();
    }
    else if(m_bHover)
    {
        QRect rect2 = rect();
        QPainter painter(this);
        painter.fillRect(rect2, QBrush(m_clrHover));
        painter.end();
    }
    else if(m_bSelected)
    {
        QRect rect2 = rect();
        QPainter painter(this);
        painter.fillRect(rect2, QBrush(m_clrSelected));
        painter.end();
    }
}

void CHsWidget::mousePressEvent(QMouseEvent * event)
{
    event->accept();
//    QWidget::mousePressEvent(event);

    if ( event->button() == Qt::LeftButton)
    {
        if(m_bIsPressable)
        {
            m_bPressed = true;
            repaint();
        }
    }
}

void CHsWidget::mouseReleaseEvent(QMouseEvent * event)
{
    event->accept();
 //   QWidget::mouseReleaseEvent(event);

    if ( event->button() == Qt::LeftButton )
    {
        if(m_bIsPressable)
        {
            m_bPressed = false;
            repaint();
        }

        emit click(this);
    }
}

void CHsWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
//    QWidget::mouseDoubleClickEvent(event);
    event->accept();
    if ( event->button() == Qt::LeftButton )
    {
        if(m_bIsPressable)
        {
            m_bPressed = false;
            repaint();
        }
        emit dbClick(this);
    }
}

void CHsWidget::enterEvent(QEvent * event)
{
    event->accept();
//    QWidget::enterEvent(event);
    if(m_bIsHoverable)
    {
        m_bHover = true;
        repaint();
    }
}

void CHsWidget::leaveEvent(QEvent * event)
{
    event->accept();
 //   QWidget::leaveEvent(event);
    if(m_bIsHoverable)
    {
        m_bHover = false;
        repaint();
    }
}

bool CHsWidget::setSelected(bool bSelected)
{
    if(m_bIsSelectable && m_bSelected != bSelected)
    {
        m_bSelected = bSelected;
        repaint();
        emit selectedChanged();
        return true;
    }

    return false;
}

bool CHsWidget::isSelected() const
{
    return m_bSelected;
}

void CHsWidget::setSelectable(bool bValue)
{
    m_bIsSelectable = bValue;
}

void CHsWidget::setHoverable(bool bValue)
{
    m_bIsHoverable = bValue;
}

void CHsWidget::setPressable(bool bValue)
{
    m_bIsPressable = bValue;
}

void CHsWidget::setColorSelected(const QColor& clrSelected)
{
    m_clrSelected = clrSelected;
}

void CHsWidget::setColorHover(const QColor& clrHover)
{
    m_clrHover = clrHover;
}

void CHsWidget::setColorPress(const QColor& clrPress)
{
    m_clrPress = clrPress;
}

void CHsWidget::setGraphicsWidget(QGraphicsWidget* pGraphicsWidget)
{
    m_pGraphicsWidget = pGraphicsWidget;
}

void CHsWidget::emitRemoveMe()
{
    emit removeMe(this);
}

void CHsWidget::onContextMenu(const QPoint &pt)
{
    emit emitContexMenu(this);
}
