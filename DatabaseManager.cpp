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

#include "stdafx.h"
#include <QSqlQuery>
#include <QNetworkRequest>
#include <QSqlRecord>
#include <QSqlField>
#include "NoteItem.h"
#include "AttachmentItem.h"
#include "CommonFunctions.h"
#include "MainFrame.h"
#include "DatabaseManager.h"

enum NOTE_IDS {
    ID_DB_NOTE_ID = 0,
    ID_DB_NOTE_SERVER_ID,
    ID_DB_NOTE_USER_ID,
    ID_DB_NOTE_SET_AT,
    ID_DB_NOTE_TYPE,
    ID_DB_NOTE_TITLE,
    ID_DB_NOTE_SHARE_TYPE,
    ID_DB_NOTE_CONTENT,
    ID_DB_NOTE_CONTENT_SIZE,
    ID_DB_NOTE_MD5,
    ID_DB_NOTE_STATE
} ;

enum ATTACHMENT_ITEM_IDS{
    ID_DB_ATTACHMENT_ID = 0,
    ID_DB_ATTACHMENT_SERVER_ID,
//    ID_DB_ATTACHMENT_USER_ID,
//    ID_DB_ATTACHMENT_NOTE_LOCAL_ID,
    ID_DB_ATTACHMENT_FILE_NAME,
 //   ID_DB_ATTACHMENT_CONTENT,
    ID_DB_ATTACHMENT_CONTENT_SIZE,
    ID_DB_ATTACHMENT_MD5,
    ID_DB_ATTACHMENT_STATE
} ;
//////////////////////////////////////////////////////////////////////////////////////////////

QString g_getClassTypeString(int nType)
{
    switch(nType)
    {
    case INCLUDE_TYPE_LEADING:
        return QString(QApplication::translate("Begin with", "Begin with"));
    case INCLUDE_TYPE_INCLUDE:
        return QString(QApplication::translate("Include", "Include"));
    case INCLUDE_TYPE_ENDING:
        return QString(QApplication::translate("End with", "End with"));
    }

    return QString();
}

int g_getClassTypeInt(const QString& str)
{
    if(QString(QApplication::translate("Include", "Include")) == str)
    {
        return INCLUDE_TYPE_INCLUDE;
    }
    else if(QString(QApplication::translate("End with", "End with")) == str)
    {
        return INCLUDE_TYPE_ENDING;
    }
    return INCLUDE_TYPE_LEADING;
}

QString g_getClassString(int nType, const QString& str)
{
    switch(nType)
    {
    case INCLUDE_TYPE_LEADING:
        return QString("%1%2%3").arg("^").arg(str).arg(" ");
    case INCLUDE_TYPE_INCLUDE:
        return QString("%1%2%3").arg(" ").arg(str).arg(" ");
    case INCLUDE_TYPE_ENDING:
        return QString("%1%2%3").arg(" ").arg(str).arg("$");
    default:
        break;
    }

    return QString();
}

QString g_getClassifyStringName(ST_CLASSIFY_TYPE* pType)
{
    if(!pType)
        return QString();

    return g_getClassString(pType->nIncludeType, pType->strClassifyString);
}

bool g_parseClassifyString(const QString& str, ST_CLASSIFY_TYPE* pType)
{
    if(!pType || str.length() < 3)
        return false;

    if(str.at(0) == '^')
    {
        pType->nIncludeType = INCLUDE_TYPE_LEADING;
    }
    else if(str.at(str.length() - 1) == '$')
    {
        pType->nIncludeType = INCLUDE_TYPE_ENDING;
    }
    else
    {
        pType->nIncludeType = INCLUDE_TYPE_INCLUDE;
    }

    pType->strClassifyString = str.mid(1, str.length() - 2);
    return true;
}

QString g_getClassStringToolTip(const QString& str)
{
    if(str.length() < 3)
        return QString();

    QString strTip;
    if(str.at(0) == '^')
    {
        strTip = QString(QApplication::translate("begin", "Title begin with \"%1\"")).arg(str.mid(1, str.length() - 2));
    }
    else if(str.at(str.length() - 1) == '$')
    {
        strTip = QString(QApplication::translate("end", "Title end with \"%1\"")).arg(str.mid(1, str.length() - 2));
    }
    else
    {
        strTip = QString(QApplication::translate("include", "Title include \"%1\"")).arg(str.mid(1, str.length() - 2));
    }

    return strTip;
}

//////////////////////////////////////////////////////////////////////////////////////////////
CNoteItem* G_getNoteItemByLocalId(int nId, const QList<CNoteItem*>& lstNoteItems)
{
    foreach(CNoteItem* pItem, lstNoteItems)
    {
        if(pItem->getLocalId() == nId)
            return pItem;
    }

    return NULL;
}

