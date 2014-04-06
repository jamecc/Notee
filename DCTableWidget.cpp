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

#include <QMenu>
#include <QMouseEvent>
#include "DCTableWidget.h"
#include "MainFrame.h"
#include "predefine.h"

CDCTableWidget::CDCTableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setColumnCount(3);
    setHorizontalHeaderItem(0, new QTableWidgetItem("Type"));
    setHorizontalHeaderItem(1, new QTableWidgetItem("Date/Time"));
    setHorizontalHeaderItem(2, new QTableWidgetItem("Title"));

    setColumnWidth(0, 40);
    setColumnWidth(1, 100);
    setColumnWidth(2, 160);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onContextMenu(const QPoint&)));
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
    connect(this, SIGNAL(itemDoubleClicked(QTableWidgetItem *)), this, SLOT(onItemDoubleClicked(QTableWidgetItem *)));
}

QStyleOptionViewItem CDCTableWidget::viewOptions() const
{
    QStyleOptionViewItem option = QTableWidget::viewOptions();
//    option.decorationAlignment = Qt::AlignCenter;
//        option.decorationPosition = QStyleOptionViewItem::Top;

    return option;
}

void CDCTableWidget::mouseDoubleClickEvent( QMouseEvent *e )
{
    if(e->button() == Qt::LeftButton)
        QTableWidget::mouseDoubleClickEvent(e);
    else
        e->ignore();
}

void CDCTableWidget::onContextMenu(const QPoint& pt)
{
    QMenu menu(this);

    bool bDlgNoteEditing = G_MainFrame()->isDlgNoteEditing();

    // new
    QAction* pActionNew = menu.addAction(tr("New..."), this, SLOT(onNewNoteItem()));
    QIcon icon1(QString::fromUtf8(":/res/calendar.png"));
    pActionNew->setIcon(icon1);
    // edit
    QAction* pActionEdit = menu.addAction(tr("Edit..."), this, SLOT(onEditNoteItem()));
    QIcon icon2(QString::fromUtf8(":/res/edit.png"));
    pActionEdit->setIcon(icon2);
    // delete
    QAction* pActionDelete = menu.addAction(tr("Delete"), this, SLOT(onDeleteNoteItem()));
    QIcon icon3(QString::fromUtf8(":/res/delete.png"));
    pActionDelete->setIcon(icon3);
    pActionEdit->setEnabled(selectionModel()->selectedRows().length());
    pActionDelete->setEnabled(selectionModel()->selectedRows().length());

    if(bDlgNoteEditing)
    {
        pActionNew->setEnabled(false);
        pActionEdit->setEnabled(false);
        pActionDelete->setEnabled(false);
    }

    menu.exec(QCursor::pos());
}

void CDCTableWidget::setNoteItemList(const QList<CNoteItem*>& lstNoteItems)
{
    clearContent();

    m_lstNoteItems = lstNoteItems;
    foreach(CNoteItem* pNoteItem, m_lstNoteItems)
    {
        connect(pNoteItem, SIGNAL(dataChanged()), this, SLOT(onNoteItemDataChanged()));
        connect(pNoteItem, SIGNAL(selectedChanged()), this, SLOT(onNoteItemSelectedChanged()));
        connect(pNoteItem, SIGNAL(itemStateChanged()), this, SLOT(onNoteItemStateChanged()));

        int nRowIdx = rowCount();
        insertRow(nRowIdx);

        QTableWidgetItem* pTypeWidgetItem = new QTableWidgetItem(pNoteItem->getIcon(), QString());
        pTypeWidgetItem->setData(Qt::UserRole, (qulonglong)pNoteItem);
        pTypeWidgetItem->setTextAlignment(Qt::AlignCenter);
        setItem(nRowIdx, 0, pTypeWidgetItem);

        if(pNoteItem->isNote())
        {
            pTypeWidgetItem->setData(Qt::ToolTipRole, tr("Note"));

            QTableWidgetItem* pDateWidgetItem = new QTableWidgetItem(pNoteItem->getSetAt().toString(DATE_FORMAT));
            pDateWidgetItem->setData(Qt::ToolTipRole, pNoteItem->getSetAt().toString(DATE_FORMAT));
            setItem(nRowIdx, 1, pDateWidgetItem);
        }
        else
        {
            pTypeWidgetItem->setData(Qt::ToolTipRole, tr("Schedule"));

            QTableWidgetItem* pDateWidgetItem = new QTableWidgetItem(pNoteItem->getSetAt().toString(DATE_TIME_FORMAT));
            pDateWidgetItem->setData(Qt::ToolTipRole, pNoteItem->getSetAt().toString(DATE_TIME_FORMAT));
            setItem(nRowIdx, 1, pDateWidgetItem);
        }

        QTableWidgetItem* pTitleWidgetItem = new QTableWidgetItem(pNoteItem->getTitle());
        pTitleWidgetItem->setData(Qt::ToolTipRole, pNoteItem->getTitle());
        setItem(nRowIdx, 2, pTitleWidgetItem);
    }
}

