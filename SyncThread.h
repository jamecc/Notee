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

#ifndef SYNCTHREAD_H
#define SYNCTHREAD_H

#include <QThread>
#include <QNetworkReply>
#include "JsonParser.h"
#include "NetworkManager.h"

enum{
    RETURN_TYPE_CONTINUE = 0,
    RETURN_TYPE_ERROR,
    RETURN_TYPE_FINISH
};

class CSyncThread : public QThread
{
    Q_OBJECT
public:
    CSyncThread(QObject *parent = 0);

    QString getValueByKey(const QString& strKey) const;
    QString getErrorString();
    int getResultValue() const;

    void setDataUpdatedAt(QString str)   {m_strDataUpdatedAt = str;}
    
signals:
    void failToLogin(const QString& strUserName, const QString& strError);
    void successToLogin(const QString& strUserIdLocal, const QString& strUserId);
    void failToGetNoteList(const QString& strError);
    void failToGetAmountOfNotes(const QString& strError);
    void successToGetAmountOfNotes(const QString& strAmount);
    void failToInsertNoteItem(const QString& strNoteLocalId, const QString& strError);
    void successToInsertNoteItem(const QString& strNoteLocalId, const QString& strNoteId);
    void failToUpdateNoteItem(const QString& strNoteId, const QString& strError);
    void successToUpdateNoteItem(const QString& strNoteId);
    void failToDeleteNoteItem(const QString& strNoteId, const QString& strError);
    void successToDeleteNoteItem(const QString& strNoteId);
    void failToGetNoteContent(const QString& strNoteId, const QString& strError);
    void successToGetNoteContent(const QString& strNoteId);
    void failToPulseUpdatedDate(const QString& strError);
    void successToPulseUpdatedDate(const QString& strUpdatedDate);

    void signalSyncProgressMax(int);
    void signalSyncProgressPos(int);
    void syncProgressBegin();
    void syncProgressFinished();
    void refreshDayWidget(const QDate& dt);

    void updateNoteItem(int nLocalId);
    void removeNoteItem(int nLocalId);

    void emitError(const QString& strErr);
    void networkReplyFinished();
    
protected slots:
    void onNetworkReplyFinished(QNetworkReply* pNetworkReply);

protected:
    virtual void run();
    void connectFinished(QNetworkReply* pNetworkReply);
    // 0 continue, 1 error, 2 finish
    int parseBuffer();
    // 0 continue, 1 error, 2 finish
    void syncLocalAndServerDbOffset();
    void beforeSync();
    void afterSync();

private:
    CNetworkManager* m_pNetworkManager;
    CJsonParser m_jsonParser;

    QString m_strAction;
    QString m_strUserId;
    QString m_strUserIdLocal;
    QString m_strUserName;
    QString m_strLoginToken;
    bool m_bGotAmountOfNotes;
    int m_nAmountOfNotes;
    int m_nNumberPerTimes;
    int m_nCurrentOffsetOfNotes;
    int m_nCurrentPosOfNotes;
    QByteArray m_arrBuffer;
    QString m_strDataUpdatedAt;
};

#endif // SYNCTHREAD_H
