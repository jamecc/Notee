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

#include "DlgProgress.h"
#include <QHBoxLayout>
#include <QProgressBar>
#include <QApplication>
#include "FileDownload.h"


CDlgProgress::CDlgProgress(QWidget *parent) :
    QWidget(parent)
{
    m_pTableWidget = new CTableProgress(this);

    QHBoxLayout* pMainLayout = new QHBoxLayout;
    pMainLayout->addWidget(m_pTableWidget);
    pMainLayout->setContentsMargins(0,0,0,0);
    setLayout(pMainLayout);
}

void CDlgProgress::addNetworkItem(NETWORK_ITEM* pNetworkItem, const QString& strDesc)
{
    if(!pNetworkItem)
        return;

    // desc
    m_pTableWidget->insertRow(0);
    QTableWidgetItem* pWidgetItem = new QTableWidgetItem(strDesc);
    pWidgetItem->setData(Qt::UserRole, QVariant::fromValue((qulonglong)pNetworkItem));
    m_pTableWidget->setItem(0, 0, pWidgetItem);
    // upload
    QProgressBar* pProgressBar = new QProgressBar(m_pTableWidget);
 //   pProgressBar->setTextVisible(true);
    pProgressBar->setRange(0, 100);
    pProgressBar->setValue(0);
    m_pTableWidget->setCellWidget(0, 1, pProgressBar);
    // download
    QProgressBar* pProgressBar2 = new QProgressBar(m_pTableWidget);
//    pProgressBar2->setTextVisible(true);
    pProgressBar2->setRange(0, 100);
    pProgressBar2->setValue(0);
    m_pTableWidget->setCellWidget(0, 2, pProgressBar2);

    pWidgetItem = new QTableWidgetItem();
    m_pTableWidget->setItem(0, 3, pWidgetItem);

    // connect
    if(pNetworkItem->m_pNetworkReply)
    {
        connect(pNetworkItem->m_pNetworkReply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(onUploadProgress(qint64,qint64)));
        connect(pNetworkItem->m_pNetworkReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
        connect(pNetworkItem->m_pNetworkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onNetworkReplyFinished()));
        connect(pNetworkItem->m_pNetworkReply, SIGNAL(finished()), this, SLOT(onNetworkReplyFinished()));
    }
    else if(pNetworkItem->m_pFileDownload)
    {
        connect(pNetworkItem->m_pFileDownload, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(onUploadProgress(qint64,qint64)));
        connect(pNetworkItem->m_pFileDownload, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));
        connect(pNetworkItem->m_pFileDownload, SIGNAL(finishedDownloadAttachmentItem(CAttachmentItem*,QString)), this, SLOT(onFileDownloadFinished(CAttachmentItem*,QString)));
    }
}

bool CDlgProgress::hasNetworkItemWorking()
{
    return m_pTableWidget->rowCount() != 0;
}

int CDlgProgress::findItemPointer(qulonglong nItemPointer)
{
    QTableWidgetItem* pWidgetItem = NULL;
    for(int i = m_pTableWidget->rowCount() - 1; i >= 0; i--)
    {
        pWidgetItem = m_pTableWidget->item(i, 0);
        NETWORK_ITEM* pNetworkItem = (NETWORK_ITEM*)pWidgetItem->data(Qt::UserRole).toULongLong();
        if(pNetworkItem && (((qulonglong)pNetworkItem->m_pFileDownload) == nItemPointer ||
                            (pNetworkItem->m_pFileDownload &&
                             (((qulonglong)pNetworkItem->m_pFileDownload->getNetworkReply()) == nItemPointer ||
                             ((qulonglong)pNetworkItem->m_pFileDownload->getAttachmentItem()) == nItemPointer)) ||
                            ((qulonglong)pNetworkItem->m_pNetworkReply) == nItemPointer))
        {
            return i;
        }
    }

    /*
    //void* pVoid = (void*) nItemPointer;
    for(int i = m_pTableWidget->rowCount() - 1; i >= 0; i--)
    {
        pWidgetItem = m_pTableWidget->item(i, 0);
        NETWORK_ITEM* pNetworkItem = (NETWORK_ITEM*)pWidgetItem->data(Qt::UserRole).toULongLong();
        if(pNetworkItem && (((qulonglong)pNetworkItem->m_pFileDownload) == nItemPointer ||
                            (pNetworkItem->m_pFileDownload &&
                             ((qulonglong)pNetworkItem->m_pFileDownload->getNetworkReply()) == nItemPointer) ||
                            ((qulonglong)pNetworkItem->m_pNetworkReply) == nItemPointer))
        {
            return i;
        }
    }
    */

    return -1;
}

