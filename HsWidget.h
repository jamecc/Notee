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

#ifndef HSWIDGET_H
#define HSWIDGET_H

#include <QWidget>

class QGraphicsWidget;
class CHsWidget : public QWidget
{
    Q_OBJECT
public:
    CHsWidget(QWidget *parent = 0);
    virtual ~CHsWidget();

    virtual bool setSelected(bool bSelected);
    bool isSelected() const;
    void setSelectable(bool bValue);
    void setHoverable(bool bValue);
    void setPressable(bool bValue);

    void setColorSelected(const QColor& clrSelected);
    void setColorHover(const QColor& clrHover);
    void setColorPress(const QColor& clrPress);

    void setGraphicsWidget(QGraphicsWidget* pGraphicsWidget);

    void emitRemoveMe();

Q_SIGNALS:
    void click(CHsWidget* pThis);
    void emitContexMenu(CHsWidget* pThis);
    void dbClick(CHsWidget* pThis);
    void selectedChanged();
    void removeMe(QWidget* pMe);

public slots:
    void onContextMenu(const QPoint& pt);

protected:
    virtual void paintEvent ( QPaintEvent * event );
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);

    bool m_bSelected;
    bool m_bHover;
    bool m_bPressed;

    bool m_bIsSelectable;
    bool m_bIsHoverable;
    bool m_bIsPressable;

    QGraphicsWidget* m_pGraphicsWidget;

    QColor m_clrHover;
    QColor m_clrPress;
    QColor m_clrSelected;
};

#endif // HSWIDGET_H
