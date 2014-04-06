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
#include "DlgLogin.h"
#include "MainFrame.h"

CDlgLogin::CDlgLogin(QWidget *parent)
    : QWidget(parent)
{
    setMaximumSize(QSize(400,200));
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    // icon
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/res/login.png"));
    setWindowIcon(icon1);

    // title
    setWindowTitle(tr("Login"));

    createLayout();
}

CDlgLogin::~CDlgLogin()
{

}

void CDlgLogin::initControls()
{
    m_pEditUserName->setText("");
    m_pEditPassword->setText("");
    m_pCheckBoxAlwaysLogin->setChecked(false);
}

void CDlgLogin::createLayout()
{
    QHBoxLayout* pMainLayout = new QHBoxLayout;
    setLayout(pMainLayout);
    QGroupBox* pGroupBox = new QGroupBox(this);
    pMainLayout->addWidget(pGroupBox);
    pGroupBox->setTitle(tr("Login"));

    // label edit
    QLabel* pLabelUserName = new QLabel(tr("User Name:"), this);
    QLabel* pLabelPassword = new QLabel(tr("Password:"), this);
    m_pEditUserName = new QLineEdit(this);
    m_pEditPassword = new QLineEdit(this);
    pLabelUserName->setBuddy(m_pEditUserName);
    pLabelPassword->setBuddy(m_pEditPassword);
    m_pEditPassword->setEchoMode(QLineEdit::Password);
    connect(m_pEditUserName, SIGNAL(returnPressed()), this, SLOT(onOk()));
    connect(m_pEditPassword, SIGNAL(returnPressed()), this, SLOT(onOk()));

    m_pCheckBoxAlwaysLogin = new QCheckBox(this);
    m_pCheckBoxAlwaysLogin->setText(tr("Always login by the name"));

    // ok cancel
    m_pBtnOk = new QPushButton(this);
    m_pBtnOk->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/res/login.png"));
    m_pBtnOk->setIcon(icon1);
    m_pBtnOk->setIconSize(QSize(48,48));
    m_pBtnOk->setToolTip(tr("Login"));
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
    pGridLayout->addWidget(pLabelPassword, 1, 0);
    pGridLayout->addWidget(m_pEditUserName, 0, 1);
    pGridLayout->addWidget(m_pEditPassword, 1, 1);
    pGridLayout->addWidget(m_pBtnOk, 0, 2, 2, 1);
    pGridLayout->addWidget(m_pCheckBoxAlwaysLogin, 2, 0, 1, 2);
    pGridLayout->addWidget(m_pBtnCancel, 2, 2);
    pGroupBox->setLayout(pGridLayout);
    pGridLayout->setContentsMargins(20, 0, 20, 0);
}

QString CDlgLogin::getUserName()
{
    return m_pEditUserName->text();
}

QString CDlgLogin::getPassword()
{
    return m_pEditPassword->text();
}

void CDlgLogin::setUserName(const QString& str)
{
    m_pEditUserName->setText(str);
}

void CDlgLogin::setPassword(const QString& str)
{
    m_pEditPassword->setText(str);
}

bool CDlgLogin::getAlwaysLogin()
{
    return Qt::Checked == m_pCheckBoxAlwaysLogin->checkState();
}

void CDlgLogin::setAlwaysLogin(bool bAlwaysLogin)
{
    m_pCheckBoxAlwaysLogin->setChecked(bAlwaysLogin);
}

void CDlgLogin::onOk()
{
    G_MainFrame()->afterLogin();
//    QDialog::accept();
}

void CDlgLogin::onCancel()
{
//    QDialog::reject();
    G_MainFrame()->afterLoginCancel();
}

void CDlgLogin::showEvent(QShowEvent * )
{
    m_pEditUserName->setFocus();
}
