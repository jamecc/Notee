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
#include <QBrush>
#include <QGraphicsColorizeEffect>
#include "GraphicsHsItem.h"

CGraphicsHsItem::CGraphicsHsItem(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , m_bIsHovered(false)
    , m_bDisabled(false)
    , m_bHasDoubleClick(true)
    , m_bStartClicking(false)
    , m_bChecked(false)
{
      m_clrSelected = QColor(128,25,25, 25);
      m_clrHovered = QColor(25,25,125, 25);

      m_bSelectable = true;
      setFlag(ItemIsSelectable);
      setAcceptHoverEvents(true);
      setAcceptedMouseButtons(Qt::LeftButton);

      m_effectGray.setColor(QColor(128,128,128,128));
      m_effectShadown.setColor(QColor(128,128,128,128));
      m_effectShadown.setOffset(2.0, 2.0);
      m_effectShadown.setBlurRadius(5.0);
//	m_grayEffect.setStrength(0.5);
      m_font = QFont("Arial", 14, QFont::Normal, false);

      setZValue(1.0);
}

void CGraphicsHsItem::click()
{
      emit OnClick();
}

void CGraphicsHsItem::doubleClick()
{
      emit OnDoubleClick();
}

bool CGraphicsHsItem::IsSelectable()
{
      return m_bSelectable;
}

void CGraphicsHsItem::SetDisabled(bool bDisabled/* = true*/)
{
      m_bDisabled = bDisabled;
      if(m_bDisabled)
      {
              setAcceptHoverEvents(false);
              setAcceptedMouseButtons(Qt::NoButton);
              QGraphicsColorizeEffect* pEffect = new QGraphicsColorizeEffect;
              pEffect->setColor(m_effectGray.color());
              setGraphicsEffect(NULL);
              setGraphicsEffect(pEffect);
      }
      else
      {
              setAcceptHoverEvents(true);
              setAcceptedMouseButtons(Qt::LeftButton);
              setGraphicsEffect(NULL);
      }
}

void CGraphicsHsItem::SetStartClicking(bool bStart/* = true*/)
{
      m_bStartClicking = bStart;
}

QVariant CGraphicsHsItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
      if(change == ItemSelectedChange)
      {
              update();
      }

      return QGraphicsItem::itemChange(change, value);
}

QRectF CGraphicsHsItem::boundingRect() const
{
 //     return QRectF(GetRect());
      return QRectF();
}

void CGraphicsHsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
      DrawSelected(painter);
      DrawHover(painter);
}

void CGraphicsHsItem::DrawSelected(QPainter *painter)
{
      if(isSelected())
      {
              QBrush bhSelected(m_clrSelected);
              painter->setBrush(bhSelected);
//              painter->drawRect(GetRectLeft());
      }
}

void CGraphicsHsItem::DrawHover(QPainter *painter)
{
      if(m_bIsHovered)
      {
              QBrush bhHover(m_clrHovered);
              painter->setBrush(bhHover);
//              painter->drawRect(GetRectLeft());
      }
}

void CGraphicsHsItem::SetHover(bool bHovered)
{
      m_bIsHovered = bHovered;

      if(m_bIsHovered)
      {
              QGraphicsDropShadowEffect* pEffect = new QGraphicsDropShadowEffect;
              pEffect->setColor(m_effectShadown.color());
              pEffect->setOffset(m_effectShadown.offset());
              pEffect->setBlurRadius(m_effectShadown.blurRadius());
              setGraphicsEffect(NULL);
              setGraphicsEffect(pEffect);
      }
      else
              setGraphicsEffect(NULL);
}

void CGraphicsHsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
      if(!m_bIsHovered)
      {
              SetHover(true);
              update();
      }
}

void CGraphicsHsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
      if(m_bIsHovered)
      {
              SetHover(false);
              update();
      }
}