CNoteItem* G_getNoteItemByServerId(int nId, const QList<CNoteItem*>& lstNoteItems)
{
    foreach(CNoteItem* pItem, lstNoteItems)
    {
        if(pItem->getId() == nId)
            return pItem;
    }

    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////
CDatabaseManager::CDatabaseManager(int nType/* = DB_TYPE_SQLITE*/)
    : m_nDbType(nType)
{
    m_bNeedUpdateClassify = false;
    m_bLoadAllNoteItems = false;
    clearLoginInfo();
}

CDatabaseManager::~CDatabaseManager(void)
{
    m_db.close();
    clearClassifyTypesList();
}

bool CDatabaseManager::checkIfNeedClearDB()
{
    if (m_db.isOpen())
    {
        // no table 'users'
        if(m_db.tables().indexOf("users") < 0)
            return false;

  //      QString strDbVer = getDbVersion();
        if(getDbVersion() != DB_VERSION)
            return true;
    }

    return false;
}

QString CDatabaseManager::getDbVersion()
{
    if (m_db.isOpen())
    {
        QString strSQL = "SELECT * FROM options WHERE name = 'db_ver'";

        QSqlQuery query(m_db);
        bool bRet = query.exec(strSQL);

        if(bRet && query.next()) {
            return query.value(1).toString();
        }
    }

    return QString();
}

bool CDatabaseManager::OpenDB(const QString& strDB)
{
    QString strDBTmp;
    if(strDB.isEmpty())
        strDBTmp = "public.notee";
    else
        strDBTmp = strDB;
    // Find QSLite driver
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setUserName("NoteeApp");
    m_db.setPassword("ABCDEFGHIJJJJIHHZasdfwerfsdfiashdfkasdfnzxcv,nzxcvkiuwhefnxzcvUYTRYTFHGVFHG32834687234#$%#%$#@#^^&^*(&^^&$$%#%#%#$%#8986413213131684&^%&^%");
#ifdef Q_OS_LINUX
    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append(strDBTmp);
    path = QDir::toNativeSeparators(path);
    m_db.setDatabaseName(path);
#else    // NOTE: File exists in the application private folder, in Symbian Qt implementation
    m_db.setDatabaseName(strDBTmp);
#endif     // Open database
    bool bRet = m_db.open();
    if(bRet)
    {
        if(checkIfNeedClearDB())
        {
            DeleteDB();
            return OpenDB(strDB);
        }

        createTables();
    }
    else
        QMessageBox::critical(NULL, tr("Error"), tr("Fail to open database."));
    return bRet;
}

bool CDatabaseManager::DeleteDB()
{
    QString strName = m_db.databaseName();
    m_strDataUpdatedAt.clear();
    // Close database
    m_db.close();
#ifdef Q_OS_LINUX    
    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append(strName);
    path = QDir::toNativeSeparators(path);
    return QFile::remove(path);
#else
    // Remove created database binary file
    return QFile::remove(strName);
#endif
}

void CDatabaseManager::CloseDB()
{
    m_db.close();
}

QSqlError CDatabaseManager::GetLastError()
{
    return m_db.lastError();
}

QSqlDatabase& CDatabaseManager::GetDB()
{
    return m_db;
}

bool CDatabaseManager::hasModifiedData() const
{
    foreach(CNoteItem* pItem, m_lstNoteItems)
    {
        if(STATE_NORMAL != pItem->getState())
            return true;
    }

    return false;
}

bool CDatabaseManager::createTables()
{
    bool ret = false;
    if (m_db.isOpen())
    {
        // Table "users"
        if(m_db.tables().indexOf("users") < 0)
        {
            QSqlQuery query(m_db);
            ret = query.exec("CREATE TABLE users"
                    "(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "server_id INTEGER,"
                    "account TEXT, "
                    "password TEXT, "
                    "nickname TEXT, "
                    "email TEXT, "
                    "birthday TEXT,"
                    "update_at TEXT,"
                    "classify_labels TEXT,"
                    "classify_labels_at_server TEXT"
                    ")");
        }

        // Table "notes"
        if(m_db.tables().indexOf("notes") < 0)
        {
            QSqlQuery query(m_db);
            ret = query.exec("CREATE TABLE notes"
                    "(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "server_id INTEGER, "
                    "user_id INTEGER,"
                    "set_at TEXT, "
                    "type INTEGER, "
                    "title TEXT, "
                    "share_type INTEGER, "
                    "content LONGBLOB, "
                    "content_size TEXT,"
                    "md5 TEXT, "
                    "state INTEGER)");
        }

        // Table "attachments"
        if(m_db.tables().indexOf("attachments") < 0)
        {
            QSqlQuery query(m_db);
            ret = query.exec("CREATE TABLE attachments"
                    "(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "server_id INTEGER, "
                    "user_id INTEGER, "
                    "note_local_id INTEGER,"
                    "filename TEXT,"
                    "content LONGBLOB, "
                    "content_size TEXT,"
                    "md5 TEXT, "
                    "state INTEGER)");
        }

        // Table "options"
        if(m_db.tables().indexOf("options") < 0)
        {
            QSqlQuery query(m_db);
            ret = query.exec("CREATE TABLE options ("
                    "name TEXT,"
                    "val TEXT)");

            QSqlQuery query2(m_db);
            query2.prepare("INSERT INTO options (name, val) "
                          "VALUES (?, ?)");
            query2.bindValue(0, QVariant::fromValue(QString("db_ver")));
            query2.bindValue(1, QVariant::fromValue(QString(DB_VERSION)));

            ret = query2.exec();
        }
    }
    return ret;
}

int CDatabaseManager::getUserLocalId(const QString& strUserName)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM users WHERE account = ?");
    query.bindValue(0, QVariant::fromValue(strUserName)); // UserID
    query.exec();

    if (query.next()) {
        return query.value(ID_DB_NOTE_ID).toInt();
    }

    return -1;
}

QString CDatabaseManager::addUser(const QString& strUserName, const QString& strPwd, int nUserServerId)
{
    int nUserLocalId = getUserLocalId(strUserName);
    if(nUserLocalId > 0)
    {
        updataUser(nUserLocalId, nUserServerId, strUserName, strPwd);
        return QString();
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT INTO users (account, password, server_id) "
                  "VALUES (:account, :password, :server_id)");
    query.bindValue(0, QVariant::fromValue(strUserName)); //
    query.bindValue(1, QVariant::fromValue(strPwd));	//
    query.bindValue(2, QVariant::fromValue(nUserServerId));	//
    query.exec();

    if(query.numRowsAffected() <= 0)
            return query.lastError().text();

    m_nUserLocalId = query.lastInsertId().toInt();
    m_nUserServerId = nUserServerId;
    m_strUserName = strUserName;

    return QString();
}

QString CDatabaseManager::updataUser(int nUserLocalId, int nUserServerId, const QString& strUserName, const QString& strPwd)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET server_id = :server_id, "
                  "account = :user_name "
                  "password = :pwd "
                  "WHERE id = :id");
    query.bindValue(0, QVariant::fromValue(nUserServerId));	// server_id
    query.bindValue(1, QVariant::fromValue(strUserName)); // user name
    query.bindValue(2, QVariant::fromValue(strPwd)); // password
    query.bindValue(3, QVariant::fromValue(nUserLocalId)); // local id
    query.exec();

    if(query.numRowsAffected() <= 0)
            return query.lastError().text();

    m_nUserLocalId = nUserLocalId;
    m_nUserServerId = nUserServerId;
    m_strUserName = strUserName;

    return QString();
}

