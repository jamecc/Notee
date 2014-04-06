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

#include "FileDownload.h"
#include <QNetworkAccessManager>
#include "AttachmentItem.h"
#include <QDir>

CFileDownload::CFileDownload(QNetworkAccessManager* pNetworkMgr, CAttachmentItem* pAttachmentItem, QObject *parent) :
    QObject(parent)
{
    m_pNetworkMgr = pNetworkMgr;
    m_pAttachmentItem = pAttachmentItem;
    m_pNetworkReply = NULL;
    m_bAborted = false;
    m_bPaused = false;
    m_bOverWrite = true;
    m_bAutoRemoveFile = false;
    m_pFile = NULL;
    if(m_pAttachmentItem)
        m_pAttachmentItem->setFileDownload(this);
    m_nFileSize = 0;
}

CFileDownload::~CFileDownload()
{
    if(m_pNetworkReply)
        m_pNetworkReply->deleteLater();
    if(m_pFile)
        delete m_pFile, m_pFile = NULL;
}

void CFileDownload::enforceCancel(bool bSetNULL)
{
    if(!m_pNetworkMgr || !m_pNetworkReply)
        return;

 //   m_pNetworkReply->abort();
    if(bSetNULL && getAttachmentItem())
        getAttachmentItem()->setFileDownload(NULL);
 //   m_pNetworkReply->deleteLater();
    m_pNetworkReply = NULL;
    m_pAttachmentItem = NULL;
    m_bAborted = true;
}

void CFileDownload::download(const QString& strUrl, const QString& strFolder, bool bOverWrite, bool bAutoRemoveFile)
{
    m_strFolder = strFolder;
    m_strUrl = strUrl;
    m_bOverWrite = bOverWrite;
    m_bAutoRemoveFile = bAutoRemoveFile;

    QUrl url;
    url.setEncodedUrl(m_strUrl.toAscii());
    if (!url.isValid())
    {
        emit finishedDownloadAttachmentItem(m_pAttachmentItem, tr("Error: Bad file URL"));
        return;
    }

    if(m_pAttachmentItem)
        m_pAttachmentItem->setProcessState(STATE_PS_UPLOADING);

    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)");
 //   request.setRawHeader("User-Agent", "Wget");
 //   request.setRawHeader("Accept", "*/*");
  //  request.setRawHeader("Connection", "Keep-Alive");
//    request.setRawHeader("Keep-Alive", "300");

    m_pNetworkReply = m_pNetworkMgr->get(request);
    connect(m_pNetworkReply, SIGNAL(metaDataChanged()), this, SLOT(onMetaDataChanged()));
    connect(m_pNetworkReply, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(m_pNetworkReply, SIGNAL(uploadProgress(qint64, qint64)), this, SIGNAL(uploadProgress(qint64, qint64)));
    connect(m_pNetworkReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(onDownloadProgress(qint64,qint64)));

    connect(m_pNetworkReply, SIGNAL(finished()), this, SLOT(onFinished()));
}

void CFileDownload::onMetaDataChanged()
{
    if(!m_pNetworkReply)
        return;

    if (m_pNetworkReply->error()>0)
    {
        m_strError = tr("Network error: %1").arg(QString::number(m_pNetworkReply->error()));
        return;
    }

    int statuscode = m_pNetworkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    // redirect location
    if (statuscode >= 300 && statuscode <= 399)
    {
        m_strRedirectLocation = QString(m_pNetworkReply->rawHeader(QString("location").toAscii()));
        m_pNetworkReply->abort();
        return;
    }

    // 400 Bad Request, 403 Forbidden, 404 Not Found
    if (statuscode >= 400 && statuscode <= 499)
    {
        switch(statuscode)
        {
        case 400:
            m_strError = tr("Client error: bad request(%1)").arg(statuscode);
            break;
        case 403:
            m_strError = tr("Client error: forbidden(%1)").arg(statuscode);
            break;
        case 404:
            m_strError = tr("Client error: not found(%1)").arg(statuscode);
            break;
        default:
            m_strError = tr("Client error: status code(%1)").arg(statuscode);
            break;
        }

        m_pNetworkReply->abort();
        return;
    }

    // 500 Internal Server Error, 503 Server Unavailable
    if (statuscode >= 500 && statuscode <= 599)
    {
        switch(statuscode)
        {
        case 500:
            m_strError = tr("Client error: internal server(%1)").arg(statuscode);
            break;
        case 503:
            m_strError = tr("Client error: server unavailable(%1)").arg(statuscode);
            break;
        default:
            m_strError = tr("Client error: status code(%1)").arg(statuscode);
            break;
        }
        m_pNetworkReply->abort();
        return;
    }

    if(m_pAttachmentItem)
        m_pAttachmentItem->setProcessState(STATE_PS_DOWNLOADING);

    if (m_pNetworkReply->hasRawHeader(QString("Content-Disposition").toAscii()))
    {
        QString inf = m_pNetworkReply->rawHeader(QString("Content-Disposition").toAscii());
        int startpos = inf.indexOf("filename=", 0, Qt::CaseInsensitive);
        if (startpos>=0)
        {
            int len = inf.length()-startpos-QString("filename=").length();
            if (len>0)
            {
                m_strFileNameDownload = inf.right(len);
                m_strFileNameDownload.remove(QChar('\"'));
            }
        }
    }

    if (m_pNetworkReply->hasRawHeader(QString("Content-Md5").toAscii()))
    {
        m_strMd5 = m_pNetworkReply->rawHeader(QString("Content-Md5").toAscii());
    }

    if (m_pNetworkReply->hasRawHeader(QString("Content-Length2").toAscii()))
    {
        m_nFileSize = m_pNetworkReply->rawHeader(QString("Content-Length2").toAscii()).toLongLong();
    }

    if(m_strFolder.isEmpty())
        m_strFolder = QDir::tempPath();

    if (m_strFolder.right(1) != QDir::separator())
        m_strFolder.append(QDir::separator());

    qint64 size = m_pNetworkReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();

    // temp file name
    QString strDateTime = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString strFileName = m_strFileNameDownload;

    int i = 1;
    while (QFile::exists(m_strFolder + strDateTime + strFileName + ".part"))
    {
        strFileName = QString("%1_%2").arg(m_strFileNameDownload).arg(i);
        i++;
    }

    QString strPathFileName = m_strFolder + strDateTime + strFileName + ".part";
//    emit started(strPathFileName, size, m_strUrl);
    m_pFile = new QFile(strPathFileName);
    if (!m_pFile->open(QIODevice::ReadWrite))
    {
        m_strError = tr("File error: Can't create file \"%1\"").arg(strPathFileName);
        m_pNetworkReply->abort();
        return;
    }

    m_strPathFileName = m_pFile->fileName();
}

