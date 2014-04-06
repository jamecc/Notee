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

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QWidget>
#include <QTabBar>
#include "DCTableWidget.h"
#include "NoteItem.h"

class CTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CTabWidget(QWidget *parent = 0);

    void updateTabLabels(const QStringList& lstStrings);

    void clearContent();
    void refresh();

    void selectNoteItem(CNoteItem* pNoteItem);
    
signals:
    
public slots:
    void onTabCurrentChanged(int);

protected:
    void initControls();

private:
    QTabBar* m_pTabBar;
    CDCTableWidget* m_pTableWidget;

    QList<CNoteItem*> m_lstNoteItems;

    bool m_bInit;
};

#endif // TABWIDGET_H