QString CDatabaseManager::deleteUser(const QString& strUserName)
{
    int nUserId = getUserLocalId(strUserName);
    if(nUserId > 0)
    {
        return deleteUser(nUserId);
    }

    return QString();
}

QString CDatabaseManager::deleteUser(int nUserLocalId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE id = :id");
    query.bindValue(0, QVariant::fromValue(nUserLocalId));	// server_id
    query.exec();

    if(query.numRowsAffected() <= 0)
            return query.lastError().text();

    // remove attachment items
    query.prepare("SELECT id FROM notes WHERE user_id = :user_id");
    query.bindValue(0, QVariant::fromValue(nUserLocalId));	//
    query.exec();

    while (query.next()) {
        deleteAttachmentItemsByNoteId(query.value(0).toInt());
    }

    // remove note items
    deleteNoteItemsByUserId(nUserLocalId);	//

    if(m_nUserLocalId == nUserLocalId)
    {
        clearLoginInfo();
    }

    return QString();
}

bool CDatabaseManager::login(const QString strUserName, const QString& strPwd)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id, server_id, update_at, "
                  "classify_labels, classify_labels_at_server FROM users "
                  "WHERE account = ? AND password = ?");
    query.bindValue(0, QVariant::fromValue(strUserName)); //
    query.bindValue(1, QVariant::fromValue(strPwd)); //
    query.exec();

    if (query.next()) {
        m_nUserLocalId = query.value(0).toInt();
        m_nUserServerId = query.value(1).toInt();
        m_strDataUpdatedAt = query.value(2).toString();
        m_strClassifyLabels = query.value(3).toString();
        m_strClassifyLabelsAtServer = query.value(4).toString();
        m_strUserName = strUserName;

        m_bNeedUpdateClassify = true;
        parseClassifyTypesList();
        return true;
    }

    return false;
}

void CDatabaseManager::clearLoginInfo()
{
    m_nUserLocalId = 0;
    m_nUserServerId = 0;
    m_strUserName = "";
    m_bLoadAllNoteItems = false;

    clearNoteItems();
    clearClassifyTypesList();
}

void CDatabaseManager::clearNoteItems()
{
    foreach(CNoteItem* pItem, m_lstNoteItems)
    {
        delete pItem;
    }

    m_lstNoteItems.clear();
}

int CDatabaseManager::getUserLocalId() const
{
    return m_nUserLocalId;
}

int CDatabaseManager::getUserServerId() const
{
    return m_nUserServerId;
}

QString CDatabaseManager::getUserName() const
{
    return m_strUserName;
}

bool CDatabaseManager::setDataUpdateAtString(const QString& str)
{
    m_strDataUpdatedAt = str;

    QSqlQuery query(m_db);
    QString strSql = QString("UPDATE users SET update_at = '%1' WHERE id = %2")
            .arg(m_strDataUpdatedAt)
            .arg(G_MainFrame()->getUserLocalId());
    if(!query.exec(strSql))
    {
        QString str = query.lastQuery();
        return false;
    }

    QString str2 = query.lastQuery();
    if(0 == query.numRowsAffected())
        return false;

    return true;
}

QString CDatabaseManager::addData(CNoteItem* pNoteItem)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO notes (user_id, server_id, type, title, share_type, content, content_size, set_at, state, md5) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.bindValue(0, QVariant::fromValue(getUserLocalId())); // UserID
    query.bindValue(1, QVariant::fromValue(pNoteItem->getId()));	// server id
    query.bindValue(2, QVariant::fromValue(pNoteItem->getType()));	// type
    query.bindValue(3, QVariant::fromValue(pNoteItem->getTitle()));	// title
    query.bindValue(4, QVariant::fromValue(pNoteItem->getShareType()));	// share_type
    query.bindValue(5, QVariant::fromValue(pNoteItem->getContent()));	// conten
    query.bindValue(6, QVariant::fromValue(pNoteItem->getContentSize()));	// conten_size
    query.bindValue(7, QVariant::fromValue(pNoteItem->getSetAt().toString(DATE_TIME_FORMAT)));	// DateSet
    query.bindValue(8, QVariant::fromValue(pNoteItem->getState()));	// state
    query.bindValue(9, QVariant::fromValue(pNoteItem->getMd5()));	// md5
    query.exec();

    if(query.numRowsAffected() <= 0)
            return query.lastError().text();

    pNoteItem->setLocalId(query.lastInsertId().toInt());

    // add attachments
    QList<CAttachmentItem*> lstAttachmentItems = pNoteItem->getAttachmentList();
    foreach(CAttachmentItem* pItem, lstAttachmentItems)
    {
        addData(pItem);
    }

    m_lstNoteItems.push_back(pNoteItem);
    connectNoteItem(pNoteItem);

    return QString();
}

void CDatabaseManager::connectNoteItem(CNoteItem* pNoteItem)
{
    if(!pNoteItem)
        return;

    connect(pNoteItem, SIGNAL(selectedChanged()), this, SIGNAL(noteItemSelectionChanged()));
}

QString CDatabaseManager::addData(CAttachmentItem* pAttachItem)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO attachments (user_id, server_id, note_local_id, filename, content, content_size, state, md5) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
    query.bindValue(0, QVariant::fromValue(getUserLocalId())); // UserID
    query.bindValue(1, QVariant::fromValue(pAttachItem->getId()));	// server id
    query.bindValue(2, QVariant::fromValue(pAttachItem->getParentNoteItem()->getLocalId()));	// note local id
    query.bindValue(3, QVariant::fromValue(pAttachItem->getFileName()));	// file name
    query.bindValue(4, QVariant::fromValue(pAttachItem->getContent()), QSql::In | QSql::Binary);	// conten
    query.bindValue(5, QVariant::fromValue(pAttachItem->getContentSize()));	// conten_size
    query.bindValue(6, QVariant::fromValue(pAttachItem->getState()));	// state
    query.bindValue(7, QVariant::fromValue(pAttachItem->getMd5()));	// md5
    query.exec();

    if(query.numRowsAffected() <= 0)
            return query.lastError().text();

    pAttachItem->setLocalId(query.lastInsertId().toInt());
    return QString();
}

