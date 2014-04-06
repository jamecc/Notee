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

#include <QNetworkRequest>
#include <QDateTime>
#include <QTextDocument>
#include <QDomDocument>
#include <QHttpMultiPart>
#include <QDebug>
#include "predefine.h"
#include "CommonFunctions.h"
#include "NoteItem.h"
#include "AttachmentItem.h"
#include "NetworkManager.h"
#include "MainFrame.h"
#include "FileUpload.h"
#include "FileDownload.h"

//#define URL_BASE    "http://localhost/notee_web/remote/"
#define URL_BASE        "http://notee.coodesoft.com/remote/"
#define URL_PAD_NOTEE   "http://www.coodesoft.com/downloads/pad_notee.xml"

bool parseJsonItem2AttachmentItem(CJsonItem* pAttachmentItems, QList<CAttachmentItem*>& lstAttachmentItems);

CNetworkManager::CNetworkManager(QObject *parent)
    : QObject(parent)
{
    m_nActionType = ACTION_IDLE;
    m_nActionTypeLast = ACTION_IDLE;
    m_pNetworkAccessManager = new QNetworkAccessManager(this);

 //   connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(finished(QNetworkReply*)));
    connect(m_pNetworkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReplyFinished(QNetworkReply*)));
    m_bGotAmountOfNotes = false;
    m_bGotNoteList = false;
    m_nAmountOfNotes = -1;
    m_nAmountOfItems = -1;
}

CNetworkManager::~CNetworkManager()
{
    clearNoteItems();
}

void CNetworkManager::replyConnect(QNetworkReply* pNetworkReply)
{
    connect(pNetworkReply, SIGNAL(uploadProgress(qint64,qint64)), this, SIGNAL(uploadProgress(qint64,qint64)));
    connect(pNetworkReply, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(downloadProgress(qint64,qint64)));
//    connect(pNetworkReply, SIGNAL(error(QNetworkReply::NetworkError)),this, SIGNAL(replyError(QNetworkReply::NetworkError)));
}

void CNetworkManager::register_user(QString strName, QString strPwd, QString strEmail)
{
    // name
    // pwd
    // email
    // login_from
    // ip
    m_nActionType = ACTION_REGISTER;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"name\""));
    textPart1.setBody(strName.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"email\""));
    textPart2.setBody(strEmail.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"pwd\""));
    textPart3.setBody(strPwd.toUtf8());

    QHttpPart textPart4;
    textPart4.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_from\""));
    textPart4.setBody(QString(LOGIN_FROM).toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);
    multiPart->append(textPart4);

    postRequest(QString(URL_BASE) + URL_REGISTER, multiPart, tr("Register"));
 //   emit started();
}

QNetworkReply* CNetworkManager::postRequest(QString strUrl, QHttpMultiPart* pHttpPart, QString strAction)
{
    QUrl url(strUrl);
    QNetworkRequest request(url);
 //   request.setRawHeader("Connection", "keep-alive");
    request.setRawHeader("Connection", "close");
 //   request.setRawHeader("Keep-Alive", "300");
 //   request.setRawHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)");

    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)");
 //   request.setRawHeader("Accept-Encoding", "gzip,deflate");
 //   request.setRawHeader("Accept-Charset", "windows-1251,utf-8;q=0.7,*;q=0.7");

    QNetworkReply* pNetworkReply = m_pNetworkAccessManager->post(request, pHttpPart);
    pHttpPart->setParent(pNetworkReply); // delete the multiPart with the reply

    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " Network reply is open:" << pNetworkReply->isOpen() << "\r\n";
    qDebug() << "is running:" << pNetworkReply->isRunning() << "\r\n";

    G_MainFrame()->addNetworkReply(pNetworkReply, strAction);

    replyConnect(pNetworkReply);
    return pNetworkReply;
}

void CNetworkManager::login(QString strName, QString strPwd)
{
    // name
    // pwd
    // login_from
    // ip
    // lang

    m_nActionType = ACTION_LOGIN;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"name\""));
    textPart1.setBody(strName.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"pwd\""));
    textPart2.setBody(strPwd.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_from\""));
    textPart3.setBody(QString(LOGIN_FROM).toUtf8());

    QHttpPart textPart4;
    textPart4.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"lang\""));
    textPart4.setBody(G_Settings()->getLang().toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);
    multiPart->append(textPart4);

    postRequest(QString(URL_BASE) + URL_LOGIN, multiPart, tr("Login"));
  //  emit started();
}

void CNetworkManager::forgotPwd(QString strUserName)
{
    // user_name
    m_nActionType = ACTION_FORGOT_PWD;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_name\""));
    textPart1.setBody(strUserName.toUtf8());

    multiPart->append(textPart1);

    postRequest(QString(URL_BASE) + URL_FORGOT_PWD, multiPart, tr("Forgot password"));
}

