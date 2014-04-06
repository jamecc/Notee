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

#include <QFileInfo>
#include <QMessageBox>
#include <QMimeData>
#include <QAction>
#include <QDragEnterEvent>
#include "AttachmentListWidget.h"
#include "CommonFunctions.h"
#include "NoteItem.h"
#include <QMenu>
#include <QUrl>

CAttachmentListWidget::CAttachmentListWidget(QWidget *parent)
    : QListWidget(parent)
{
    setViewMode(QListView::IconMode);
    setAcceptDrops(true);
    setMouseTracking(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setViewMode(QListView::ListMode);
    setFlow(QListView::LeftToRight);
     setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onContextMenu(const QPoint&)));
}

CAttachmentItem* CAttachmentListWidget::getAttachmentItemByPathFileName(const QString& strPathFileName)
{
    CAttachmentItem* pAttachItem = NULL;

    for(int i = count() - 1; i >= 0; --i)
    {
        pAttachItem = (CAttachmentItem*)(item(i)->data(Qt::UserRole).toLongLong());
        if(pAttachItem->getFileName() == strPathFileName)
            return pAttachItem;
    }

    return NULL;
}

CAttachmentItem* CAttachmentListWidget::getAttachmentItemByFileNameDisplay(const QString& strFileNameDisplay)
{
    CAttachmentItem* pAttachItem = NULL;

    for(int i = count() - 1; i >= 0; --i)
    {
        pAttachItem = (CAttachmentItem*)(item(i)->data(Qt::UserRole).toLongLong());
        if(pAttachItem->getFileNameDisplay() == strFileNameDisplay)
            return pAttachItem;
    }

    return NULL;
}

bool CAttachmentListWidget::addAttachmentItem(CAttachmentItem* pItem)
{
    /*
    if(getAttachmentItemByPathFileName(pItem->getFileName()))
    {
        QMessageBox::critical(this, tr("Repeated attachment"), tr("The attachment has been added."));
        return false;
    }

    while(getAttachmentItemByFileNameDisplay(pItem->getFileNameDisplay()))
    {
        QFileInfo fileInfo(pItem->getFileNameDisplay());
        QString strSuffix = fileInfo.suffix();
        if(strSuffix.length())
            strSuffix = "." + strSuffix;
        QString strFileName = pItem->getFileNameDisplay();
        QString strPrefix = strFileName.left(strFileName.length() - strSuffix.length());
        int nPos = strPrefix.length() - 1;
        while(nPos >= 0 && strPrefix.at(nPos).isDigit())
        {
            nPos--;
        }

        if(nPos > 0 && nPos < (strPrefix.length() - 1) && strPrefix.at(nPos) == '_')
        {
            pItem->setFileNameDisplay(strPrefix.left(strPrefix.length() - nPos - 1) + "_" + QString::number(strPrefix.mid(nPos).toInt() + 1) + strSuffix);
        }
        else
        {
            pItem->setFileNameDisplay(strPrefix + "_" + QString::number(1) + strSuffix);
        }
    }
    */

    QListWidgetItem* pWidgetItem = new QListWidgetItem(this);
    pWidgetItem->setText(pItem->getFileNameDisplay() + "(" + G_getFileSize(pItem->getContentSize()) + ")");
    pWidgetItem->setData(Qt::UserRole, QVariant::fromValue(qlonglong(pItem)));
    QIcon iconFile(QString::fromUtf8(":/res/attachment.png"));
    pWidgetItem->setIcon(iconFile);
    addItem(pWidgetItem);
    return true;
}

void CAttachmentListWidget::delAttachmentItem(CAttachmentItem* pItem)
{
    QList<QListWidgetItem*> lstWidgetItems;
    if(pItem)
    {
        QListWidgetItem* pWidgetItem = getWidgetItem(pItem);
        if(pWidgetItem)
            lstWidgetItems.push_back(pWidgetItem);
    }
    else
    {
        lstWidgetItems = selectedItems();
    }

    CAttachmentItem* pAttachmentItem = NULL;
    foreach(QListWidgetItem* pWidgetItem, lstWidgetItems)
    {
        pAttachmentItem = (CAttachmentItem*)pWidgetItem->data(Qt::UserRole).toLongLong();
        if(!pAttachmentItem)
            continue;

        pAttachmentItem->setState(STATE_DELETE);
        removeItemWidget(pWidgetItem);
        delete pWidgetItem, pWidgetItem = NULL;
    }
}

QList<CAttachmentItem *> CAttachmentListWidget::getSelectedAttachments()
{
    QList<CAttachmentItem*> lstAttachmentItems;
    if(selectedItems().length() > 0)
    {
        CAttachmentItem* pItem = NULL;
        for(int i = 0; i < selectedItems().length(); ++i)
        {
            pItem = (CAttachmentItem*)selectedItems().at(i)->data(Qt::UserRole).toLongLong();
            if(!pItem)
                continue;

            lstAttachmentItems.push_back(pItem);
        }
    }

    return lstAttachmentItems;
}

QListWidgetItem* CAttachmentListWidget::getWidgetItem(CAttachmentItem* pItem)
{
    CAttachmentItem* pAttachItem = NULL;

    for(int i = count() - 1; i >= 0; --i)
    {
        pAttachItem = (CAttachmentItem*)(itemAt(i, 0)->data(Qt::UserRole).toLongLong());
        if(pAttachItem == pItem)
            return itemAt(i, 0);
    }

    return NULL;
}

void CAttachmentListWidget::onContextMenu(const QPoint& pt)
{
    QMenu menu(this);

    // add
    QAction* pActionAdd = menu.addAction(tr("Add..."), this, SIGNAL(emitAdd()));
    QIcon icon1(QString::fromUtf8(":/res/attachment.png"));
    pActionAdd->setIcon(icon1);
    // delete
    QAction* pActionDelete = menu.addAction(tr("Delete"), this, SIGNAL(emitDelete()));
    QIcon icon2(QString::fromUtf8(":/res/attachment_del.png"));
    pActionDelete->setIcon(icon2);
    // save as
    QAction* pActionSaveAs = menu.addAction(tr("Save as..."), this, SIGNAL(emitSaveAs()));
    QIcon icon3(QString::fromUtf8(":/res/attachment_save.png"));
    pActionSaveAs->setIcon(icon3);
    pActionDelete->setEnabled(selectionModel()->selectedRows().length());
    pActionSaveAs->setEnabled(selectionModel()->selectedRows().length());

    menu.exec(QCursor::pos());
}

void CAttachmentListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        foreach (QUrl url, event->mimeData()->urls())
        {
            QFileInfo info(url.toLocalFile());
            if(info.isFile())
            {
                event->acceptProposedAction();
                break;
            }
        }
    }
}

void CAttachmentListWidget::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasUrls())
    {
        foreach (QUrl url, mimeData->urls())
        {
            QFileInfo info(url.toLocalFile());
            if(info.isFile())
            {
                emit addAttachment(info.filePath());
            }
        }
        event->acceptProposedAction();
    }
}

void CAttachmentListWidget::keyPressEvent(QKeyEvent * event)
{
    if(Qt::Key_Delete == event->key())
    {
        emit emitDelete();
    }

    QWidget::keyPressEvent(event);
}