QString CDatabaseManager::updateData(CNoteItem* pNoteItem, bool bOnlyState, bool bByServerId, bool bWithoutContent)
{
    CNoteItem* pNoteItemOrg = NULL;
    if(bByServerId)
    {
        pNoteItemOrg = getNoteItem(pNoteItem->getId());
        if(!pNoteItemOrg)
        {
            return QString("No item with id %1.").arg(pNoteItem->getId());
        }
    }
    else
    {
        pNoteItemOrg = getNoteItemByLocalId(pNoteItem->getLocalId());
        if(!pNoteItemOrg)
        {
            return QString("No item with id %1.").arg(pNoteItem->getLocalId());
        }
    }

    QSqlQuery query(m_db);
    if(bOnlyState)
    {
        if(bByServerId)
        {
            query.prepare("UPDATE notes SET state = ? "
                          "WHERE server_id = ?");
            query.bindValue(0, QVariant::fromValue(pNoteItem->getState())); // state
            query.bindValue(1, QVariant::fromValue(pNoteItem->getId())); // server id
        }
        else
        {
            query.prepare("UPDATE notes SET server_id = ?, "
                          "state = ? "
                          "WHERE id = ?");
            query.bindValue(0, QVariant::fromValue(pNoteItem->getId()));	// server_id
            query.bindValue(1, QVariant::fromValue(pNoteItem->getState())); // state
            query.bindValue(2, QVariant::fromValue(pNoteItem->getLocalId())); // local id
        }
    }
    else
    {
        if(bWithoutContent)
        {
            if(bByServerId)
            {
                query.prepare("UPDATE notes SET user_id = ?, "
                              "type = ?, title = ?,"
                              "share_type = ?,"
                              "content_size = ?, md5 = ?, "
                              "set_at = ?, "
                              "state = ? "
                              "WHERE server_id = ?");
                query.bindValue(0, QVariant::fromValue(G_MainFrame()->getUserLocalId()));	// user id
                query.bindValue(1, QVariant::fromValue(pNoteItem->getType()));	// type
                query.bindValue(2, QVariant::fromValue(pNoteItem->getTitle()));	// title
                query.bindValue(3, QVariant::fromValue(pNoteItem->getShareType()));	// share_type
                query.bindValue(4, QVariant::fromValue(pNoteItem->getContentSize()));	// content size
                query.bindValue(5, QVariant::fromValue(pNoteItem->getMd5()));	// md5
                query.bindValue(6, QVariant::fromValue(pNoteItem->getSetAt().toString(DATE_TIME_FORMAT)));	// DateSet
                query.bindValue(7, QVariant::fromValue(pNoteItem->getState())); // state
                query.bindValue(8, QVariant::fromValue(pNoteItem->getId())); // server_id id
            }
            else
            {
                query.prepare("UPDATE notes SET server_id = ?, user_id = ?, "
                              "type = ?, title = ?,"
                              "share_type = ?,"
                              "content_size = :content_size, md5 = ?, "
                              "set_at = ?, "
                              "state = ? "
                              "WHERE id = ?");
                query.bindValue(0, QVariant::fromValue(pNoteItem->getId()));	// server_id
                query.bindValue(1, QVariant::fromValue(G_MainFrame()->getUserLocalId()));	// user id
                query.bindValue(2, QVariant::fromValue(pNoteItem->getType()));	// type
                query.bindValue(3, QVariant::fromValue(pNoteItem->getTitle()));	// title
                query.bindValue(4, QVariant::fromValue(pNoteItem->getShareType()));	// share_type
                query.bindValue(5, QVariant::fromValue(pNoteItem->getContentSize()));	// content size
                query.bindValue(6, QVariant::fromValue(pNoteItem->getMd5()));	// md5
                query.bindValue(7, QVariant::fromValue(pNoteItem->getSetAt().toString(DATE_TIME_FORMAT)));	// DateSet
                query.bindValue(8, QVariant::fromValue(pNoteItem->getState())); // state
                query.bindValue(9, QVariant::fromValue(pNoteItem->getLocalId())); // local id
            }
        }
        else
        {
            if(bByServerId)
            {
                query.prepare("UPDATE notes SET user_id = ?, "
                              "type = ?, title = ?,"
                              "share_type = ?,"
                              "content = ?, content_size = ?, md5 = ?, "
                              "set_at = ?, "
                              "state = ? "
                              "WHERE server_id = ?");
                query.bindValue(0, QVariant::fromValue(G_MainFrame()->getUserLocalId()));	// user id
                query.bindValue(1, QVariant::fromValue(pNoteItem->getType()));	// type
                query.bindValue(2, QVariant::fromValue(pNoteItem->getTitle()));	// title
                query.bindValue(3, QVariant::fromValue(pNoteItem->getShareType()));	// share_type
                query.bindValue(4, QVariant::fromValue(pNoteItem->getContent()));	// content
                query.bindValue(5, QVariant::fromValue(pNoteItem->getContentSize()));	// content size
                query.bindValue(6, QVariant::fromValue(pNoteItem->getMd5()));	// md5
                query.bindValue(7, QVariant::fromValue(pNoteItem->getSetAt().toString(DATE_TIME_FORMAT)));	// DateSet
                query.bindValue(8, QVariant::fromValue(pNoteItem->getState())); // state
                query.bindValue(9, QVariant::fromValue(pNoteItem->getId())); // server_id id
            }
            else
            {
                query.prepare("UPDATE notes SET server_id = ?, user_id = ?, "
                              "type = ?, title = ?,"
                              "share_type = ?,"
                              "content = ?, content_size = ?, md5 = ?, "
                              "set_at = ?, "
                              "state = ? "
                              "WHERE id = ?");
                query.bindValue(0, QVariant::fromValue(pNoteItem->getId()));	// server_id
                query.bindValue(1, QVariant::fromValue(G_MainFrame()->getUserLocalId()));	// user id
                query.bindValue(2, QVariant::fromValue(pNoteItem->getType()));	// type
                query.bindValue(3, QVariant::fromValue(pNoteItem->getTitle()));	// title
                query.bindValue(4, QVariant::fromValue(pNoteItem->getShareType()));	// share_type
                query.bindValue(5, QVariant::fromValue(pNoteItem->getContent()));	// content
                query.bindValue(6, QVariant::fromValue(pNoteItem->getContentSize()));	// content size
                query.bindValue(7, QVariant::fromValue(pNoteItem->getMd5()));	// md5
                query.bindValue(8, QVariant::fromValue(pNoteItem->getSetAt().toString(DATE_TIME_FORMAT)));	// DateSet
                query.bindValue(9, QVariant::fromValue(pNoteItem->getState())); // state
                query.bindValue(10, QVariant::fromValue(pNoteItem->getLocalId())); // local id
            }
        }
    }

    query.exec();

    if(query.numRowsAffected() <= 0)
    {

        QString str = query.lastError().text();
        return query.lastError().text();
    }

    if(pNoteItemOrg != pNoteItem)
    {
        // attachment of org and new
        QList<CAttachmentItem*> lstAttachmentsOrg = pNoteItemOrg->getAttachmentList();
        QList<CAttachmentItem*> lstAttachmentsNew = pNoteItem->getAttachmentList();
        CAttachmentItem* pAttachmentItemNew = NULL;
        // org -> new
        foreach(CAttachmentItem* pItem, lstAttachmentsOrg)
        {
            bool bStateChanged = false;
            if(pItem->getId() <= 0 && pItem->getLocalId() <= 0)
            {
                if(pItem->getState() != STATE_DELETE)
                {
                    pItem->setState(STATE_DELETE);
                    bStateChanged = true;
                }
            }

            if(pItem->getState() == STATE_NORMAL)
            {
                if(pItem->getId() > 0)
                    pAttachmentItemNew = pNoteItem->getAttachmentItem(pItem->getId());
                else if(pItem->getLocalId() > 0)
                    pAttachmentItemNew = pNoteItem->getAttachmentItemByLocalId(pItem->getLocalId());
                if(pAttachmentItemNew)
                {
                    if(pItem->getState() != pAttachmentItemNew->getState())
                    {
                        pItem->setState(pAttachmentItemNew->getState());
                        bStateChanged = true;
                    }
                }
            }

            if(bStateChanged && pItem->getId() > 0)
                updateData(pItem, true);

            if(pItem->getState() == STATE_DELETE)
            {
                // not in server
                if(pItem->getId() <= 0)
                {
                    deleteData(pItem);
                    pItem->getParentNoteItem()->removeAttachmentItem(pItem);
                    delete pItem, pItem = NULL;
                }
            }
        }

        // new -> org
        foreach(CAttachmentItem* pItem, lstAttachmentsNew)
        {
            bool isInOrg = true;
            if(pItem->getState() == STATE_NEW)
            {
                isInOrg = false;
            } else if(pItem->getId() <= 0 && pItem->getLocalId() <= 0)
            {
                isInOrg = false;
            }
            else
            {
                if(pItem->getId() > 0)
                {
                    isInOrg = !!pNoteItemOrg->getAttachmentItem(pItem->getId());
                }
                else if(pItem->getLocalId() > 0)
                {
                    isInOrg = !!pNoteItemOrg->getAttachmentItemByLocalId(pItem->getLocalId());
                }
            }

            // new item is not in origin
            if(!isInOrg)
            {
                CAttachmentItem* pTempItem = new CAttachmentItem(pNoteItemOrg);
                *pTempItem = *pItem;
                pTempItem->setContent(pItem->getContent());
                pNoteItemOrg->addAttachmentItem(pTempItem);
                pTempItem->setState(STATE_NEW);
                addData(pTempItem);
                pTempItem->clearContent();
            }
        }

        // copy to
        pNoteItem->copyTo(pNoteItemOrg, false, true);
    }

//    CNoteItem* pNoteItemLocal = G_getNoteItemByServerId(pNoteItem->getId(), m_lstNoteItems);
//    *(pNoteItemLocal) = *(pNoteItem);

    return QString();
}