const QDate& CNetworkManager::getCurrentMonthFromServer() const
{
    return m_dtCurrentMonthServer;
}

void CNetworkManager::getAmountOfNotesFromServer()
{
    // user_id
    // login_token

    m_nActionType = ACTION_GET_AMOUNT_OF_NOTES;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart1.setBody(m_strUserId.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart2.setBody(m_strLoginToken.toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);

    postRequest(QString(URL_BASE) + URL_GET_AMOUNT_OF_NOTES, multiPart, tr("Get amount of notes"));
}

void CNetworkManager::getNoteItemListMonth(int nYear, int nMonth)
{
    // user_id
    // login_token

    // time_year
    // time_year
    m_nActionType = ACTION_GET_NOTE_LIST_MONTH;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart1.setBody(m_strUserId.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart2.setBody(m_strLoginToken.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"time_year\""));
    textPart3.setBody(QString::number(nYear).toUtf8());

    QHttpPart textPart4;
    textPart4.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"time_month\""));
    textPart4.setBody(QString::number(nMonth).toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);
    multiPart->append(textPart4);

    postRequest(QString(URL_BASE) + URL_GET_NOTE_LIST_MONTH, multiPart, tr("Get note list"));
}

void CNetworkManager::getNoteItemListByOffset(int nOffset, int nNum)
{
    // user_id
    // login_token

    // offset
    // number
    m_nActionType = ACTION_GET_NOTE_LIST_OFFSET;

    clearNoteItems();

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart1.setBody(m_strUserId.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart2.setBody(m_strLoginToken.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"offset\""));
    textPart3.setBody(QString::number(nOffset).toUtf8());

    QHttpPart textPart4;
    textPart4.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"number\""));
    textPart4.setBody(QString::number(nNum).toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);
    multiPart->append(textPart4);

    postRequest(QString(URL_BASE) + URL_GET_NOTE_LIST_OFFSET, multiPart, tr("Get note list"));
}

/*
void CNetworkManager::getNoteItemList(const QDateTime& dtFrom, const QDateTime& dtTo)
{
    // user_id
    // login_token

    // time from
    // time end
    m_nActionType = ACTION_GET_NOTE_LIST;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart1.setBody(m_strUserId.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart2.setBody(m_strLoginToken.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"time_from\""));
    textPart3.setBody(dtFrom.toString(DATE_TIME_FORMAT).toUtf8());

    QHttpPart textPart4;
    textPart4.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"time_end\""));
    textPart4.setBody(dtTo.toString(DATE_TIME_FORMAT).toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);
    multiPart->append(textPart4);

    postRequest(QString(URL_BASE) + URL_GET_NOTE_ITEM, multiPart, tr("Get note list"));
}
*/

void CNetworkManager::getNoteItem(int nNoteId)
{
    // user_id
    // login_token

    // note_id
    m_nActionType = ACTION_GET_NOTE_ITEM;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart1.setBody(m_strUserId.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart2.setBody(m_strLoginToken.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"note_id\""));
    textPart3.setBody(QString::number(nNoteId).toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);

    postRequest(QString(URL_BASE) + URL_GET_NOTE_ITEM, multiPart, tr("Get note: \"") + QString::number(nNoteId) + "\"");
}

void CNetworkManager::insertNoteItem(CNoteItem* pNoteItem, bool bUpdate/* = false*/)
{
    // user_id
    // login_token

    // title
    // share_type
    // set_at
    // note_id | 0
    // note_local_id
    // type
    // content
    // content_size
    // md5

    // attachment
    // attachment_file_names[], attachment_content_sizes[], attachment_md5s[],
    // attachment_local_ids[]
    // attachment_ids[]
    QString strProgress;
    if(bUpdate)
    {
        m_nActionType = ACTION_UPDATE_NOTE_ITEM;
        strProgress = tr("Update note: \"") + pNoteItem->getTitle() + "\"";
    }
    else
    {
        m_nActionType = ACTION_INSERT_NOTE_ITEM;
        strProgress = tr("Insert note: \"") + pNoteItem->getTitle() + "\"";
    }

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart1.setBody(m_strUserId.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart2.setBody(m_strLoginToken.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"note_local_id\""));
    textPart3.setBody(QString::number(pNoteItem->getLocalId()).toUtf8());

    QHttpPart textPart4;
    textPart4.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"title\""));
    textPart4.setBody(pNoteItem->getTitle().toUtf8());

    QHttpPart textPart5;
    textPart5.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"share_type\""));
    textPart5.setBody(QString::number(pNoteItem->getShareType()).toUtf8());

    QHttpPart textPart6;
    textPart6.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"set_at\""));
    textPart6.setBody(pNoteItem->getSetAt().toString(DATE_TIME_FORMAT).toUtf8());

    QHttpPart textPart7;
    textPart7.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"type\""));
    textPart7.setBody(QString::number(pNoteItem->getType()).toUtf8());

    QHttpPart textPart8;
    textPart8.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"content\""));
    textPart8.setBody(pNoteItem->getContent());

    QHttpPart textPart9;
    textPart9.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"content_size\""));
    textPart9.setBody(QString::number(pNoteItem->getContentSize()).toUtf8());

    QHttpPart textPart10;
    textPart10.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"md5\""));
    textPart10.setBody(pNoteItem->getMd5().toUtf8());

    if(bUpdate)
    {
        QHttpPart textPart11;
        textPart11.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"note_id\""));
        textPart11.setBody(QString::number(pNoteItem->getId()).toUtf8());

        multiPart->append(textPart11);
    }

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);
    multiPart->append(textPart4);
    multiPart->append(textPart5);
    multiPart->append(textPart6);
    multiPart->append(textPart7);
    multiPart->append(textPart8);
    multiPart->append(textPart9);
    multiPart->append(textPart10);

    postRequest(QString(URL_BASE) + URL_UPDATE_NOTE_ITEM, multiPart, strProgress);
}