void CDCTableWidget::onNewNoteItem()
{
    QDate dt;
    CNoteItem* pNoteItem = getSelectedNoteItem();
    if(pNoteItem)
    {
        dt = pNoteItem->getSetAt().date();
    }

    G_MainFrame()->onNew(dt);
}

void CDCTableWidget::onEditNoteItem()
{
    CNoteItem* pNoteItem = getSelectedNoteItem();
    if(!pNoteItem)
        return;

    G_MainFrame()->onEdit(pNoteItem);
}

void CDCTableWidget::onDeleteNoteItem()
{
    CNoteItem* pNoteItem = getSelectedNoteItem();
    if(!pNoteItem)
        return;

    G_MainFrame()->onDelete(pNoteItem);
}

void CDCTableWidget::onNoteItemDataChanged()
{
    CNoteItem* pNoteItem = qobject_cast<CNoteItem*>(sender());
    int nIdx = getRowIndex(pNoteItem);
    if(nIdx >= 0)
    {
        QTableWidgetItem* pWidgetItem0 = item(nIdx, 0);
        QTableWidgetItem* pWidgetItem1 = item(nIdx, 1);
        QTableWidgetItem* pWidgetItem2 = item(nIdx, 2);

        if(pNoteItem->isNote())
        {
            // type
            QIcon icon1;
            icon1.addFile(QString::fromUtf8(":/res/itemnote.png"), QSize(), QIcon::Normal, QIcon::Off);
            pWidgetItem0->setIcon(icon1);

            // set at
            pWidgetItem1->setText(pNoteItem->getSetAt().toString(DATE_FORMAT));
        }
        else
        {
            // type
            QIcon icon1;
            icon1.addFile(QString::fromUtf8(":/res/itemschedule.png"), QSize(), QIcon::Normal, QIcon::Off);
            pWidgetItem0->setIcon(icon1);

            // set at
            pWidgetItem1->setText(pNoteItem->getSetAt().toString(DATE_TIME_FORMAT));
        }

        // title
        pWidgetItem2->setText(pNoteItem->getTitle());
    }
}

void CDCTableWidget::onNoteItemSelectedChanged()
{
    CNoteItem* pNoteItem = qobject_cast<CNoteItem*>(sender());
    if(!pNoteItem)
        return;

    int nIdx = getRowIndex(pNoteItem);
    if(nIdx < 0)
        return;

    if(pNoteItem->isSelected())
    {
        selectNoteItem(pNoteItem);
    }
    else
    {
        unselectNoteItem(pNoteItem);
    }
}