void CFileDownload::onReadyRead()
{
    if (m_pNetworkReply->error() > 0)
    {
        m_strError = tr("Network error: %1").arg(QString::number(m_pNetworkReply->error()));
        m_pNetworkReply->abort();
        return;
    }

    QByteArray bytes = m_pNetworkReply->readAll();
    if (m_pFile->write(bytes) < 0)
    {
        m_strError = tr("File error: error writing file %1").arg(m_pFile->fileName());
        m_pNetworkReply->abort();
        return;
    }
}

void CFileDownload::onFinished()
{
    if (!m_strRedirectLocation.isEmpty())
    {
        download(m_strRedirectLocation, m_strFolder, m_bOverWrite);
        return;
    }

    if(m_pAttachmentItem)
        m_pAttachmentItem->setProcessState(STATE_PS_NORMAL);

    if (m_pNetworkReply->error() > 0 && m_strError.isEmpty())
    {
        m_strError = tr("Network error: %1").arg(m_pNetworkReply->errorString());
    }

    if (m_pFile)
    {
        if ((m_bAborted || (!m_strError.isEmpty())) && (!m_bPaused))
        {
            m_pFile->close();
            m_pFile->remove();
        }
        else
        {
            if(m_pAttachmentItem)
            {
                m_pFile->flush();
                m_pFile->close();
                m_pFile->open(QIODevice::ReadOnly);
                m_pAttachmentItem->setContent(m_pFile->readAll());
                m_pAttachmentItem->setMd5(m_strMd5);
                m_pAttachmentItem->setFileName(m_strFileNameDownload);
                emit finishedDownloadAttachmentItem(m_pAttachmentItem, m_strError);
            }

            m_pFile->close();
            if(m_bAutoRemoveFile)
                m_pFile->remove();
            else
            {
                QString strFileName = m_strFileNameDownload;
                if(strFileName.isEmpty())
                    strFileName = "nofilename";

                QString strFileNameT = strFileName;
                if(!m_bOverWrite)
                {
                    QFileInfo fileInfo(strFileName);
                    QString strFileNameSuffix = fileInfo.suffix();
                    QString strFileNameTitle;
                    if(strFileNameSuffix.length() > 0)
                        strFileNameTitle = strFileName.left(strFileName.length() - strFileNameSuffix.length() - 1);
                    else
                        strFileNameTitle = strFileName;
                    int i = 1;
                    while(QFile::exists(m_strFolder + strFileNameT))
                    {
                        if(strFileNameSuffix.length() > 0)
                            strFileNameT = QString("%1_%2.%3").arg(strFileNameTitle).arg(i).arg(strFileNameSuffix);
                        else
                            strFileNameT = QString("%1_%2").arg(strFileNameTitle).arg(i);
                    }
                }


                if(!m_pFile->rename(strFileNameT))
                {
                    m_strError = tr("File error: fail to rename \"%1\"").arg(strFileNameT);
                }

                m_strPathFileName = m_pFile->fileName();
            }
        }
        delete m_pFile, m_pFile = NULL;
    }

    if(!m_strError.isEmpty())
    {
        emit finishedDownloadAttachmentItem(m_pAttachmentItem, m_strError);
    }
 //   emit finished(m_strError);
}

void CFileDownload::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, m_nFileSize);
}