void CNetworkManager::updateNoteItem(CNoteItem* pNoteItem)
{
    // user_id
    // login_token

    // note_id
    // title
    // public_type
    // time_at
    // type

    return insertNoteItem(pNoteItem, true);
}

/*
void CNetworkManager::getNoteItemContent(int nNoteId)
{
    // user_id
    // login_token

    // note_id
    m_nActionType = ACTION_GET_NOTE_CONTENT;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart1.setBody(m_strUserId.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart2.setBody(m_strLoginToken.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"note_id\""));
    textPart3.setBody(QString::number(nNoteId).toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);

    postRequest(QString(URL_BASE) + URL_GET_NOTE_CONTENT, multiPart, tr("Get note content: \"") + QString::number(nNoteId) + "\"");
}
*/

/*
void CNetworkManager::updateNoteItemContent(CNoteItem* pNoteItem)
{
    // user_id
    // login_token

    // content
    m_nActionType = ACTION_UPDATE_NOTE_CONTENT;

    QByteArray boundaryRegular(QString("--"+QString::number(qrand(), 10)).toAscii());
    QByteArray boundary("\r\n--"+boundaryRegular+"\r\n");
    QByteArray boundaryLast("\r\n--"+boundaryRegular+"--\r\n");

    QUrl url(QString(URL_BASE) + URL_UPDATE_NOTE_CONTENT);
    QNetworkRequest request(url);
    request.setRawHeader("Host", url.encodedHost());
    request.setRawHeader("Content-Type", QByteArray("multipart/form-data; boundary=").append(boundaryRegular));

    QByteArray mimedata2(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"user_id\"\r\n\r\n");
    mimedata2.append(m_strUserId);
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"login_token\"\r\n\r\n");
    mimedata2.append(m_strLoginToken);
    mimedata2.append(boundary);

    mimedata2.append("Content-Disposition: form-data; name=\"content\"\r\n\r\n");
    mimedata2.append(pNoteItem->getContent());
//    mimedata2.append(QString::fromAscii(pNoteItem->getContent()).toUtf8());
    mimedata2.append(boundaryLast);

    m_pNetworkReply = m_pNetworkAccessManager->post(request, mimedata2);
    replyConnect(m_pNetworkReply);
    emit started();
}
*/

void CNetworkManager::deleteNoteItem(CNoteItem* pNoteItem)
{
    // user_id
    // login_token

    // note_local_id
    // note_id
    m_nActionType = ACTION_DELETE_NOTE_ITEM;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart1.setBody(m_strUserId.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart2.setBody(m_strLoginToken.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"note_id\""));
    textPart3.setBody(QString::number(pNoteItem->getId()).toUtf8());

    QHttpPart textPart4;
    textPart4.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"note_local_id\""));
    textPart4.setBody(QString::number(pNoteItem->getLocalId()).toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);
    multiPart->append(textPart4);

    postRequest(QString(URL_BASE) + URL_DELETE_NOTE_ITEM, multiPart, tr("Delete note: \"") + pNoteItem->getTitle() + "\"");
}

void CNetworkManager::deleteAttachmentItem(CAttachmentItem* pAttachmentItem)
{
    // user_id
    // login_token

    // attachment_id
    m_nActionType = ACTION_DELETE_ATTACHMENT_ITEM;
    pAttachmentItem->setProcessState(STATE_PS_DOWNLOADING);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart1.setBody(m_strUserId.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart2.setBody(m_strLoginToken.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"attachment_id\""));
    textPart3.setBody(QString::number(pAttachmentItem->getId()).toUtf8());

    QHttpPart textPart4;
    textPart4.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"attachment_local_id\""));
    textPart4.setBody(QString::number(pAttachmentItem->getLocalId()).toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);
    multiPart->append(textPart4);

    postRequest(QString(URL_BASE) + URL_DELETE_ATTACHMENT_ITEM, multiPart, tr("Delete attachment: \"") + pAttachmentItem->getFileName() + "\"");
}

