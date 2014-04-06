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

#include "GraphicsViewLayoutScene.h"

#define MARGIN_VIEW 5

CGraphicsViewLayoutScene::CGraphicsViewLayoutScene(QWidget *parent)
    : QGraphicsView(parent)
{
}

void CGraphicsViewLayoutScene::setBackgroundImage(const QImage& img)
{
    m_imgBkgd = img;
 //   invalidateScene();
    scene()->invalidate();
}

void CGraphicsViewLayoutScene::paintEvent(QPaintEvent *event)
{
    if(!m_imgBkgd.isNull())
    {
        QPainter painter(viewport());
        painter.drawImage(rect(), m_imgBkgd);
        painter.end();
    }

    QGraphicsView::paintEvent(event);
}

void CGraphicsViewLayoutScene::resizeEvent(QResizeEvent *event)
{
    if(m_rcClient != rect())
    {
        m_rcClient = rect();
        layout();
    }
}

void CGraphicsViewLayoutScene::layout()
{
    QGraphicsScene* pScene = scene();
    if(!pScene)
        return;

    QRect rectTmp = rect();
    rectTmp = QRect(rectTmp.left(), rectTmp.top(), rectTmp.width() - MARGIN_VIEW, rectTmp.height() - MARGIN_VIEW);
    pScene->setSceneRect(rectTmp);
}
