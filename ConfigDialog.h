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

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class CSettings;

class CConfigDialog : public QDialog
{
    Q_OBJECT
public:
    CConfigDialog(QWidget *parent = 0, CSettings* pSettings = 0);

signals:

public slots:

private Q_SLOTS:
    void onChangePage(QListWidgetItem* pCurrent, QListWidgetItem* pPrevious);
    void onOk();

    bool isModified() const {return m_bModified;}

private:
    void createPageItems();

private:
    QListWidget* m_pListContentWidget;
    QStackedWidget* m_pPagesWidget;
    CSettings* m_pSettings;

    bool m_bModified;
};

#endif // CONFIGDIALOG_H