void CNetworkManager::processNoteAttachments(CNoteItem* pNoteItem)
{
    if(!pNoteItem || pNoteItem->getId() <= 0)
        return;

    foreach(CAttachmentItem* pItem, pNoteItem->getAttachmentList())
    {
        switch(pItem->getState())
        {
        case STATE_NEW:
            insertAttachmentItem2(pItem);
            break;
        case STATE_DELETE:
            deleteAttachmentItem(pItem);
            break;
        default:
            break;
        }
    }
}

void CNetworkManager::getAttachmentItem2(CAttachmentItem* pAttachmentItem)
{
    m_nActionType = ACTION_GET_ATTACHMENT_ITEM;

    pAttachmentItem->setProcessState(STATE_PS_DOWNLOADING);

    CFileDownload* pFileDownload = new CFileDownload(m_pNetworkAccessManager, pAttachmentItem, this);
    pFileDownload->download(QString(URL_BASE) + URL_DOWNLOAD_ATTACHMENT_ITEM + "/" + QString::number(pAttachmentItem->getId()));
    connect(pFileDownload, SIGNAL(finishedDownloadAttachmentItem(CAttachmentItem*, QString)), this, SIGNAL(finishedDownloadAttachmentItem(CAttachmentItem*, QString)));
 //   connect(pFileDownload, SIGNAL(errorFinishedDownloadAttachmentItem(CAttachmentItem*,QString)), this, SIGNAL(errorFinishedDownloadAttachmentItem(CAttachmentItem*,QString)));
 //   connect(pFileDownload, SIGNAL(finished(QString)), this, SIGNAL(finishedDownloadAttachment(QString)));

    G_MainFrame()->addFileDownload(pFileDownload, tr("Get attachment: \"") + QString::number(pAttachmentItem->getId()) + "\"");
}

/*
QNetworkReply* CNetworkManager::insertAttachmentItem(CAttachmentItem* pItem)
{
    // user_id
    // login_token

    // local_id
    // file_name
    // note_local_id
    // content
    // content_size
    // md5

    m_nActionType = ACTION_INSERT_ATTACHMENT_ITEM;

    QByteArray boundaryRegular(QString("--"+QString::number(qrand(), 10)).toAscii());
    QByteArray boundary("\r\n--"+boundaryRegular+"\r\n");
    QByteArray boundaryLast("\r\n--"+boundaryRegular+"--\r\n");

    QUrl url(QString(URL_BASE) + URL_INSERT_ATTACHMENT_ITEM);
    QNetworkRequest request(url);
    request.setRawHeader("Host", url.encodedHost());
/*    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)");
    request.setRawHeader("Accept-Encoding", "gzip,deflate");
    request.setRawHeader("Accept-Charset", "windows-1251,utf-8;q=0.7,*;q=0.7");
    request.setRawHeader("Keep-Alive", "300");*/
    /*
    request.setRawHeader("Connection", "keep-alive");
    request.setRawHeader("Content-Type", QByteArray("multipart/form-data; boundary=").append(boundaryRegular));

    QByteArray mimedata2(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"user_id\"\r\n\r\n");
    mimedata2.append(m_strUserId);
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"login_token\"\r\n\r\n");
    mimedata2.append(m_strLoginToken);
    mimedata2.append(boundary);

    mimedata2.append("Content-Disposition: form-data; name=\"local_id\"\r\n\r\n");
    mimedata2.append(QString::number(pItem->getLocalId()));
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"file_name\"\r\n\r\n");
    mimedata2.append(pItem->getFileNameDisplay().toUtf8());
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"note_id\"\r\n\r\n");
    mimedata2.append(QString::number(pItem->getParentNoteItem()->getId()));
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"content_size\"\r\n\r\n");
    mimedata2.append(QString::number(pItem->getContentSize()));
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"md5\"\r\n\r\n");
    mimedata2.append(pItem->getMd5());
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"content\"\r\n\r\n");
    mimedata2.append(pItem->getContent().toBase64());

    // attachment item
    mimedata2.append(boundaryLast);

    m_pNetworkReply = m_pNetworkAccessManager->post(request, mimedata2);
    G_MainFrame()->addNetworkReply(m_pNetworkReply, tr("Insert attachment: \"") + pItem->getFileName() + "\"");
    return m_pNetworkReply;
}
*/