void CDCTableWidget::onNoteItemStateChanged()
{
    CNoteItem* pNoteItem = qobject_cast<CNoteItem*>(sender());
    if(pNoteItem)
    {
        switch(pNoteItem->getState())
        {
        case STATE_DELETE:
            removeNoteItem(pNoteItem);
            break;
        default:
            break;
        }
    }
}

void CDCTableWidget::onItemSelectionChanged()
{
    CNoteItem* pNoteItem = getSelectedNoteItem();
    if(pNoteItem)
    {
        CNoteItem* pSelectedNoteItemLocal = G_MainFrame()->getLocalDbManager()->getSelectedNoteItem();
        if(pSelectedNoteItemLocal != pNoteItem)
        {
            if(pSelectedNoteItemLocal)
                pSelectedNoteItemLocal->setSelected(false);

            pNoteItem->setSelected(true);
        }
    }
}

void CDCTableWidget::onItemDoubleClicked(QTableWidgetItem *item)
{
    if(G_MainFrame()->isDlgNoteEditing())
        return;

    G_MainFrame()->onEdit(getSelectedNoteItem());
}

void CDCTableWidget::removeNoteItem(CNoteItem* pNoteItem)
{
    int nIdx = getRowIndex(pNoteItem);
    if(nIdx >= 0)
    {
        disconnect(pNoteItem, SIGNAL(dataChanged()));
        disconnect(pNoteItem, SIGNAL(selectedChanged()));
        disconnect(pNoteItem, SIGNAL(itemStateChanged()));

        removeRow(nIdx);
        m_lstNoteItems.removeOne(pNoteItem);
    }
}

void CDCTableWidget::clearNoteItems()
{
    m_lstNoteItems.clear();
}

void CDCTableWidget::clearContent()
{
    selectionModel()->clearSelection();
    foreach(CNoteItem* pNoteItem, m_lstNoteItems)
    {
        disconnect(pNoteItem, SIGNAL(dataChanged()));
        disconnect(pNoteItem, SIGNAL(selectedChanged()));
        disconnect(pNoteItem, SIGNAL(itemStateChanged()));
    }

    for(int i = rowCount() - 1; i >= 0; --i)
    {
        removeRow(i);
    }
    m_lstNoteItems.clear();
}

CNoteItem* CDCTableWidget::getSelectedNoteItem()
{
    QModelIndexList lstSelectedIndex =  selectionModel()->selectedRows();
    if(lstSelectedIndex.count())
    {
        QTableWidgetItem* pTypeWidgetItem = item(lstSelectedIndex.at(0).row(), 0);
        if(pTypeWidgetItem)
        {
            return (CNoteItem*)pTypeWidgetItem->data(Qt::UserRole).toULongLong();
        }
    }

    return NULL;
}

int CDCTableWidget::getRowIndex(CNoteItem* pNoteItem)
{
    QTableWidgetItem* pTypeWidgetItem = NULL;
    for(int i = rowCount() - 1; i >= 0; --i)
    {
        pTypeWidgetItem = item(i, 0);
        if(((CNoteItem*)pTypeWidgetItem->data(Qt::UserRole).toULongLong()) == pNoteItem)
        {
            return i;
        }
    }

    return -1;
}

void CDCTableWidget::selectNoteItem(CNoteItem* pNoteItem)
{
    if(!pNoteItem)
    {
        selectionModel()->clearSelection();
        return;
    }

    if(getSelectedNoteItem() == pNoteItem)
        return;

    int nIdx = getRowIndex(pNoteItem);
    if(nIdx >= 0)
    {
        selectionModel()->select(model()->index(nIdx, 0),
                                                 QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);

        scrollTo(model()->index(nIdx, 0));
    }
    else
    {
        selectionModel()->clearSelection();
    }
}

void CDCTableWidget::unselectNoteItem(CNoteItem* pNoteItem)
{
    int nIdx = getRowIndex(pNoteItem);
    if(nIdx >= 0)
    {
        selectionModel()->select(model()->index(nIdx, 0),
                                                 QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
    }
}
