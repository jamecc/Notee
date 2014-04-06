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

#include "SyncThread.h"
#include "NetworkManager.h"
#include "MainFrame.h"
#include "NoteItem.h"
#include "predefine.h"

CSyncThread::CSyncThread(QObject *parent) :
    QThread(parent)
{
    m_nAmountOfNotes = 0;
    m_nNumberPerTimes = 10;
    m_nCurrentOffsetOfNotes = 0;
    m_nCurrentPosOfNotes = 0;
    m_pNetworkManager = NULL;
}

QString CSyncThread::getValueByKey(const QString& strKey) const
{
    return m_jsonParser.getValueByKey(strKey);
}

QString CSyncThread::getErrorString()
{
    return m_jsonParser.getValueByKey("error");
}

int CSyncThread::getResultValue() const
{
    QString strValue = m_jsonParser.getValueByKey("result");
    strValue = strValue.trimmed();

    if(strValue.isEmpty())
        return RET_OK;

    return strValue.toInt();
}

void CSyncThread::onNetworkReplyFinished(QNetworkReply* pNetworkReply)
{
    // parse
    m_arrBuffer.append(pNetworkReply->readAll());

    m_jsonParser.clearJsonItems();
    if(RETURN_TYPE_ERROR == parseBuffer())
    {
        emit syncProgressFinished();
        exit();
    }
}

// 0 continue, 1 error, 2 finish
int CSyncThread::parseBuffer()
{
    CDatabaseManager* pLocalDbManager = G_MainFrame()->getLocalDbManager();

    if(m_jsonParser.parse(m_arrBuffer))
    {
        m_strAction = m_jsonParser.getValueByKey("action");
        // amount
        if(URL_GET_AMOUNT_OF_NOTES == m_strAction)
        {
            if(RET_OK != getResultValue())
            {
                emit failToGetAmountOfNotes(getErrorString());
                return RETURN_TYPE_ERROR;
            }

            m_bGotAmountOfNotes = true;
            QString strAmount = m_jsonParser.getValueByKey("amount");
            if(!strAmount.isEmpty())
                m_nAmountOfNotes = strAmount.toInt();
            else
                m_nAmountOfNotes = 0;

            emit signalSyncProgressMax(pLocalDbManager->GetCountOfNoteItems() + m_nAmountOfNotes);

            m_pNetworkManager->getNoteItemListByOffset(m_nCurrentOffsetOfNotes, m_nNumberPerTimes);
        }
        // note list
        else if(URL_GET_NOTE_LIST_OFFSET == m_strAction)
        {
            if(RET_OK != getResultValue())
            {
                emit failToGetNoteList(getErrorString());
                return RETURN_TYPE_ERROR;
            }

            m_pNetworkManager->clearNoteItems();
            CJsonItem* pItem = m_jsonParser.getItem("notes", false);
            m_pNetworkManager->parseJsonItem2NoteItem(pItem);

            syncLocalAndServerDbOffset();
        }
        // modify
        else if(URL_INSERT_NOTE_ITEM == m_strAction ||
                URL_UPDATE_NOTE_ITEM == m_strAction ||
                URL_DELETE_NOTE_ITEM == m_strAction)
        {
            bool bInsert = (URL_INSERT_NOTE_ITEM == m_strAction);
            bool bUpdate = (URL_UPDATE_NOTE_ITEM == m_strAction);

            if(RET_OK != getResultValue())
            {
                if(bInsert)
                {
                    emit failToInsertNoteItem(getValueByKey("note_local_id"), getErrorString());
                }
                else if(bUpdate)
                {
                    emit failToUpdateNoteItem(getValueByKey("note_id"), getErrorString());
                }
                else
                {
                    emit failToDeleteNoteItem(getValueByKey("note_id"), getErrorString());
                }
                return RETURN_TYPE_ERROR;
            }

            if(bInsert)
            {
                QString strLocalId = getValueByKey("note_local_id").trimmed();
                QString strServerId = getValueByKey("note_id").trimmed();
                if(strLocalId.isEmpty())
                {
                    emit emitError(tr("No local note id"));
                    return RETURN_TYPE_ERROR;
                }

                if(strServerId.isEmpty())
                {
                    emit emitError(tr("No node id on server"));
                    return RETURN_TYPE_ERROR;
                }
                CNoteItem* pNoteItemLocal = pLocalDbManager->getNoteItemByLocalId(strLocalId.toInt());
                if(!pNoteItemLocal)
                {
                    emit emitError(QString(tr("Cannot find the note with id '%1'")).arg(strLocalId));
                    return RETURN_TYPE_ERROR;
                }
                pNoteItemLocal->setId(strServerId.toInt());
                pNoteItemLocal->setState(STATE_NORMAL);
                G_MainFrame()->updateNoteItemM(pNoteItemLocal, true, false);
            }
            else if(bUpdate)
            {
                QString strServerId = getValueByKey("note_id").trimmed();
                if(strServerId.isEmpty())
                {
                    emit emitError(tr("No node id on server"));
                    return RETURN_TYPE_ERROR;
                }
                CNoteItem* pNoteItemLocal = pLocalDbManager->getNoteItem(strServerId.toInt());
                if(!pNoteItemLocal)
                {
                    emit emitError(QString(tr("Cannot find the note with id '%1'")).arg(strServerId));
                    return RETURN_TYPE_ERROR;
                }
                pNoteItemLocal->setState(STATE_NORMAL);
                G_MainFrame()->updateNoteItemM(pNoteItemLocal, true, true);
            }
            else
            {
                QString strServerId = getValueByKey("note_id").trimmed();
                if(strServerId.isEmpty())
                {
                    emit emitError(tr("No node id on server"));
                    return RETURN_TYPE_ERROR;
                }
                CNoteItem* pNoteItemLocal = pLocalDbManager->getNoteItem(strServerId.toInt());
                if(!pNoteItemLocal)
                {
                    emit emitError(QString(tr("Cannot find the note with id '%1'")).arg(strServerId));
                    return RETURN_TYPE_ERROR;
                }

                G_MainFrame()->deleteNoteItemM(pNoteItemLocal);
            }
            m_nCurrentPosOfNotes++;
            emit signalSyncProgressPos(m_nCurrentPosOfNotes);
        }
        /*
        // note content
        else if(URL_GET_NOTE_CONTENT == m_strAction)
        {
            if(RET_OK != getResultValue())
            {
                emit failToGetNoteContent(getValueByKey("note_id"), getErrorString());
                return RETURN_TYPE_ERROR;
            }

            QString strServerId = getValueByKey("note_id").trimmed();
            if(strServerId.isEmpty())
            {
                emit emitError(tr("No node id on server"));
                return RETURN_TYPE_ERROR;
            }
            CNoteItem* pNoteItemLocal = pLocalDbManager->getNoteItem(strServerId.toInt());
            if(!pNoteItemLocal)
            {
                emit emitError(QString(tr("Cannot find the note with id '%1'")).arg(strServerId));
                return RETURN_TYPE_ERROR;
            }

            pNoteItemLocal->setContent(QString::fromAscii(m_pNetworkManager->getLeftContent()).toUtf8());
            pNoteItemLocal->setState(STATE_NORMAL);
            G_MainFrame()->updateNoteItemM(pNoteItemLocal, false, true, false);
            m_nCurrentPosOfNotes++;
            emit signalSyncProgressPos(m_nCurrentPosOfNotes);
        }
        */
        // unknown
        else
        {
            emit emitError(tr("Unknown action") + QString(" \"%1\"").arg(m_strAction));
        }
    }
    else
    {
        emit emitError(m_jsonParser.getLastError());
    }

    return RETURN_TYPE_CONTINUE;
}

