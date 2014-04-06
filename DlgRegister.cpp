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

#include "stdafx.h"
#include <QNetworkReply>
#include <QRegExp>
#include <QGroupBox>
#include "DlgRegister.h"
#include "MainFrame.h"

CDlgRegister::CDlgRegister(QWidget *parent)
    : QWidget(parent)
{
    setMaximumSize(QSize(500,300));
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    // icon
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/res/register.png"));
    setWindowIcon(icon1);

    // title
    setWindowTitle(tr("Register"));

    createLayout();
}

CDlgRegister::~CDlgRegister()
{
}

void CDlgRegister::createLayout()
{
    QHBoxLayout* pMainLayout = new QHBoxLayout;
    setLayout(pMainLayout);
    QGroupBox* pGroupBox = new QGroupBox(this);
    pMainLayout->addWidget(pGroupBox);
    pGroupBox->setTitle(tr("Register"));

    // label edit
    QLabel* pLabelUserName = new QLabel(tr("User Name:"), this);
    QLabel* pLabelEmail = new QLabel(tr("Email:"), this);
    QLabel* pLabelPassword = new QLabel(tr("Password:"), this);
    QLabel* pLabelConfirmPwd = new QLabel(tr("Confirm Password:"), this);
    m_pEditUserName = new QLineEdit(this);
    m_pEditEmail = new QLineEdit(this);
    m_pEditPassword = new QLineEdit(this);
    m_pEditConfirmPwd = new QLineEdit(this);
    pLabelUserName->setBuddy(m_pEditUserName);
    pLabelEmail->setBuddy(m_pEditEmail);
    pLabelPassword->setBuddy(m_pEditPassword);
    pLabelConfirmPwd->setBuddy(m_pEditConfirmPwd);
    m_pEditPassword->setEchoMode(QLineEdit::Password);
    m_pEditConfirmPwd->setEchoMode(QLineEdit::Password);
    m_pEditEmail->setToolTip(tr("Get forgot password by the e-mail."));
    connect(m_pEditUserName, SIGNAL(returnPressed()), this, SLOT(onOk()));
    connect(m_pEditEmail, SIGNAL(returnPressed()), this, SLOT(onOk()));
    connect(m_pEditPassword, SIGNAL(returnPressed()), this, SLOT(onOk()));
    connect(m_pEditConfirmPwd, SIGNAL(returnPressed()), this, SLOT(onOk()));

    // ok cancel
    m_pBtnOk = new QPushButton(this);
    m_pBtnOk->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/res/register.png"));
    m_pBtnOk->setIcon(icon1);
    m_pBtnOk->setIconSize(QSize(48,48));
    m_pBtnOk->setToolTip(tr("Register"));
    m_pBtnCancel = new QPushButton(this);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/res/cancel.png"));
    m_pBtnCancel->setIcon(icon2);
    m_pBtnCancel->setText(tr("Cancel"));
    m_pBtnCancel->setIconSize(QSize(16,16));
    m_pBtnCancel->setToolTip(tr("Cancel"));

    // connect
    connect(m_pBtnOk, SIGNAL(clicked()), this, SLOT(onOk()));
    connect(m_pBtnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));

    // layout
    QGridLayout* pGridLayout = new QGridLayout(this);
    pGridLayout->addWidget(pLabelUserName, 0, 0);
    pGridLayout->addWidget(pLabelEmail, 1, 0);
    pGridLayout->addWidget(pLabelPassword, 2, 0);
    pGridLayout->addWidget(pLabelConfirmPwd, 3, 0);
    pGridLayout->addWidget(m_pEditUserName, 0, 1);
    pGridLayout->addWidget(m_pEditEmail, 1, 1);
    pGridLayout->addWidget(m_pEditPassword, 2, 1);
    pGridLayout->addWidget(m_pEditConfirmPwd, 3, 1);
    pGridLayout->addWidget(m_pBtnOk, 0, 2, 4, 1);
    pGridLayout->addWidget(m_pBtnCancel, 4, 2);
    pGroupBox->setLayout(pGridLayout);
    pGridLayout->setContentsMargins(20, 0, 20, 0);
}

void CDlgRegister::initControls()
{
    m_pEditUserName->setText("");
    m_pEditEmail->setText("");
    m_pEditPassword->setText("");
    m_pEditConfirmPwd->setText("");
}

QString CDlgRegister::getUserName()
{
    return m_pEditUserName->text().trimmed();
}

QString CDlgRegister::getEmail()
{
   return m_pEditEmail->text().trimmed();
}

QString CDlgRegister::getPassword()
{
    return m_pEditPassword->text().trimmed();
}

bool CDlgRegister::verifyControls()
{
    QString strUserName = m_pEditUserName->text().trimmed();
    if(strUserName.isEmpty() || strUserName.length() < 6 || strUserName.length() > 32)
    {
        QMessageBox::critical(this, tr("Error"), tr("the length of User Name is larger than 5 and less than 33."));
        return false;
    }

    QString strEmail = m_pEditEmail->text().trimmed();
    if(strEmail.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Email cannot be empty."));
        return false;
    }

    QString strPassword = m_pEditPassword->text().trimmed();
    if(strPassword.isEmpty() || strPassword.length() < 6 || strPassword.length() > 32)
    {
        QMessageBox::critical(this, tr("Error"), tr("the length of Password is larger than 5 and less than 33."));
        return false;
    }

    QString strConfirmPwd = m_pEditConfirmPwd->text().trimmed();
    if(strConfirmPwd.isEmpty() || strConfirmPwd.length() < 6 || strConfirmPwd.length() > 32)
    {
        QMessageBox::critical(this, tr("Error"), tr("the length of Confirm Password is larger than 5 and less than 33."));
        return false;
    }

    if(strConfirmPwd != strPassword)
    {
        QMessageBox::critical(this, tr("Error"), tr("Password isn't same with Confirm Password."));
        return false;
    }

    QRegExp regExpEmail("^[0-9a-zA-Z_-.]+@[0-9a-zA-Z_-]+.[0-9a-zA-Z_-]+");
    if(!regExpEmail.exactMatch(strEmail))
    {
        QMessageBox::critical(this, tr("Error"), tr("The format of Email is wrong."));
        return false;
    }

    QRegExp regExp("^[a-zA-Z][0-9a-zA-Z_-.]+");
    if(!regExp.exactMatch(strUserName))
    {
        QMessageBox::critical(this, tr("Error"), tr("The format of User Name should be \'^[a-zA-Z][0-9a-zA-Z_-.]+\'."));
        return false;
    }

    return true;
}

void CDlgRegister::onOk()
{
    if(!verifyControls())
        return;

    G_MainFrame()->afterRegister();
//    QDialog::accept();
}

void CDlgRegister::onCancel()
{
//    QDialog::reject();
    G_MainFrame()->afterRegisterCancel();
}

void CDlgRegister::showEvent(QShowEvent *)
{
    m_pEditUserName->setFocus();
}