QString CDatabaseManager::updateData(CAttachmentItem* pAttachItem, bool bWithoutContent)
{
    QSqlQuery query(m_db);

    if(bWithoutContent)
    {
        query.prepare("UPDATE attachments SET user_id = ?, server_id = ?, note_local_id = ?, filename = ?, content_size = ?, state = ? "
                "WHERE id = ?");
        query.addBindValue(QVariant::fromValue(getUserLocalId())); // UserID
        query.addBindValue(QVariant::fromValue(pAttachItem->getId()));	// server id
        query.addBindValue(QVariant::fromValue(pAttachItem->getParentNoteItem()->getLocalId()));	// note local id
        query.addBindValue(QVariant::fromValue(pAttachItem->getFileName()));	// file name
        query.addBindValue(QVariant::fromValue(pAttachItem->getContentSize()));	// conten_size
        query.addBindValue(QVariant::fromValue(pAttachItem->getState()));	// state
        query.addBindValue(QVariant::fromValue(pAttachItem->getLocalId()));	// local id
    }
    else
    {
        query.prepare("UPDATE attachments SET user_id = ?, server_id = ?, note_local_id = ?, filename = ?, content = ?, content_size = ?, md5 = ?, state = ? "
                "WHERE id = ?");
        query.bindValue(0, QVariant::fromValue(getUserLocalId())); // UserID
        query.bindValue(1, QVariant::fromValue(pAttachItem->getId()));	// server id
        query.bindValue(2, QVariant::fromValue(pAttachItem->getParentNoteItem()->getLocalId()));	// note local id
        query.bindValue(3, QVariant::fromValue(pAttachItem->getFileName()));	// file name
        query.bindValue(4, QVariant::fromValue(pAttachItem->getContent()), QSql::In | QSql::Binary);	// conten
        query.bindValue(5, QVariant::fromValue(pAttachItem->getContentSize()));	// conten_size
        query.bindValue(6, QVariant::fromValue(pAttachItem->getMd5()));	// md5
        query.bindValue(7, QVariant::fromValue(pAttachItem->getState()));	// state
        query.bindValue(8, QVariant::fromValue(pAttachItem->getLocalId()));	// local id
    }

    query.exec();

    if(query.numRowsAffected() <= 0)
    {
        QString str = query.lastError().text();
        return query.lastError().text();
    }

    pAttachItem->clearContent(); // to save memory
    return QString();
}