void CSyncThread::run()
{
#ifdef NOTEE_DEBUG
    {
        /*
        QFile output("C:\\json.txt");
        output.open(QIODevice::WriteOnly | QIODevice::Text);
        output.write(arrBuffer);
        output.flush();
        */
    }
#endif

    emit syncProgressBegin();

    if(!m_pNetworkManager)
    {
        m_pNetworkManager = new CNetworkManager(this);
 //       QThread* pThread = this;
        m_pNetworkManager->moveToThread(this);
        connect(m_pNetworkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onNetworkReplyFinished(QNetworkReply*)));
    }

    // before sync
    beforeSync();

    // get amount of notes
    m_pNetworkManager->getAmountOfNotesFromServer();
//    pNetworkReply->moveToThread(this);
//    connectFinished(pNetworkReply);
    exec();
}

void CSyncThread::connectFinished(QNetworkReply* pNetworkReply)
{
    if(pNetworkReply)
    {
        connect(pNetworkReply, SIGNAL(finished()), this, SIGNAL(networkReplyFinished()));
    }
}

// 0 continue, 1 error, 2 finish
void CSyncThread::syncLocalAndServerDbOffset()
{
    /*
    emit signalSyncProgressPos(m_nCurrentPosOfNotes);

    QList<CNoteItem*>& lstNoteItemsFromServer = m_pNetworkManager->getNoteItemsFromServer();

    CNoteItem* pItem2 = NULL;
    QList<CNoteItem*> addedNoteItems;
    CDatabaseManager* pLocalDbManager = G_MainFrame()->getLocalDbManager();
    QList<CNoteItem*>& lstNoteItemsFromLocal = pLocalDbManager->getNoteItemList();

    // server -> local
    foreach(CNoteItem* pItemServer, lstNoteItemsFromServer)
    {
        pItem2 = G_getNoteItemByServerId(pItemServer->getId(), lstNoteItemsFromLocal);
        if(pItem2)
        {
            if(!pItemServer->equalTo(pItem2))
            {
                // update local
                if(pItem2->getState() == STATE_NORMAL)
                {
                    if(pItem2->getMd5() == pItemServer->getMd5())
                    {
                        pItem2->setState(STATE_NORMAL);
                        m_nCurrentPosOfNotes++;
                        emit signalSyncProgressPos(m_nCurrentPosOfNotes);
                    }
                    else
                    {
                        pItem2->setState(STATE_MODIFY_SERVER);
                        QNetworkReply* pNetworkReply = m_pNetworkManager->getNoteItemContent(pItemServer->getId());
                        connectFinished(pNetworkReply);
                    }
                    pItemServer->setLocalId(pItem2->getLocalId());
                    G_MainFrame()->updateNoteItemM(pItemServer, false, true);
                }
                // update server
                else
                {
                    QNetworkReply* pNetworkReply = m_pNetworkManager->updateNoteItem(pItem2);
                    connectFinished(pNetworkReply);
                }
            }
        }
        // server item cannot be found in local
        else
        {
            pItemServer->setState(STATE_NEW_SERVER);
            pItemServer->setFoundIt(true);
            G_MainFrame()->insertNoteItemM(pItemServer);
            addedNoteItems.push_back(pItemServer);
            QNetworkReply* pNetworkReply = m_pNetworkManager->getNoteItemContent(pItemServer->getId());
            connectFinished(pNetworkReply);
 //           refreshDayWidget(pItemServer->getSetAt().date());
        }
    }

    // remove added item for server list
    int nSizeFromServer = lstNoteItemsFromServer.size();
    foreach(CNoteItem* pItem, addedNoteItems)
    {
        for(int i = lstNoteItemsFromServer.size() - 1; i >= 0; i--)
        {
            if(lstNoteItemsFromServer.at(i) == pItem)
            {
                lstNoteItemsFromServer.removeAt(i);
                break;
            }
        }
    }

    // local -> server
    foreach(CNoteItem* pItemLocal, lstNoteItemsFromLocal)
    {
        pItem2 = G_getNoteItemByServerId(pItemLocal->getId(), lstNoteItemsFromServer);
        if(pItem2)
        {
            // found it in server
            pItemLocal->setFoundIt(true);

            switch(pItemLocal->getState())
            {
            case STATE_DELETE:
            {
                QNetworkReply* pNetworkReply = m_pNetworkManager->deleteNoteItem(pItem2);
                connectFinished(pNetworkReply);
            }
            default:
                break;
            }
        }
    }

    // no next group
    if(nSizeFromServer < m_nNumberPerTimes || 0 == nSizeFromServer)
    {
        // after sync
        afterSync();
        emit syncProgressFinished();
        exit();
        return;
    }

    // get next group of note items
    m_nCurrentOffsetOfNotes += m_nNumberPerTimes;

    QNetworkReply* pNetworkReply = m_pNetworkManager->getNoteItemListByOffset(m_nCurrentOffsetOfNotes, m_nNumberPerTimes);
    connectFinished(pNetworkReply);
    */
}

