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

#ifndef DLGSHORTNEWNOTE_H
#define DLGSHORTNEWNOTE_H

#include <QDialog>
#include <QLineEdit>

class CDlgShortNewNote : public QWidget
{
    Q_OBJECT
public:
    explicit CDlgShortNewNote(QWidget *parent = 0);

protected:
    void createLayouts();
    
signals:
    void emitReturnString(QString);
    
public slots:
    void onReturnPressed();

private:
    QLineEdit* m_pLineEdit;
    
};

#endif // DLGSHORTNEWNOTE_H
