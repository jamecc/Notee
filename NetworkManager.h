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

#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H


#include <QObject>
#include <QNetworkReply>
//#include <QSslError>
#include <QDate>
#include <QDateTime>
#include <QMap>
#include <QList>
#include <QStringList>
#include "JsonParser.h"

#define URL_REGISTER        "register"
#define URL_LOGIN           "login"
#define URL_FORGOT_PWD      "forgot_pwd"
#define URL_GET_NOTE_LIST_MONTH     "get_note_list_by_month"
#define URL_GET_NOTE_LIST_OFFSET    "get_note_list_by_offset"
#define URL_GET_NOTE_ITEM       "get_note_item"
#define URL_INSERT_NOTE_ITEM    "insert_note_item"
#define URL_UPDATE_NOTE_ITEM    "update_note_item"
#define URL_DELETE_NOTE_ITEM    "delete_note_item"
//#define URL_GET_NOTE_CONTENT        "get_note_content"
//#define URL_UPDATE_NOTE_CONTENT     "update_note_content"
//#define URL_GET_ATTACHMENT_ITEM         "get_attachment_item"
//#define URL_INSERT_ATTACHMENT_ITEM      "insert_attachment_item"
#define URL_DOWNLOAD_ATTACHMENT_ITEM    "download_attachment_item"
#define URL_UPLOAD_ATTACHMENT_ITEM      "upload_attachment_item"
#define URL_DELETE_ATTACHMENT_ITEM      "delete_attachment_item"

#define URL_UPDATE_CLASSIFY_STRING      "update_classify_string"

#define URL_GET_AMOUNT_OF_NOTES         "get_amount_of_notes"
#define URL_USER_PULSE                  "user_pulse"

class CNoteItem;
class CAttachmentItem;
class QNetworkAccessManager;
class CFileUpload;

class CNetworkManager : public QObject
{
    Q_OBJECT

public:
    enum{ACTION_IDLE=0,
         ACTION_REGISTER,
         ACTION_LOGIN,
         ACTION_FORGOT_PWD,
         ACTION_GET_NOTE_LIST,
         ACTION_GET_NOTE_LIST_MONTH,
         ACTION_GET_NOTE_LIST_OFFSET,

         ACTION_GET_NOTE_ITEM,
         ACTION_INSERT_NOTE_ITEM,
         ACTION_UPDATE_NOTE_ITEM,
         ACTION_DELETE_NOTE_ITEM,

         ACTION_GET_NOTE_CONTENT,
         ACTION_UPDATE_NOTE_CONTENT,

         ACTION_GET_ATTACHMENT_ITEM,
         ACTION_INSERT_ATTACHMENT_ITEM,
         ACTION_UPLOAD_ATTACHMENT_ITEM,
         ACTION_DELETE_ATTACHMENT_ITEM,

         ACTION_GET_ATTACHMENT_CONTENT,
         ACTION_UPDATE_ATTACHMENT_CONTENT,

         ACTION_GET_MIN_MAX_TIME_OF_NOTES,
         ACTION_GET_AMOUNT_OF_NOTES,
         ACTION_UPDATE_CLASSIFY_STRING,
         ACTION_USER_PULSE};

public:
    explicit CNetworkManager(QObject *parent = 0);
    virtual ~CNetworkManager();

    void replyConnect(QNetworkReply* pNetworkReply);
    QNetworkReply* postRequest(QString strUrl, QHttpMultiPart* pHttpPart, QString strAction);

    void register_user(QString strName, QString strPwd, QString strEmail);
    void login(QString strName, QString strPwd);
    void forgotPwd(QString strUserName);

    void getNoteItemListMonth(int nYear, int nMonth);
    void getNoteItemListByOffset(int nOffset, int nNum);
//    void getNoteItemList(const QDateTime& dtFrom, const QDateTime& dtTo);
    void getNoteItem(int nNodeId);
    void insertNoteItem(CNoteItem* pNoteItem, bool bUpdate = false);
    void updateNoteItem(CNoteItem* pNoteItem);
//    void getNoteItemContent(int nNoteId);
//    void insertNoteItemContent(CNoteItem* pNoteItem);
//    void updateNoteItemContent(CNoteItem* pNoteItem);
//    QNetworkReply* getAttachmentItem(int nAttachmentId);
    void getAttachmentItem2(CAttachmentItem* pAttachmentItem);
 //   void insertAttachmentItem(CAttachmentItem* pItem);
    void insertAttachmentItem2(CAttachmentItem* pItem);
    void deleteNoteItem(CNoteItem* pNoteItem);
    void deleteAttachmentItem(CAttachmentItem* pAttachmentItem);
    void processNoteAttachments(CNoteItem* pNoteItem);

    QString getValueByKey(const QString& strKey) const;
    const QByteArray& getLeftContent() const;
    QString getActionValue() const;
    int getResultValue() const;
    QString getErrorString();
    QString getDataUpdatedAtString();
    int getOffsetOfNotes();
    int getNumberOfNotes();
    QString getClassifyString();
    const QString& getLoginToken() const;
    const QString& getUserName() const;
    int getUserId() const;
    int getActionType();
    int getActionTypeLast();

    const QDate& getCurrentMonthFromServer() const;
    void getAmountOfNotesFromServer();
    void getAmountOfItemsFromServer();
    bool getAmountOfNotes(int& nAmount, int& nAmountOfItems);
    bool isGotNoteList() const          {return m_bGotNoteList;}
    void setGotNoteList(bool bVal)      {m_bGotNoteList = bVal;}

    QString getResultString()           {return m_strReturn;}

    void clearNoteItems();
    void clearLoginInfo();

    void userPulse();
    void getPadFile();

    void updateClassifyString(const QString& strClassifyString);

    void parseJsonItem2NoteItem(CJsonItem* pItem);
    QList<CNoteItem*>& getNoteItemsFromServer();

    bool parsePadFile(const QByteArray& arrBuffer);

signals:
  //  void started();
    void readyReadyfinished();
    void padReadyReadyfinished();
    void uploadProgress(qint64,qint64);
    void downloadProgress(qint64,qint64);
//    void replyError(QNetworkReply::NetworkError);
//    void finished(QNetworkReply*);
    void finishedDownloadAttachmentItem(CAttachmentItem* pAttachmentItem, QString);
//    void errorFinishedDownloadAttachmentItem(CAttachmentItem* pAttachmentItem,QString);
//    void finishedDownloadAttachment(QString);
    void noteItemSelectedChanged();
    void emitMsg(QString);

public slots:
    void onNetworkReplyFinished(QNetworkReply*);

protected:
//    void closeFileUploadByReply(QNetworkReply* pReply);

private:
    QNetworkAccessManager* m_pNetworkAccessManager;
    QString m_strAction;
    QString m_strUserId;
    QString m_strUserName;
    QString m_strLoginToken;
    QString m_strReturn;
    CJsonParser m_jsonParser;

    bool m_bGotAmountOfNotes;
    int m_nAmountOfNotes;
    int m_nAmountOfItems;
    bool m_bGotNoteList;

    int m_nActionTypeLast;
    int m_nActionType;
    int m_nUserId;

    QDate m_dtCurrentMonthServer;
    QList<CNoteItem*> m_lstNoteItemsFromServer;
    QList<QDate> m_lstDtMonthsFromServer;

//    QMap<QNetworkReply*, CFileUpload*> m_mapReplyAndFileUpload;
};

bool parseJsonItem2NoteItem(CJsonItem* pItem, QList<CNoteItem*>& lstNoteItems);

#endif // NETWORKMANAGER_H