/*
QNetworkReply* CNetworkManager::insertAttachmentItem2(CAttachmentItem* pItem)
{
    // user_id
    // login_token

    // local_id
    // file_name
    // note_local_id
    // content
    // content_size
    // md5

    m_nActionType = ACTION_UPLOAD_ATTACHMENT_ITEM;

    QByteArray boundaryRegular(QString("--"+QString::number(qrand(), 10)).toAscii());
    QByteArray boundary("\r\n--"+boundaryRegular+"\r\n");
    QByteArray boundaryLast("\r\n--"+boundaryRegular+"--\r\n");

    QUrl url(QString(URL_BASE) + URL_UPLOAD_ATTACHMENT_ITEM);
    QNetworkRequest request(url);
    request.setRawHeader("Host", url.encodedHost());
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows; U; Windows NT 5.1; ru; rv:1.9.1.3) Gecko/20090824 Firefox/3.5.3 (.NET CLR 3.5.30729)");
//    request.setRawHeader("Keep-Alive", "300");
    request.setRawHeader("Connection", "keep-alive");
    request.setRawHeader("Content-Type", QByteArray("multipart/form-data; boundary=").append(boundaryRegular));

    QByteArray mimedata1("--"+boundaryRegular+"\r\n");
    mimedata1.append("Content-Disposition: form-data; name=\"action\"\r\n\r\n");
    mimedata1.append("file_upload");
    mimedata1.append(boundary);
    mimedata1.append("Content-Disposition: form-data; name=\"upload_files\"; filename=\""+ pItem->getFileNameDisplay().toUtf8()+"\"\r\n");
    mimedata1.append("Content-Type: application/octet-stream\r\n\r\n");

    QByteArray mimedata2(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"user_id\"\r\n\r\n");
    mimedata2.append(m_strUserId.toUtf8());
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"login_token\"\r\n\r\n");
    mimedata2.append(m_strLoginToken.toUtf8());
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"local_id\"\r\n\r\n");
    mimedata2.append(QString::number(pItem->getLocalId()).toUtf8());
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"note_id\"\r\n\r\n");
    mimedata2.append(QString::number(pItem->getParentNoteItem()->getId()).toUtf8());
    mimedata2.append(boundary);
    mimedata2.append("Content-Disposition: form-data; name=\"md5\"\r\n\r\n");
    mimedata2.append(pItem->getMd5().toUtf8());
    mimedata2.append(boundaryLast);

    CFileUpload* pFileUpload = new CFileUpload(pItem->getContent(), mimedata1, mimedata2, this);
    if(!pFileUpload->openFile())
    {
        QString str = tr("Cannot open file \"") + pItem->getFileName() +("\"");
        emit emitMsg(str);
   //     G_MainFrame()->showMessage(tr("Cannot open file \"") + pItem->getFileName() +("\""));
        delete pFileUpload, pFileUpload = NULL;
        return NULL;
    }

    emit emitMsg(QString("insertAttachmentItem2 (Length %1, %2 + %3 + %4 = %5)")
                 .arg(QString::number(pFileUpload->size()))
                 .arg(QString::number(mimedata1.length()))
                 .arg(QString::number(pItem->getContent().length()))
                 .arg(QString::number(mimedata2.length()))
                 .arg(QString::number(mimedata1.length() + pItem->getContent().length() + mimedata2.length())));

    pItem->setProcessState(STATE_PS_UPLOADING);

    m_pNetworkReply = m_pNetworkAccessManager->post(request, pFileUpload);
    m_mapReplyAndFileUpload.insert(m_pNetworkReply, pFileUpload);
    G_MainFrame()->addNetworkReply(m_pNetworkReply, tr("Insert attachment: \"") + pItem->getFileName() + "\"");
    return m_pNetworkReply;
}
*/

void CNetworkManager::insertAttachmentItem2(CAttachmentItem* pItem)
{
    m_nActionType = ACTION_UPLOAD_ATTACHMENT_ITEM;
    pItem->setProcessState(STATE_PS_UPLOADING);

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"action\""));
    textPart1.setBody(QString("file_upload").toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart2.setBody(m_strUserId.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart3.setBody(m_strLoginToken.toUtf8());

    QHttpPart textPart4;
    textPart4.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"local_id\""));
    textPart4.setBody(QString::number(pItem->getLocalId()).toUtf8());

    QHttpPart textPart5;
    textPart5.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"note_id\""));
    textPart5.setBody(QString::number(pItem->getParentNoteItem()->getId()).toUtf8());

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"upload_files\"; filename=\""+ pItem->getFileNameDisplay() + "\""));
    CFileUpload* pFileUpload = new CFileUpload(pItem->getContent(), QByteArray(), QByteArray());
    if(!pFileUpload->openFile())
    {
        QString str = tr("Cannot open file \"") + pItem->getFileName() +("\"");
        emit emitMsg(str);
   //     G_MainFrame()->showMessage(tr("Cannot open file \"") + pItem->getFileName() +("\""));
        delete pFileUpload, pFileUpload = NULL;
        delete multiPart, multiPart = NULL;
        return;
    }
    filePart.setBodyDevice(pFileUpload);
    pFileUpload->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);
    multiPart->append(textPart4);
    multiPart->append(textPart5);
    multiPart->append(filePart);

    postRequest(QString(URL_BASE) + URL_UPLOAD_ATTACHMENT_ITEM, multiPart, tr("Insert attachment: \"") + pItem->getFileName() + "\"");
}

