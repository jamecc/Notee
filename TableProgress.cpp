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

#include "TableProgress.h"
#include <QMenu>
#include "MainFrame.h"

CTableProgress::CTableProgress(QWidget * parent)
    : QTableWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onContextMenu(const QPoint&)));

    // init
    setColumnCount(4);
    //setHorizontalHeaderItem(0, new QTableWidgetItem("Desc"));
    //setHorizontalHeaderItem(1, new QTableWidgetItem("Upload"));
    //setHorizontalHeaderItem(2, new QTableWidgetItem("Download"));
    //setHorizontalHeaderItem(3, new QTableWidgetItem("Memo"));

    QStringList lstHeader;
    lstHeader << tr("Desc") << tr("Upload") << tr("Download") << tr("Memo");
    setHorizontalHeaderLabels(lstHeader);
}

void CTableProgress::onContextMenu(const QPoint&)
{
    QMenu menu(this);

    // delete
    QAction* pActionDelete = menu.addAction(tr("Delete"), this, SLOT(onDelete()));
    QIcon icon1(QString::fromUtf8(":/res/erase.png"));
    pActionDelete->setIcon(icon1);
    // clear all
    menu.addAction(tr("Clear all"), this, SLOT(onClearAll()));
    menu.addSeparator();
    menu.addAction(G_MainFrame()->getActionSync());

    pActionDelete->setEnabled(selectionModel()->selectedRows().length());

    menu.exec(QCursor::pos());
}

void CTableProgress::onDelete()
{
    if(selectionModel()->selectedRows().length() > 0)
    {
        removeRow(selectionModel()->selectedRows().at(0).row());
    }
}

void CTableProgress::onClearAll()
{
    while(this->rowCount())
    {
        removeRow(0);
    }
}
