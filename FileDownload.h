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

#ifndef FILEDOWNLOAD_H
#define FILEDOWNLOAD_H

#include <QObject>
#include <QNetworkReply>
#include <QFile>

class QNetworkAccessManager;
class CAttachmentItem;
class CFileDownload : public QObject
{
    Q_OBJECT
public:
    explicit CFileDownload(QNetworkAccessManager* pNetworkMgr, CAttachmentItem* pAttachmentItem, QObject *parent = 0);
    virtual ~CFileDownload();

    QNetworkReply* getNetworkReply() const      {return m_pNetworkReply;}
    CAttachmentItem* getAttachmentItem() const  {return m_pAttachmentItem;}

    void download(const QString& strUrl, const QString& strFolder = "", bool bOverWrite = true, bool bAutoRemoveFile = true);
    void enforceCancel(bool bSetNULL = true);
    QString getErrorString() const {return m_strError;}
    
signals:
  //  void started(const QString &filename, qint64 size, const QString &fileurl);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
  //  void finished(const QString& error);
    void finishedDownloadAttachmentItem(CAttachmentItem*, QString);
  //  void errorFinishedDownloadAttachmentItem(CAttachmentItem*, QString);
    
public slots:
    void onMetaDataChanged();
    void onReadyRead();
    void onFinished();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QNetworkAccessManager* m_pNetworkMgr;
    CAttachmentItem* m_pAttachmentItem;
    QNetworkReply* m_pNetworkReply;
    QString m_strPathFileName;
    QString m_strFileNameDownload;
    QString m_strRedirectLocation;
    QString m_strFolder;
    QString m_strUrl;
    QString m_strError;
    QString m_strMd5;
    QFile* m_pFile;
    qlonglong m_nFileSize;
    bool m_bAborted;
    bool m_bPaused;
    bool m_bOverWrite;
    bool m_bAutoRemoveFile;
};

#endif // FILEDOWNLOAD_H
