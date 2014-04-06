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

#ifndef DLGPROGRESS_H
#define DLGPROGRESS_H

#include <QWidget>
#include <QTableWidget>
#include <QNetworkReply>
#include "TableProgress.h"
#include "FileDownload.h"
#include "AttachmentItem.h"

struct NETWORK_ITEM{
    NETWORK_ITEM(){
        m_pNetworkReply = NULL;
        m_pFileDownload = NULL;
    }

    void abortNetworkReply()
    {
        if(m_pNetworkReply)
        {
  //          m_pNetworkReply->abort();
            m_pNetworkReply->deleteLater();
            m_pNetworkReply = NULL;
        }
        else if(m_pFileDownload)
        {
            m_pFileDownload->enforceCancel();
            m_pFileDownload = NULL;
        }
    }

    QNetworkReply* m_pNetworkReply;
    CFileDownload* m_pFileDownload;
};

class CDlgProgress : public QWidget
{
    Q_OBJECT
public:
    explicit CDlgProgress(QWidget *parent = 0);

    void addNetworkItem(NETWORK_ITEM* pNetworkItem, const QString& strDesc);
    bool hasNetworkItemWorking();

    int findItemPointer(qulonglong nItemPointer);
    void removeItemPointer(qulonglong nItemPointer);
    void addMemo(qulonglong nItemPointer, QString strErr);
    void clearEndedNetworkReply();
    void clearContent();

    void removeRow(int nRowIdx, bool bCancel = true);
    void removeItemPointer(qlonglong nPointer);

signals:
    
public slots:
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onNetworkReplyFinished();
    void onNetworkReplyError(QNetworkReply::NetworkError err);
    void onFileDownloadFinished(CAttachmentItem *, const QString &str);

private:
    CTableProgress* m_pTableWidget;
    
};

#endif // DLGPROGRESS_H
