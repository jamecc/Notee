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

#ifndef DLGLOGIN_H
#define DLGLOGIN_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
class QPushButton;

class CDlgLogin : public QWidget
{
	Q_OBJECT
public:
	CDlgLogin(QWidget *parent);
	~CDlgLogin();

    void initControls();

    QString getUserName();
    QString getPassword();

    void setUserName(const QString& str);
    void setPassword(const QString& str);

    bool getAlwaysLogin();
    void setAlwaysLogin(bool bAlwaysLogin);

protected Q_SLOTS:
    void onOk();
    void onCancel();

protected:
    virtual void showEvent(QShowEvent *);

private:
    void createLayout();

    QLineEdit* m_pEditUserName;
    QLineEdit* m_pEditPassword;
    QCheckBox* m_pCheckBoxAlwaysLogin;
    QPushButton* m_pBtnOk;
    QPushButton* m_pBtnCancel;
};

#endif // DLGLOGIN_H
