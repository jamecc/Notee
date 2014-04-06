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

#include "DlgForgotPwd.h"
#include "ui_DlgForgotPwd.h"
#include "MainFrame.h"

CDlgForgotPwd::CDlgForgotPwd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgForgotPwd)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(onOk()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(onCancel()));
    connect(ui->lineEditUserName, SIGNAL(returnPressed()), this, SLOT(onOk()));
}

CDlgForgotPwd::~CDlgForgotPwd()
{
    delete ui;
}

void CDlgForgotPwd::initControls()
{
    ui->lineEditUserName->setText("");
}

QString CDlgForgotPwd::getUserName()
{
    return ui->lineEditUserName->text().trimmed();
}

QString CDlgForgotPwd::getEmail()
{
    return ui->lineEditUserName->text().trimmed();
}

void CDlgForgotPwd::onOk()
{
    G_MainFrame()->afterForgotPwd();
}

void CDlgForgotPwd::onCancel()
{
    G_MainFrame()->afterForgotPwdCancel();
}

void CDlgForgotPwd::showEvent(QShowEvent *)
{
    ui->lineEditUserName->setFocus();
}