void CSyncThread::beforeSync()
{
    CDatabaseManager* pLocalDbManager = G_MainFrame()->getLocalDbManager();
    pLocalDbManager->loadAllNoteItems();
    QList<CNoteItem*>& lstNoteItems = pLocalDbManager->getNoteItemList();

    foreach(CNoteItem* pItem, lstNoteItems)
    {
        pItem->setFoundIt(false);
    }

    m_nCurrentOffsetOfNotes = 0;
}

void CSyncThread::afterSync()
{
    CDatabaseManager* pLocalDbManager = G_MainFrame()->getLocalDbManager();

    QList<CNoteItem*> lstNoteItemsUnfoundIt;
    QList<CNoteItem*>& lstNoteItemsLocal = pLocalDbManager->getNoteItemList();
    foreach(CNoteItem* pItem, lstNoteItemsLocal)
    {
        if(!pItem->isFoundIt())
        {
            switch(pItem->getState())
            {
            case STATE_NEW:
            {
                m_pNetworkManager->insertNoteItem(pItem);

                break;
            }
            case STATE_DELETE:
            case STATE_MODIFY:
            case STATE_NORMAL:
            default:
                lstNoteItemsUnfoundIt.push_back(pItem);
                break;
            }
        }
    }

    QDate dtRefresh;
    int nLocalNoteItemId;
    foreach(CNoteItem* pItem, lstNoteItemsUnfoundIt)
    {
        dtRefresh = pItem->getSetAt().date();
        nLocalNoteItemId = pItem->getLocalId();
        G_MainFrame()->deleteNoteItemM(pItem);
    }

    pLocalDbManager->setDataUpdateAtString(m_strDataUpdatedAt);
//    G_MainFrame()->afterSync();
}