void CNetworkManager::updateClassifyString(const QString& strClassifyString)
{
    // user_id
    // login_token

    // time_year
    // time_year
    m_nActionType = ACTION_UPDATE_CLASSIFY_STRING;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart1.setBody(m_strUserId.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart2.setBody(m_strLoginToken.toUtf8());

    QHttpPart textPart3;
    textPart3.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"classify_string\""));
    textPart3.setBody(strClassifyString.toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);
    multiPart->append(textPart3);

    postRequest(QString(URL_BASE) + URL_UPDATE_CLASSIFY_STRING, multiPart, tr("Update Classify String"));
}

void CNetworkManager::clearNoteItems()
{
    // clear items
    m_bGotAmountOfNotes = false;
    m_bGotNoteList = false;
    m_nAmountOfNotes = -1;
    m_nAmountOfItems = -1;
    m_dtCurrentMonthServer = QDate();

    foreach(CNoteItem* pItem, m_lstNoteItemsFromServer)
    {
        delete pItem;
    }

    m_lstNoteItemsFromServer.clear();
}

void CNetworkManager::clearLoginInfo()
{
    // login info
    m_strUserId = m_strUserName = m_strLoginToken = "";
    m_nActionTypeLast = m_nActionType = ACTION_IDLE;
    m_nUserId = 0;
    m_nAmountOfNotes = -1;
    m_nAmountOfItems = -1;

    clearNoteItems();
}

QList<CNoteItem*>& CNetworkManager::getNoteItemsFromServer()
{
    return m_lstNoteItemsFromServer;
}

bool parseJsonItem2NoteItem(CJsonItem* pItem, QList<CNoteItem*>& lstNoteItems)
{
    if(!pItem)
        return false;

    QString strTmp;
    CJsonItem* pAttachmentItems = NULL;
    CNoteItem* pNoteItem = NULL;
    foreach(CJsonItem* pSubItem, pItem->getChildNodes())
    {
        pNoteItem = new CNoteItem();
        pSubItem->getValueByKey("id", strTmp, false);
        pNoteItem->setId(strTmp.toInt());
        strTmp = "";
        pSubItem->getValueByKey("share_type", strTmp, false);
        pNoteItem->setShareType(strTmp.toInt());
        strTmp = "";
        pSubItem->getValueByKey("type", strTmp, false);
        pNoteItem->setType(strTmp.toInt());
        strTmp = "";
        pSubItem->getValueByKey("set_at", strTmp, false);
        pNoteItem->setSetAt(QDateTime::fromString(strTmp, DATE_TIME_FORMAT));
        strTmp = "";
        pSubItem->getValueByKey("title", strTmp, false);
        pNoteItem->setTitle(strTmp);
        strTmp = "";
        pSubItem->getValueByKey("content_size", strTmp, false);
        pNoteItem->setContentSize(strTmp.toLong());
        strTmp = "";
        pSubItem->getValueByKey("md5", strTmp, false);
        pNoteItem->setMd5(strTmp);

        pAttachmentItems = pSubItem->getItem("attachments");
        if(pAttachmentItems)
        {
            QList<CAttachmentItem*> lstAttachmentItems;
            parseJsonItem2AttachmentItem(pAttachmentItems, lstAttachmentItems);
            pNoteItem->setAttachmentList(lstAttachmentItems);

            // clear list
            foreach(CAttachmentItem* pItem, lstAttachmentItems)
                delete pItem;
            lstAttachmentItems.clear();
        }

        lstNoteItems.push_back(pNoteItem);
    }

    return true;
}