QString CDatabaseManager::updateAttachmentList(CNoteItem* pNoteItem)
{
    return QString();
}

QString CDatabaseManager::updateAttachments(CNoteItem* pNoteItem)
{
    return QString();
}

QString CDatabaseManager::updateAttachmentIdAndState(int nLocalId, int nId, int nState)
{
    CAttachmentItem* pItem = getAttachmentItemByLocalId(nLocalId);
    if(!pItem)
        return QString(tr("No attachment item."));

    pItem->setId(nId);
    pItem->setState(nState);

    // save data to db
    QSqlQuery query(m_db);
    query.prepare("UPDATE attachments SET server_id = ?, state = ? "
                  "WHERE id = ?");
    query.bindValue(0, QVariant::fromValue(nId)); // server id
    query.bindValue(1, QVariant::fromValue(nState)); // state
    query.bindValue(2, QVariant::fromValue(nLocalId)); // local id

    if(!query.exec())
        return query.lastError().text();

    return QString();
}

QString CDatabaseManager::updateData(const QVector<CNoteItem*>& vecNoteItems)
{
    QString strError;
    for(int i = vecNoteItems.size() - 1; i >= 0; --i)
    {
        strError = updateData(vecNoteItems[i]);
        if(strError.length())
                return strError;
    }

    return QString();
}

QString CDatabaseManager::deleteData(CNoteItem* pNoteItem)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM notes WHERE id = ?");
    query.bindValue(0, QVariant::fromValue(pNoteItem->getLocalId()));
    query.exec();

    if(query.numRowsAffected() < 0)
    {
        QString str = query.lastError().text();
        return query.lastError().text();
    }

    deleteAttachmentItemsByNoteId(pNoteItem->getLocalId());


//    m_lstNoteItems.removeOne(pNoteItem);
//    delete pNoteItem, pNoteItem = NULL;

    return QString();
}

QString CDatabaseManager::deleteData(CAttachmentItem* pItem)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM attachments WHERE id = ?");
    query.bindValue(0, QVariant::fromValue(pItem->getLocalId()));
    query.exec();

    if(query.numRowsAffected() <= 0)
    {
        QString str = query.lastError().text();
        return query.lastError().text();
    }

    return QString();
}

bool CDatabaseManager::deleteNoteItem(int nLocalId)
{
    CNoteItem* pItem = G_getNoteItemByLocalId(nLocalId, m_lstNoteItems);
    if(pItem)
    {
        m_lstNoteItems.removeOne(pItem);
        deleteData(pItem);
        delete pItem, pItem = NULL;
        return true;
    }
    return false;
}

bool CDatabaseManager::deleteAttachmentItem(int nLocalId)
{
    CAttachmentItem* pItem = getAttachmentItemByLocalId(nLocalId);
    if(!pItem)
        return false;

    deleteData(pItem);
    pItem->getParentNoteItem()->removeAttachmentItem(pItem);
    delete pItem, pItem = NULL;
    return true;
}

void CDatabaseManager::deleteNoteItemsByUserId(int nUserId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM notes WHERE user_id = ?");
    query.bindValue(0, QVariant::fromValue(nUserId));
    query.exec();
}

void CDatabaseManager::deleteAttachmentItemsByNoteId(int nNoteLocalId)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM attachments WHERE note_local_id = ?");
    query.bindValue(0, QVariant::fromValue(nNoteLocalId));
    query.exec();
}

void CDatabaseManager::loadAttachmentContent(CAttachmentItem* pItem)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT content FROM attachments WHERE id = ?");
    query.bindValue(0, QVariant::fromValue(pItem->getLocalId()));
    query.exec();

    if(query.next())
    {
        pItem->setContent(query.value(0).toByteArray(), false);
    }

    emit emitMsg(QString("loadAttachmentContent(content length:%1).").arg(pItem->getContentSize()));
}

void CDatabaseManager::loadAllNoteItems()
{
    if(m_bLoadAllNoteItems)
        return;

    m_bLoadAllNoteItems = true;
    QList<CNoteItem*> vecNoteItems;
    GetNoteItems(QDateTime(), QDateTime(), vecNoteItems, false);
}

int CDatabaseManager::GetNoteItems(const QDateTime& dateFrom, const QDateTime& dateTo, QList<CNoteItem*>& vecNoteItems, bool bCheckState)
{
    QString strSQL;
    if((!dateFrom.isValid()) && (!dateTo.isValid()))
    {
        strSQL = QString("SELECT * FROM notes WHERE user_id = %1")
                .arg(G_MainFrame()->getUserLocalId());
    }
    else if((dateFrom.isValid()) && (!dateTo.isValid()))
    {
        strSQL = "SELECT * FROM notes WHERE set_at >= '%1' AND user_id = %2";
        strSQL = strSQL.arg(dateFrom.toString(DATE_TIME_FORMAT))
                .arg(G_MainFrame()->getUserLocalId());
    }
    else if((!dateFrom.isValid()) && (dateTo.isValid()))
    {
        strSQL = "SELECT * FROM notes WHERE set_at <= '%1' AND user_id = %2";
        strSQL = strSQL.arg(dateTo.toString(DATE_TIME_FORMAT))
                        .arg(G_MainFrame()->getUserLocalId());
    }
    else
    {
        strSQL = "SELECT * FROM notes WHERE set_at >= '%1' AND set_at <= '%2' AND user_id = %3";
        strSQL = strSQL.arg(dateFrom.toString(DATE_TIME_FORMAT))
                        .arg(dateTo.toString(DATE_TIME_FORMAT))
                        .arg(G_MainFrame()->getUserLocalId());
    }

    QSqlQuery query(m_db);
    query.exec(strSQL);

    QDateTime datetimeTmp;
    CNoteItem* pItem = NULL;

    while (query.next()) {
        // isn't the current user
//        if(G_MainFrame()->getUserLocalId() != query.value(ID_DB_NOTE_USER_ID).toInt())
//            continue;

        pItem = getNoteItemByLocalId(query.value(ID_DB_NOTE_ID).toInt());
        if(pItem)
        {
            if(bCheckState && !pItem->isShowByState())
                continue;

            vecNoteItems.push_back(pItem);
            continue;
        }

        pItem = new CNoteItem();
        pItem->setLocalId(query.value(ID_DB_NOTE_ID).toInt());
        pItem->setId(query.value(ID_DB_NOTE_SERVER_ID).toInt());
        pItem->setType(query.value(ID_DB_NOTE_TYPE).toInt());
        pItem->setShareType(query.value(ID_DB_NOTE_SHARE_TYPE).toInt());

        QString strSetAt = query.value(ID_DB_NOTE_SET_AT).toString();
        datetimeTmp = QDateTime::fromString(strSetAt, DATE_TIME_FORMAT);
        pItem->setSetAt(datetimeTmp);

        pItem->setTitle(query.value(ID_DB_NOTE_TITLE).toString());
        pItem->setContent(query.value(ID_DB_NOTE_CONTENT).toByteArray());
        pItem->setContentSize(query.value(ID_DB_NOTE_CONTENT_SIZE).toInt());
        pItem->setState(query.value(ID_DB_NOTE_STATE).toInt());
        pItem->setMd5(query.value(ID_DB_NOTE_MD5).toString());
        m_lstNoteItems.push_back(pItem);

        getAttachmentItems(pItem);
        connectNoteItem(pItem);

        if(bCheckState && !pItem->isShowByState())
        {
            delete pItem, pItem = NULL;
            continue;
        }

        vecNoteItems.push_back(pItem);
    }

    return vecNoteItems.size();
}

