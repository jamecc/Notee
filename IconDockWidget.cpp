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

#include "IconDockWidget.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>

CIconDockWidget::CIconDockWidget(QIcon icon, QString strTitle, QWidget *parent) :
    QDockWidget(strTitle, parent)
{
    /*
    QWidget* pWidget = new QWidget(this);
    QHBoxLayout* pMainLayout = new QHBoxLayout();
    pWidget->setLayout(pMainLayout);
    pMainLayout->setMargin(0);
    pMainLayout->setContentsMargins(0,0,0,0);

    if(!icon.isNull())
    {
        QLabel* pLabel1 = new QLabel(this);
        pLabel1->setPixmap(icon.pixmap(16,16));
        pMainLayout->addWidget(pLabel1);
    }

    QLabel* pLabel2 = new QLabel(strTitle,this);
    pMainLayout->addWidget(pLabel2);
    pMainLayout->addStretch(1);

    QPushButton* pBtnMax = new QPushButton(this);
    pBtnMax->setIconSize(QSize(8,8));
    pBtnMax->setIcon(QIcon(":/res/dock.png"));
    pBtnMax->setContentsMargins(0,0,0,0);
    QPushButton* pBtnHide = new QPushButton(this);
    pBtnHide->setIconSize(QSize(8,8));
    pBtnHide->setIcon(QIcon(":/res/close.png"));
    pBtnHide->setContentsMargins(0,0,0,0);
    pMainLayout->addWidget(pBtnMax);
    pMainLayout->addWidget(pBtnHide);


    connect(pBtnMax,SIGNAL(clicked()),this,SLOT(onClickBtnFloating()));
    connect(pBtnHide,SIGNAL(clicked()),this,SLOT(hide()));

    setTitleBarWidget(pWidget);

    pWidget->setStyleSheet("QWidget { background: #F7F9F9 }");
    */
}

void CIconDockWidget::onClickBtnFloating()
{
    setFloating(!isFloating());
}
