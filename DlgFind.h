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

#ifndef DLGFIND_H
#define DLGFIND_H

#include <QDialog>
#include <QTableWidgetItem>

class QLineEdit;
class QTableWidget;
class CNoteItem;
class CDCTableWidget;
class CDlgFind : public QWidget
{
    Q_OBJECT
public:
    explicit CDlgFind(QWidget *parent = 0);

    void afterAddNoteItem();
    void refresh();
    void clearNoteItems();
    void clearContent();
    void selectNoteItem(CNoteItem* pNoteItem);

    const QString& getFindText() const {return m_strFindText;}

    void updateTabLabels(const QStringList& lstStrings);
    bool needRefreshAfterChangeMonth();

    CDCTableWidget* getTableWidget() const {return m_pTableWidget;}
    
signals:
    
public slots:
    void onFind();
    void onCurrent();
    void onTabCurrentChanged(int);

protected:

private:
    bool m_bSender;
    QString m_strFindText;
    QLineEdit* m_pLineEdit;
    QTabBar* m_pTabBar;
    CDCTableWidget* m_pTableWidget;
};

#endif // DLGFIND_H