void CDatabaseManager::getAttachmentItems(CNoteItem* pNoteItem)
{
    if(!pNoteItem)
        return;

    QSqlQuery query(m_db);
    query.prepare("SELECT id,server_id,filename,content_size,md5,state  FROM attachments WHERE note_local_id = ?");
    query.bindValue(0, QVariant::fromValue(pNoteItem->getLocalId())); // UserID
    query.exec();

    CAttachmentItem* pItem = NULL;
    while (query.next()) {
        pItem = new CAttachmentItem(pNoteItem);
        pItem->setLocalId(query.value(ID_DB_ATTACHMENT_ID).toInt());
        pItem->setId(query.value(ID_DB_ATTACHMENT_SERVER_ID).toInt());
        pItem->setFileName(query.value(ID_DB_ATTACHMENT_FILE_NAME).toString());
        pItem->setContentSize(query.value(ID_DB_ATTACHMENT_CONTENT_SIZE).toUInt());
        pItem->setMd5(query.value(ID_DB_ATTACHMENT_MD5).toString());
        pItem->setState(query.value(ID_DB_ATTACHMENT_STATE).toInt());
        pNoteItem->addAttachmentItem(pItem);
    } 
}

int CDatabaseManager::GetNoteItems(const QDate& today, QList<CNoteItem*>& lstNoteItems, bool bCheckState)
{
    QDateTime dateFrom, dateTo;
    dateFrom.setDate(today);
    dateTo.setDate(today);
    dateTo.setTime(QTime(23,59,59));
    return GetNoteItems(dateFrom, dateTo, lstNoteItems, bCheckState);
}

int CDatabaseManager::GetItemCount()
{
    QString strSQL = "SELECT * FROM Notes";
    QSqlQuery query(m_db);
    query.exec(strSQL);

    return query.size();
}

int CDatabaseManager::GetCountOfNoteItems()
{
    return m_lstNoteItems.count();
}

int CDatabaseManager::GetAmountOfItems()
{
    int nAmount = 0;
    foreach(CNoteItem* pItem, m_lstNoteItems)
    {
        nAmount += pItem->getAmountOfAttachments();
    }

    nAmount += m_lstNoteItems.count();
    return nAmount;
}

bool CDatabaseManager::getNoteItemListMonth(int nYear, int nMonth, QList<CNoteItem*>& lstNoteItems, bool bStateCheck)
{
    QDate dtMonth(nYear,nMonth,1);
    if(m_lstMonthsFromLocal.indexOf(dtMonth) < 0)
    {
        m_lstMonthsFromLocal.append(dtMonth);

        QDateTime dtFrom, dtEnd;
        QString strTmp = QString("%1-%2-01 0:0:0").arg(nYear).arg(nMonth);
        dtFrom = QDateTime::fromString(strTmp, DATE_TIME_FORMAT_NZERO);
        dtEnd = dtFrom.addMonths(1).addSecs(-1);
        GetNoteItems(dtFrom, dtEnd, lstNoteItems, bStateCheck);
//        m_lstNoteItems.append(lstNoteItems);

        return true;
    }

    foreach(CNoteItem* pItem, m_lstNoteItems)
    {
        if(pItem->getSetAt().date().year() == nYear &&
                pItem->getSetAt().date().month() == nMonth)
        {
            if(bStateCheck && !pItem->isShowByState())
                continue;

            lstNoteItems.append(pItem);
        }
    }

    return true;
}

void CDatabaseManager::getMinMaxTimeOfNotes(QString& strMinTime, QString& strMaxTime) const
{
    QString strSql = QString("SELECT min(set_at) FROM notes WHERE user_id = %1").arg(G_MainFrame()->getUserLocalId());

    QSqlQuery query(m_db);
    query.exec(strSql);

    if(query.next())
        strMinTime = query.value(0).toString();
    else
        return;

    strSql = QString("SELECT max(set_at) FROM notes WHERE user_id = %1").arg(G_MainFrame()->getUserLocalId());

    QSqlQuery query2(m_db);
    query2.exec(strSql);

    if(query2.next())
        strMaxTime = query2.value(0).toString();
}

CNoteItem* CDatabaseManager::getNoteItem(int nServerId) const
{
    foreach(CNoteItem* pNoteItem,  m_lstNoteItems)
    {
        if(nServerId == pNoteItem->getId())
            return pNoteItem;
    }

    return NULL;
}

