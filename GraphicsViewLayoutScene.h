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

#ifndef GRAPHICSVIEWLAYOUTSCENE_H
#define GRAPHICSVIEWLAYOUTSCENE_H

#include <QGraphicsView>
#include <QImage>

class CGraphicsViewLayoutScene : public QGraphicsView
{
    Q_OBJECT
public:
    explicit CGraphicsViewLayoutScene(QWidget *parent = 0);

    void setBackgroundImage(const QImage& img);
    virtual void layout();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent *event);

signals:

public slots:

protected:
    QImage m_imgBkgd;
    QRect m_rcClient;
};

#endif // GRAPHICSVIEWLAYOUTSCENE_H
