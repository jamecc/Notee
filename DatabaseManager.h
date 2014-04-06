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

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlError>
#include <QSqlDatabase>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QSslError>
#include <QApplication>

enum
{
    INCLUDE_TYPE_LEADING = 0,
    INCLUDE_TYPE_INCLUDE,
    INCLUDE_TYPE_ENDING
};

struct ST_CLASSIFY_TYPE
{
    int nIncludeType;
    QString strClassifyString;
};

///////////////////////////////////////////////////////////////////
QString g_getClassTypeString(int nType);
int g_getClassTypeInt(const QString& str);
QString g_getClassString(int nType, const QString& str);
QString g_getClassifyStringName(ST_CLASSIFY_TYPE* pType);
bool g_parseClassifyString(const QString& str, ST_CLASSIFY_TYPE* pType);
QString g_getClassStringToolTip(const QString& str);

///////////////////////////////////////////////////////////////////
#define DB_TYPE_SQLITE	0
class CNoteItem;
class CAttachmentItem;
class CDatabaseManager : public QObject
{
    Q_OBJECT
public:
    CDatabaseManager(int nType = DB_TYPE_SQLITE);
    virtual ~CDatabaseManager(void);

    bool checkIfNeedClearDB();
    QString getDbVersion();

    bool OpenDB(const QString& strDB = QString());
    bool DeleteDB();
    void CloseDB();
    QSqlError GetLastError();
    QSqlDatabase& GetDB();

    bool hasModifiedData() const;

    int getUserLocalId(const QString& strUserName);
    QString addUser(const QString& strName, const QString& strPwd, int nUserServerId = 0);
    QString updataUser(int nUserLocalId, int nUserServerId, const QString& strUserName, const QString& strPwd);
    QString deleteUser(const QString& strUserName);
    QString deleteUser(int nUserLocalId);
    bool login(const QString strUserName, const QString& strPwd);
    void clearLoginInfo();
    void clearNoteItems();

    int getUserLocalId() const;
    int getUserServerId() const;
    QString getUserName() const;
    QString getDataUpdatedAtString() const      {return m_strDataUpdatedAt;}
    bool setDataUpdateAtString(const QString& str);

    QString addData(CNoteItem* pNoteItem);
    QString addData(CAttachmentItem* pAttachItem);
    QString updateData(CNoteItem* pNoteItem, bool bOnlyState = false,
                       bool bByServerId = false, bool bWithoutContent = true);
    QString updateData(CAttachmentItem* pAttachItem, bool bWithoutContent = true);
    QString updateAttachmentList(CNoteItem* pNoteItem);
    QString updateAttachments(CNoteItem* pNoteItem);
    QString updateAttachmentIdAndState(int nLocalId, int nId, int nState);
    QString updateData(const QVector<CNoteItem*>& vecNoteItems);

    bool deleteNoteItem(int nLocalId);
    bool deleteAttachmentItem(int nLocalId);
    void deleteNoteItemsByUserId(int nUserId);
    void deleteAttachmentItemsByNoteId(int nNoteId);

    void loadAllNoteItems();
    int GetNoteItems(const QDateTime& dateFrom, const QDateTime& dateTo, QList<CNoteItem*>& lstNoteItems, bool bCheckState = true);
    int GetNoteItems(const QDate& today, QList<CNoteItem*>& lstNoteItems, bool bCheckState = true);
    void getAttachmentItems(CNoteItem* pNoteItem);

    int GetItemCount();
    int GetCountOfNoteItems();
    int GetAmountOfItems();

    bool isIncludeTheMonthOnLocal(const QDate& dtMonth);
    bool getNoteItemsFromLocal(int nYear, int nMonth, QList<CNoteItem*>& lstNoteItems);
    bool getNoteItemListMonth(int nYear, int nMonth, QList<CNoteItem*>& lstNoteItems, bool bStateCheck = true);

    void getMinMaxTimeOfNotes(QString& strMinTime, QString& strMaxTime) const;
    CNoteItem* getNoteItem(int nId) const;
    CNoteItem* getNoteItemByLocalId(int nLocalId) const;
    CAttachmentItem* getAttachmentItem(int nId) const;
    CAttachmentItem* getAttachmentItemByLocalId(int nLocalId) const;
    QByteArray getAttachmentContent(int nLocalId);
    void loadAttachmentContent(CAttachmentItem *pItem);

    QList<CNoteItem*> getNotesByString(const QString& strText, bool bCheckState = true, int nRuleType = INCLUDE_TYPE_INCLUDE);
    QList<CNoteItem*>& getNoteItemList()                            {return m_lstNoteItems;}

    const QString& getClassifyLabels() const                   {return m_strClassifyLabels;}
    const QString& getClassifyLabelsAtServer() const           {return m_strClassifyLabelsAtServer;}
    void updateClassifyLabels(const QString& strLabels, const QString& strLabels2);
    QList<ST_CLASSIFY_TYPE*>& getClassifyTypesList();
    QStringList getClassifyTypesStringList();
    void parseClassifyTypesList();
    void clearClassifyTypesList();

    CNoteItem* getSelectedNoteItem();

    void clearSelection();

signals:
    void noteItemSelectionChanged();
    void emitMsg(QString);

protected slots:

protected:
    bool createTables();
    void connectNoteItem(CNoteItem* pNoteItem);
    QString deleteData(CNoteItem* pNoteItem);
    QString deleteData(CAttachmentItem* pItem);

protected:
    int m_nDbType;
    QSqlDatabase m_db;

    int m_nUserLocalId;
    int m_nUserServerId;
    QString m_strUserName;

    QString m_strDataUpdatedAt;

    QString m_strClassifyLabels;
    QString m_strClassifyLabelsAtServer;
    bool m_bNeedUpdateClassify;
    QList<ST_CLASSIFY_TYPE*> m_lstClassifyTypes;

    QList<CNoteItem*> m_lstNoteItems;
    QList<QDate> m_lstMonthsFromLocal;

    bool m_bLoadAllNoteItems;
};

CNoteItem* G_getNoteItemByServerId(int nId, const QList<CNoteItem*>& lstNoteItems);

#endif // #ifndef DATABASEMANAGER_H