bool parseJsonItem2AttachmentItem(CJsonItem* pAttachmentItems, QList<CAttachmentItem*>& lstAttachmentItems)
{
    if(!pAttachmentItems)
        return false;

    QString strTmp;
    CAttachmentItem* pAttachmentItem = NULL;
    foreach(CJsonItem* pSubItem, pAttachmentItems->getChildNodes())
    {
        pAttachmentItem = new CAttachmentItem();
        pSubItem->getValueByKey("id", strTmp, false);
        pAttachmentItem->setId(strTmp.toInt());
        strTmp = "";
        pSubItem->getValueByKey("file_name", strTmp, false);
        pAttachmentItem->setFileName(strTmp);
        strTmp = "";
        pSubItem->getValueByKey("file_size", strTmp, false);
        pAttachmentItem->setContentSize(strTmp.toLong());
        strTmp = "";
        pSubItem->getValueByKey("md5", strTmp, false);
        pAttachmentItem->setMd5(strTmp);        

        lstAttachmentItems.push_back(pAttachmentItem);
    }

    return true;
}

void CNetworkManager::getPadFile()
{
    QUrl url(QString(URL_PAD_NOTEE));
    QNetworkRequest request(url);
    request.setRawHeader("Host", url.encodedHost());

    QNetworkReply* pNetworkReply = m_pNetworkAccessManager->get(request);
//    G_MainFrame()->addNetworkReply(m_pNetworkReply, tr("Get Pad File"));
    replyConnect(pNetworkReply);
 //   emit started();
}

#ifdef NOTEE_DEBUG
#include <QFile>
#endif

/*
void CNetworkManager::closeFileUploadByReply(QNetworkReply* pReply)
{
    CFileUpload* pFileLoad = m_mapReplyAndFileUpload.take(pReply);
    if(pFileLoad)
    {
        pFileLoad->close();
        delete pFileLoad, pFileLoad = NULL;
    }
}
*/

void CNetworkManager::onNetworkReplyFinished(QNetworkReply* pNetworkReply)
{
    if(!pNetworkReply)
        return;

//    closeFileUploadByReply(pNetworkReply);

    if(pNetworkReply->error() > 0)
    {
        emit emitMsg(tr("Network error2: ") + pNetworkReply->errorString());
        return;
    }

    QByteArray arrBuffer;
    if(pNetworkReply->bytesAvailable())
    {
        arrBuffer = pNetworkReply->readAll();
    }

#ifdef NOTEE_DEBUG
    {
        QFile output("C:\\json.txt");
        output.open(QIODevice::WriteOnly | QIODevice::Text);
        output.write(arrBuffer);
        output.flush();
    }
#endif

    if(!arrBuffer.length())
        return;

    if(parsePadFile(arrBuffer))
        return;

    m_jsonParser.clearJsonItems();

    m_strReturn = arrBuffer;
    if(m_jsonParser.parse(arrBuffer))
    {
        m_strAction = m_jsonParser.getValueByKey("action");
        if(URL_REGISTER == m_strAction ||
                URL_LOGIN == m_strAction)
        {
            m_strUserId = m_strUserName = m_strLoginToken = "";
            // user_id
            m_strUserId = m_jsonParser.getValueByKey("user_id");

            // user_name
            m_strUserName = m_jsonParser.getValueByKey("user_name");

            // login_token
            m_strLoginToken = m_jsonParser.getValueByKey("login_token");
        }
        else if(URL_GET_AMOUNT_OF_NOTES == m_strAction)
        {
            if(RET_OK == getResultValue())
            {
                m_bGotAmountOfNotes = true;
                QString strAmount = m_jsonParser.getValueByKey("amount");
                if(!strAmount.isEmpty())
                    m_nAmountOfNotes = strAmount.toInt();

                strAmount = m_jsonParser.getValueByKey("items_amount");
                if(!strAmount.isEmpty())
                    m_nAmountOfItems = strAmount.toInt();
            }
        }
        else if(URL_GET_NOTE_LIST_OFFSET == m_strAction)
        {
            if(RET_OK == getResultValue())
            {
                clearNoteItems();

//                QString strOffset = m_jsonParser.getValueByKey("offset");
//                QString strNumber = m_jsonParser.getValueByKey("number");
                CJsonItem* pItem = m_jsonParser.getItem("notes", false);
                ::parseJsonItem2NoteItem(pItem, m_lstNoteItemsFromServer);
                m_bGotNoteList = true;
            }
        }
        else if(m_strAction.isEmpty())
        {
            m_nActionTypeLast = m_nActionType;
            m_nActionType = ACTION_IDLE;
            return;
        }
    }
    else
    {
        m_jsonParser.addKeyValue("result", "1");
        m_jsonParser.addKeyValue("error", m_jsonParser.getLastError() + "," + m_strReturn);
    }

    m_nActionTypeLast = m_nActionType;
    m_nActionType = ACTION_IDLE;
 //   delete m_pNetworkReply;
 //   m_pNetworkReply->deleteLater();

    emit readyReadyfinished();
}

void CNetworkManager::parseJsonItem2NoteItem(CJsonItem* pItem)
{
    ::parseJsonItem2NoteItem(pItem, m_lstNoteItemsFromServer);
}

