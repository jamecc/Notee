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

#include "DlgBallonTip.h"
#include "ui_DlgBallonTip.h"
#include <QDesktopWidget>

CDlgBallonTip::CDlgBallonTip(QWidget *parent) :
    QDialog(parent, Qt::Tool),
    ui(new Ui::CDlgBallonTip)
{
    ui->setupUi(this);

    m_bFirstShow = true;
}

CDlgBallonTip::~CDlgBallonTip()
{
    delete ui;
}

void CDlgBallonTip::showBalloon(QString strContent, QString strTitle)
{
    if(m_bFirstShow)
    {
        QDesktopWidget* desktopWidget = QApplication::desktop();
        QRect screenRect = desktopWidget->screenGeometry();
        move(screenRect.width()-500,screenRect.height()-150);
        m_bFirstShow = false;
    }

    setWindowTitle(strTitle);
    ui->label->setText(strContent);
    show();

    startTimer(5000);
}

void CDlgBallonTip::timerEvent(QTimerEvent *e)
{
    killTimer(e->timerId());
    hide();
}
