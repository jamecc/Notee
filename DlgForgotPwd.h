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

#ifndef DLGFORGOTPWD_H
#define DLGFORGOTPWD_H

#include <QWidget>

namespace Ui {
class CDlgForgotPwd;
}

class CDlgForgotPwd : public QWidget
{
    Q_OBJECT
    
public:
    explicit CDlgForgotPwd(QWidget *parent = 0);
    ~CDlgForgotPwd();

    void initControls();
    QString getUserName();
    QString getEmail();

protected slots:
    void onOk();
    void onCancel();

protected:
    virtual void showEvent(QShowEvent *);
    
private:
    Ui::CDlgForgotPwd *ui;
};

#endif // DLGFORGOTPWD_H