void CDlgProgress::removeItemPointer(qulonglong nItemPointer)
{
    return;
    int nRowIdx = findItemPointer(nItemPointer);
    if(nRowIdx < 0)
        return;

    removeRow(nRowIdx);
}

void CDlgProgress::addMemo(qulonglong nItemPointer, QString strErr)
{
    int nRowIdx = findItemPointer(nItemPointer);
    if(nRowIdx < 0)
        return;

    QTableWidgetItem* pWidgetItem = m_pTableWidget->item(nRowIdx,3);
    pWidgetItem->setText(strErr);
    pWidgetItem->setData(Qt::ToolTipRole, strErr);
}

void CDlgProgress::clearEndedNetworkReply()
{
    QProgressBar* pProgressBar = NULL;
    for(int i = m_pTableWidget->rowCount() - 1; i >= 0; i--)
    {
        // upload
        pProgressBar = qobject_cast<QProgressBar*>(m_pTableWidget->cellWidget(1, i));
        if(pProgressBar->value() != 100)
            continue;

        // download
        pProgressBar = qobject_cast<QProgressBar*>(m_pTableWidget->cellWidget(2, i));
        if(pProgressBar->value() != 100)
            continue;

        removeRow(i);
    }
}

void CDlgProgress::clearContent()
{
    for(int i = m_pTableWidget->rowCount() - 1; i >= 0; i--)
    {
        removeRow(i);
    }
}

void CDlgProgress::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    qulonglong nSenderPointer = (qulonglong)(sender());
    if(!nSenderPointer)
        return;

    int i = findItemPointer(nSenderPointer);
    if(i < 0)
        return;

    // upload
    QWidget* pWidget = m_pTableWidget->cellWidget(i, 1);
    QProgressBar* pProgressBar = qobject_cast<QProgressBar*>(pWidget);
    if(!pProgressBar)
        return;

    if(bytesTotal == 0)
        pProgressBar->setValue(0);
    else
        pProgressBar->setValue(bytesSent * 100 / bytesTotal);
}

void CDlgProgress::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    //QObject* pSender = sender();
    qulonglong nSenderPointer = (qulonglong)(sender());
    if(!nSenderPointer)
        return;

    int i = findItemPointer(nSenderPointer);
    if(i < 0)
        return;

    // download
    QWidget* pWidget = m_pTableWidget->cellWidget(i, 2);
    QProgressBar* pProgressBar = qobject_cast<QProgressBar*>(pWidget);
    if(!pProgressBar)
        return;

    if(bytesTotal == 0)
        pProgressBar->setValue(0);
    else
        pProgressBar->setValue(bytesReceived * 100 / bytesTotal);

//    if(pProgressBar->value() == 100)
//        removeRow(i, false);
}

void CDlgProgress::removeItemPointer(qlonglong nPointer)
{
    int i = findItemPointer(nPointer);
    if(i < 0)
        return;

    removeRow(i, false);
}

void CDlgProgress::onNetworkReplyFinished()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());
    if(!pNetworkReply)
        return;

    pNetworkReply->deleteLater();
    if(pNetworkReply->error() > 0)
    {
        addMemo((qlonglong)pNetworkReply, tr("Network Error: ") + pNetworkReply->errorString());
        return;
    }

    removeItemPointer((qlonglong)pNetworkReply);
}

void CDlgProgress::onNetworkReplyError(QNetworkReply::NetworkError err)
{

}

void CDlgProgress::onFileDownloadFinished(CAttachmentItem*,const QString& str)
{
    CFileDownload* pFileDownload = qobject_cast<CFileDownload*>(sender());
    if(!pFileDownload)
        return;

    pFileDownload->deleteLater();

    if(str.length() > 0)
    {
        addMemo((qlonglong)pFileDownload, str);
        return;
    }

    removeItemPointer((qlonglong)pFileDownload);
}

void CDlgProgress::removeRow(int nRowIdx, bool bCancel)
{
    if(nRowIdx < 0 || nRowIdx >= m_pTableWidget->rowCount())
        return;


    QTableWidgetItem* pWidgetItem = m_pTableWidget->item(nRowIdx,0);
    NETWORK_ITEM* pNetworkItem = (NETWORK_ITEM*)pWidgetItem->data(Qt::UserRole).toLongLong();
    if(pNetworkItem)
    {
        if(bCancel)
        {
            pNetworkItem->abortNetworkReply();
        }

        delete pNetworkItem, pNetworkItem = NULL;
    }


    m_pTableWidget->removeRow(nRowIdx);
    m_pTableWidget->update();

    QApplication::processEvents();
}