bool CNetworkManager::parsePadFile(const QByteArray& arrBuffer)
{
    if(arrBuffer.length() < 10)
        return false;

    if(arrBuffer[0] != '<' ||
            arrBuffer[1] != '?' ||
            arrBuffer[2] != 'x' ||
            arrBuffer[3] != 'm' ||
            arrBuffer[4] != 'l')
        return false;

    QDomDocument doc;
    doc.setContent(arrBuffer);
    QDomNodeList list = doc.elementsByTagName("Program_Version");
    QString strAppVersion = list.at(0).toElement().text();

    list = doc.elementsByTagName("Program_Change_Info");
    QString strChangeInfo = list.at(0).toElement().text();

    list = doc.elementsByTagName("Program_Release_Year");
    QString stReleaseYear = list.at(0).toElement().text();

    list = doc.elementsByTagName("Program_Release_Month");
    QString stReleaseMonth = list.at(0).toElement().text();

    list = doc.elementsByTagName("Program_Release_Day");
    QString stReleaseDay = list.at(0).toElement().text();

    list = doc.elementsByTagName("File_Size_MB");
    QString strFileSizeMB = list.at(0).toElement().text();

    list = doc.elementsByTagName("Primary_Download_URL");
    QString strDownloadUrl = list.at(0).toElement().text();

    if(!strAppVersion.isEmpty() &&
            !strChangeInfo.isEmpty() &&
            !stReleaseYear.isEmpty() &&
            !stReleaseMonth.isEmpty() &&
            !stReleaseDay.isEmpty() &&
            !strFileSizeMB.isEmpty() &&
            !strDownloadUrl.isEmpty())
    {
        G_Settings()->setPadFileTime(QDateTime::currentDateTime());
        G_Settings()->setNewVersion(strAppVersion);
        G_Settings()->setChangeInfo(strChangeInfo);
        G_Settings()->setReleaseDate(stReleaseYear + "-" + stReleaseMonth + "-" + stReleaseDay);
        G_Settings()->setFileSizeMB(strFileSizeMB);
        G_Settings()->setDownloadUrl(strDownloadUrl);

        G_Settings()->save();
    }

    emit padReadyReadyfinished();

    return true;
}

QString CNetworkManager::getValueByKey(const QString& strKey) const
{
    return m_jsonParser.getValueByKey(strKey);
}

const QByteArray& CNetworkManager::getLeftContent() const
{
    return m_jsonParser.getLeftContent();
}

QString CNetworkManager::getActionValue() const
{
    return m_strAction;
}

int CNetworkManager::getResultValue() const
{
    QString strValue = m_jsonParser.getValueByKey("result");
    strValue = strValue.trimmed();

    if(strValue.isEmpty())
        return RET_OK;

    return strValue.toInt();
}

QString CNetworkManager::getErrorString()
{
    return m_jsonParser.getValueByKey("error");
}

QString CNetworkManager::getDataUpdatedAtString()
{
    return m_jsonParser.getValueByKey("data_updated_at");
}

int CNetworkManager::getOffsetOfNotes()
{
    return m_jsonParser.getValueByKey("offset").toInt();
}

int CNetworkManager::getNumberOfNotes()
{
    return m_jsonParser.getValueByKey("number").toInt();
}

QString CNetworkManager::getClassifyString()
{
    return m_jsonParser.getValueByKey("classify_string");
}

const QString& CNetworkManager::getLoginToken() const
{
    return m_strLoginToken;
}

const QString& CNetworkManager::getUserName() const
{
    return m_strUserName;
}

int CNetworkManager::getUserId() const
{
    if(m_strUserId.isEmpty())
        return -1;

    return m_strUserId.toInt();
}

int CNetworkManager::getActionType()
{
    return m_nActionType;
}

int CNetworkManager::getActionTypeLast()
{
    return m_nActionTypeLast;
}

bool CNetworkManager::getAmountOfNotes(int& nAmount, int &nAmountOfItems)
{
    if(!m_bGotAmountOfNotes)
    {
        getAmountOfNotesFromServer();
        return false;
    }

    nAmount = m_nAmountOfNotes;
    nAmountOfItems = m_nAmountOfItems;
    return true;
}

void CNetworkManager::userPulse()
{
    if(!m_strUserId.length() || !m_strLoginToken.length())
        return;

    // user_id
    // login_token

    m_nActionType = ACTION_USER_PULSE;

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"user_id\""));
    textPart1.setBody(m_strUserId.toUtf8());

    QHttpPart textPart2;
    textPart2.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"login_token\""));
    textPart2.setBody(m_strLoginToken.toUtf8());

    multiPart->append(textPart1);
    multiPart->append(textPart2);

    postRequest(QString(URL_BASE) + URL_USER_PULSE, multiPart, tr("User pulse"));
}