CNoteItem* CDatabaseManager::getNoteItemByLocalId(int nLocalId) const
{
    foreach(CNoteItem* pNoteItem,  m_lstNoteItems)
    {
        if(nLocalId == pNoteItem->getLocalId())
            return pNoteItem;
    }

    return NULL;
}

CAttachmentItem* CDatabaseManager::getAttachmentItem(int nId) const
{
    CAttachmentItem* pAttachmentItem = NULL;
    foreach(CNoteItem* pNoteItem,  m_lstNoteItems)
    {
        pAttachmentItem = pNoteItem->getAttachmentItem(nId);
        if(pAttachmentItem)
            return pAttachmentItem;
    }

    return NULL;
}

CAttachmentItem* CDatabaseManager::getAttachmentItemByLocalId(int nLocalId) const
{
    CAttachmentItem* pAttachmentItem = NULL;
    foreach(CNoteItem* pNoteItem,  m_lstNoteItems)
    {
        pAttachmentItem = pNoteItem->getAttachmentItemByLocalId(nLocalId);
        if(pAttachmentItem)
            return pAttachmentItem;
    }

    return NULL;
}

QByteArray CDatabaseManager::getAttachmentContent(int nLocalId)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT content FROM attachments WHERE id = ?");
    query.bindValue(0, QVariant::fromValue(nLocalId));
    query.exec();

    if(query.next())
        return query.value(0).toByteArray();

    return QByteArray();
}

QList<CNoteItem*> CDatabaseManager::getNotesByString(const QString& strText, bool bCheckState, int nRuleType)
{
    loadAllNoteItems();
    QList<CNoteItem*> lstNoteItems;

    QString strSQL = QString("SELECT * FROM notes WHERE title LIKE '%%1%' OR content LIKE '%%1%' ORDER BY set_at DESC").arg(strText);
    switch(nRuleType)
    {
    case INCLUDE_TYPE_LEADING:
        strSQL = QString("SELECT * FROM notes WHERE title LIKE '%1%' OR content LIKE '%1%' ORDER BY set_at DESC").arg(strText);
        break;
    case INCLUDE_TYPE_ENDING:
        strSQL = QString("SELECT * FROM notes WHERE title LIKE '%%1' OR content LIKE '%%1' ORDER BY set_at DESC").arg(strText);
        break;
    default:
        strSQL = QString("SELECT * FROM notes WHERE title LIKE '%%1%' OR content LIKE '%%1%' ORDER BY set_at DESC").arg(strText);
        break;
    }

    QSqlQuery query(m_db);
    query.exec(strSQL);

    CNoteItem* pNoteItem = NULL;
    while(query.next())
    {
        pNoteItem = getNoteItemByLocalId(query.value(ID_DB_NOTE_ID).toInt());

        if(pNoteItem)
        {
            if(bCheckState && !pNoteItem->isShowByState())
                continue;

            lstNoteItems.push_back(pNoteItem);
        }
    }

    return lstNoteItems;
}

void CDatabaseManager::updateClassifyLabels(const QString& strLabels, const QString& strLabelsServer)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET classify_labels = ?, "
                  "classify_labels_at_server = ? "
                  "WHERE id = ?");
    query.bindValue(0, QVariant::fromValue(strLabels));	// classify_labels
    query.bindValue(1, QVariant::fromValue(strLabelsServer)); // classify_labels_at_server
    query.bindValue(2, QVariant::fromValue(m_nUserLocalId)); // local id
    query.exec();

    if(query.numRowsAffected() <= 0)
            return;

    m_bNeedUpdateClassify = m_strClassifyLabels != strLabels;

    m_strClassifyLabels = strLabels;
    m_strClassifyLabelsAtServer = strLabelsServer;
    parseClassifyTypesList();
}

QList<ST_CLASSIFY_TYPE*>& CDatabaseManager::getClassifyTypesList()
{
    return m_lstClassifyTypes;
}

QStringList CDatabaseManager::getClassifyTypesStringList()
{
    QStringList lstStrs;
    foreach(ST_CLASSIFY_TYPE* pType, m_lstClassifyTypes)
    {
        lstStrs.push_back(g_getClassifyStringName(pType));
    }

    return lstStrs;
}

void CDatabaseManager::parseClassifyTypesList()
{
    if(!m_bNeedUpdateClassify)
        return;

    clearClassifyTypesList();

    // ^abc\ ; def ; ghi$
    QChar cCurrent;
    int nPosStart = 0, nPosEnd = 0;
    QString str;
    for(int i = 0; i < m_strClassifyLabels.length(); ++i)
    {
        cCurrent = m_strClassifyLabels.at(i);
        if('\\' == cCurrent)
        {
            if(i < (m_strClassifyLabels.length() - 1))
            {
                if(';' == m_strClassifyLabels.at(i+1) ||
                        '\\' == m_strClassifyLabels.at(i+1))
                {
                    m_strClassifyLabels.remove(i,1);
                }
            }
        }
        else if(';' == cCurrent ||
                i == (m_strClassifyLabels.length() - 1))
        {
            if(';' == cCurrent)
                nPosEnd = i;
            else
                nPosEnd = i + 1;

            str = m_strClassifyLabels.mid(nPosStart, nPosEnd - nPosStart);
            str = str.replace("\\;", ";");
            ST_CLASSIFY_TYPE* pType = new ST_CLASSIFY_TYPE;
            g_parseClassifyString(str, pType);
            m_lstClassifyTypes.push_back(pType);

            nPosStart = i+1;
        }
    }


    m_bNeedUpdateClassify = false;
}

void CDatabaseManager::clearClassifyTypesList()
{
    foreach(ST_CLASSIFY_TYPE* pType, m_lstClassifyTypes)
    {
        delete pType, pType = NULL;
    }
    m_lstClassifyTypes.clear();
}

void CDatabaseManager::clearSelection()
{
    foreach(CNoteItem* pNoteItem, m_lstNoteItems)
    {
        pNoteItem->setSelected(false);
    }
}

CNoteItem* CDatabaseManager::getSelectedNoteItem()
{
    foreach(CNoteItem* pNoteItem, m_lstNoteItems)
    {
        if(pNoteItem->isSelected())
            return pNoteItem;
    }

    return NULL;
}
