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

#ifndef GRAPHICSHSITEM_H
#define GRAPHICSHSITEM_H

#include <QFont>
#include <QLayoutItem>
#include <QGraphicsItem>
#include <QGraphicsColorizeEffect>
#include <QGraphicsDropShadowEffect>

class CGraphicsHsItem : public QGraphicsObject , public QLayoutItem
{
    Q_OBJECT
public:
    explicit CGraphicsHsItem(QGraphicsItem *parent = 0);

    void SetDisabled(bool bDisabled = true);
    void SetStartClicking(bool bStart = true);

    void click();
    void doubleClick();

    bool IsSelectable();

Q_SIGNALS:
    void OnDoubleClick();
    void OnClick();

protected:
    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    void DrawSelected(QPainter *painter);
    void DrawHover(QPainter *painter);

    void SetHover(bool bHovered);

    QFont m_font;

    bool m_bIsHovered;
    bool m_bDisabled;
    bool m_bHasDoubleClick;
    bool m_bStartClicking;
    bool m_bSelectable;
    bool m_bChecked;

    QColor m_clrSelected;
    QColor m_clrHovered;

    QGraphicsColorizeEffect m_effectGray;
    QGraphicsDropShadowEffect m_effectShadown;

};

#endif // GRAPHICSHSITEM_H
