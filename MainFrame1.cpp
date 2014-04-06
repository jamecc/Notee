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
#include "NoteItem.h"
#include "CommonFunctions.h"
#include "AttachmentItem.h"
#include "FileDownload.h"
#include "MainFrame.h"
#include "DlgFind.h"
#include "CalendarView.h"

void CMainFrame::onServerDbManagerReadyReadyfinished()
{
    QString strActionValue = m_serverDbManager.getActionValue();
    addOutputString(tr("Action: ") + strActionValue);
    if(URL_REGISTER == strActionValue ||
            URL_LOGIN == strActionValue)
    {
        if(WORKING_TYPE_LOGIN == m_nWorkingType ||
           WORKING_TYPE_REGISTER == m_nWorkingType)
        {
            m_nWorkingType = WORKING_TYPE_IDLE;
        }
        m_pStatusBar->showMessage("");

        if(m_serverDbManager.getResultValue() != RET_OK)
        {
            m_localDbManager.deleteUser(m_serverDbManager.getValueByKey("user_name"));
            addOutputString(tr("Error: ") + m_serverDbManager.getErrorString());
            showBalloonTip(tr("Error: ") + m_serverDbManager.getErrorString()
                           , tr("Error"));
        }
        else
        {
            m_nLoginUserIdServer = m_serverDbManager.getUserId();
            if(m_localDbManager.getUserLocalId(m_strLoginUserNameTmp) > 0)
            {
                m_localDbManager.updataUser(m_localDbManager.getUserLocalId(), m_nLoginUserIdServer, m_strLoginUserNameTmp, m_strLoginPwdTmp);
            }
            else
            {
                QString strError = m_localDbManager.addUser(m_strLoginUserNameTmp, m_strLoginPwdTmp, m_nLoginUserIdServer);
                if(strError.isEmpty())
                {
                    m_pCalendarView->refreshAllDayWidgets();
                }
                else
                    addOutputString(tr("Error: ") + strError);
            }

            // is login
            if(URL_LOGIN == strActionValue)
            {
                onTimeOutUserPulse();
                m_timerUserPulse.start();
                checkClassifyStringServer(m_serverDbManager.getClassifyString());
                checkAllLocalNoteStatus();
            }
        }

        updateControlState();
    }
    else if(URL_FORGOT_PWD == strActionValue)
    {
        if(RET_OK == m_serverDbManager.getResultValue())
        {
            QString strEmail = m_serverDbManager.getValueByKey("email");
            showBalloonTip(QString(tr("The link of modify password is sent to the email \"%1\".")).arg(strEmail)
                           , tr("Success"));
        }
        else
        {
            QString strErr = m_serverDbManager.getErrorString();
            showBalloonTip(QString(tr("Error about forgot password: \"%1\".")).arg(strErr)
                           , tr("Error"));
        }
    }
    else if(URL_GET_NOTE_LIST_OFFSET == strActionValue)
    {
        if(!isLogin())
            return;

        if(RET_OK == m_serverDbManager.getResultValue())
        {
            syncLocalAndServerDbOffset();
        }
        else
        {
            addOutputString(tr("Error: ") + m_serverDbManager.getErrorString());
            m_nWorkingType = WORKING_TYPE_IDLE;
            doProcessAction();
        }
    }
    else if(URL_GET_AMOUNT_OF_NOTES == strActionValue)
    {
        if(!isLogin())
            return;

        if(RET_OK == m_serverDbManager.getResultValue())
        {
            m_bAllItemsNormalStatus = true;
            m_strDataUpdatedAt = m_serverDbManager.getDataUpdatedAtString();
            syncLocalAndServerDb();
        }
        else
        {
            addOutputString(tr("Error: ") + m_serverDbManager.getErrorString());
            m_nWorkingType = WORKING_TYPE_IDLE;
            doProcessAction();
        }
    }
    // note
    else if(URL_INSERT_NOTE_ITEM == strActionValue ||
            URL_UPDATE_NOTE_ITEM == strActionValue ||
            URL_DELETE_NOTE_ITEM == strActionValue)
    {
        if(!isLogin())
            return;

        bool bInsert = (URL_INSERT_NOTE_ITEM == strActionValue);
        bool bUpdate = (URL_UPDATE_NOTE_ITEM == strActionValue);
        bool bDelete = (URL_DELETE_NOTE_ITEM == strActionValue);

        if(RET_OK == m_serverDbManager.getResultValue())
        {
            QString strNoteLocalId = m_serverDbManager.getValueByKey("note_local_id");
            CNoteItem* pNoteItem = m_localDbManager.getNoteItemByLocalId(strNoteLocalId.toInt());

            if(pNoteItem)
            {
                QDate dtNeedUpdate = pNoteItem->getSetAt().date();
                // insert, update
                if(bInsert || bUpdate)
                {
                    QString strNoteServerId = m_serverDbManager.getValueByKey("note_id");

                    pNoteItem->setState(STATE_NORMAL);
                    pNoteItem->setProcessState(STATE_PS_NORMAL);
                    if(bInsert)
                        pNoteItem->setId(strNoteServerId.toInt());
                    updateNoteItemM(pNoteItem, true);
                    // non nomral
                    if(!pNoteItem->isWholeNormalState())
                    {
                        addProcessingNoteItem(pNoteItem);
                    }
                }
                // delete
                else if(bDelete)
                {
                    m_localDbManager.deleteNoteItem(pNoteItem->getLocalId());
                }
                refreshDayWidget(dtNeedUpdate);

                m_pDlgFind->afterAddNoteItem();
            }

            processingNoteItems();
        }
        else
        {
            addOutputString(tr("Error: ") + m_serverDbManager.getErrorString());
        }
    }
    // note item
    else if(URL_GET_NOTE_ITEM == strActionValue)
    {
        if(!isLogin())
            return;

        QString strNoteServerId = m_serverDbManager.getValueByKey("id");
        CNoteItem* pNoteItem = m_localDbManager.getNoteItem(strNoteServerId.toInt());
        if(pNoteItem)
        {
            bool bNeedRefresh = (STATE_NORMAL != pNoteItem->getState());
            pNoteItem->setState(STATE_NORMAL);
            pNoteItem->setProcessState(STATE_PS_NORMAL);

            QString strMd5 = m_serverDbManager.getValueByKey("md5");
            uint nSize = m_serverDbManager.getValueByKey("content_size").toUInt();

            pNoteItem->setType(m_serverDbManager.getValueByKey("type").toInt());
            pNoteItem->setShareType(m_serverDbManager.getValueByKey("share_type").toInt());
            pNoteItem->setTitle(m_serverDbManager.getValueByKey("title"));

            pNoteItem->setSetAt(QDateTime::fromString(m_serverDbManager.getValueByKey("set_at"), DATE_TIME_FORMAT));

            if(nSize > 0)
                // pNoteItem->setContent(QByteArray::fromBase64(m_serverDbManager.getValueByKey("content").toAscii()), strMd5.length() == 0);
                pNoteItem->setContent(m_serverDbManager.getValueByKey("content").toUtf8(), strMd5.length() == 0);
            else
                pNoteItem->setContent(QByteArray());
            if(strMd5.length())
                pNoteItem->setMd5(strMd5);
            updateNoteItemM(pNoteItem, false, true, false);
            refreshDayWidget(pNoteItem->getSetAt().date());
            if(bNeedRefresh)
            {
                m_pDlgFind->afterAddNoteItem();
            }
        }
    }
    /*
    // note content
    else if(URL_GET_NOTE_CONTENT == strActionValue)
    {
        if(!isLogin())
            return;

        QString strNoteServerId = m_serverDbManager.getValueByKey("note_id");
        QString strMd5 = m_serverDbManager.getValueByKey("md5");
        uint nSize = m_serverDbManager.getValueByKey("content_size").toUInt();
        CNoteItem* pNoteItem = m_localDbManager.getNoteItem(strNoteServerId.toInt());
        if(pNoteItem)
        {
            bool bNeedRefresh = (STATE_NORMAL != pNoteItem->getState());
            pNoteItem->setState(STATE_NORMAL);
            pNoteItem->setProcessState(STATE_PS_NORMAL);
            if(nSize > 0)
                pNoteItem->setContent(QByteArray::fromBase64(m_serverDbManager.getValueByKey("content").toAscii()), strMd5.length() == 0);
            else
                pNoteItem->setContent(QByteArray());
            if(strMd5.length())
                pNoteItem->setMd5(strMd5);
            updateNoteItemM(pNoteItem, false, true, false);
            refreshDayWidget(pNoteItem->getSetAt().date());
            if(bNeedRefresh)
            {
                m_pDlgFind->afterAddNoteItem();
            }
        }
    }
    */
    // attachment
    /*
    else if(URL_GET_ATTACHMENT_ITEM == strActionValue)
    {
        if(!isLogin())
            return;

        QString strAttachmentId = m_serverDbManager.getValueByKey("attachment_id");
        QString strMd5 = m_serverDbManager.getValueByKey("md5");
        CAttachmentItem* pAttachmentItem = m_localDbManager.getAttachmentItem(strAttachmentId.toInt());

        if(!pAttachmentItem)
            return;

        pAttachmentItem->setContent(QByteArray::fromBase64(m_serverDbManager.getLeftContent()), false);
        if(strMd5.length() > 0)
            pAttachmentItem->setMd5(strMd5);
        pAttachmentItem->setState(STATE_NORMAL);
        m_localDbManager.updateData(pAttachmentItem, false);
        if(pAttachmentItem->getParentNoteItem()->isWholeNormalState())
            doProcessAction();
    }
    else if(URL_INSERT_ATTACHMENT_ITEM == strActionValue)
    {
        if(!isLogin())
            return;

        QString strAttachmentLocalId = m_serverDbManager.getValueByKey("local_id");
        QString strAttachmentId = m_serverDbManager.getValueByKey("id");
        CAttachmentItem* pAttachmentItem = m_localDbManager.getAttachmentItemByLocalId(strAttachmentLocalId.toInt());
        if(pAttachmentItem)
        {
            pAttachmentItem->setId(strAttachmentId.toInt());
            pAttachmentItem->setState(STATE_NORMAL);
            pAttachmentItem->setProcessState(STATE_PS_NORMAL);
            m_localDbManager.updateData(pAttachmentItem, true);
            if(pAttachmentItem->getParentNoteItem()->isWholeNormalState())
                doProcessAction();
        }
    }
    */
    else if(URL_UPLOAD_ATTACHMENT_ITEM == strActionValue)
    {
        QString strAttachmentLocalId = m_serverDbManager.getValueByKey("local_id");
        QString strAttachmentId = m_serverDbManager.getValueByKey("id");
        CAttachmentItem* pAttachmentItem = m_localDbManager.getAttachmentItemByLocalId(strAttachmentLocalId.toInt());
        if(pAttachmentItem)
        {
            pAttachmentItem->setId(strAttachmentId.toInt());
            pAttachmentItem->setState(STATE_NORMAL);
            pAttachmentItem->setProcessState(STATE_PS_NORMAL);
            m_localDbManager.updateData(pAttachmentItem, true);
            if(pAttachmentItem->getParentNoteItem()->isWholeNormalState())
                doProcessAction();
        }

        addOutputString("Upload reply: " + m_serverDbManager.getValueByKey("status"));
 //       addOutputString("Upload reply: " + m_serverDbManager.getValueByKey("status") + ", " + m_serverDbManager.getResultString());
    }
    else if(URL_DELETE_ATTACHMENT_ITEM == strActionValue)
    {
        if(!isLogin())
            return;

        QString strAttachmentLocalId = m_serverDbManager.getValueByKey("local_id");
        CAttachmentItem* pAttachmentItem = m_localDbManager.getAttachmentItemByLocalId(strAttachmentLocalId.toInt());
        if(!pAttachmentItem)
            return;

        //       QString strAttachmentId = m_serverDbManager.getValueByKey("id");
        //       CAttachmentItem* pItem = m_localDbManager.getAttachmentItem(strAttachmentId.toInt());
       CNoteItem* pNoteItem = pAttachmentItem->getParentNoteItem();
       m_localDbManager.deleteAttachmentItem(pAttachmentItem->getLocalId());
       if(pNoteItem->isWholeNormalState())
           doProcessAction();
    }
    else if(URL_USER_PULSE == m_serverDbManager.getActionValue())
    {
        m_bGetingUserPulse = false;
        if(!isLogin())
            return;

        // is synchronizing
        if(m_nWorkingType == WORKING_TYPE_SYNC_NOTES)
            return;

        if(m_localDbManager.getDataUpdatedAtString() != m_serverDbManager.getDataUpdatedAtString())
      //          || m_localDbManager.hasModifiedData())
        {
            m_nWorkingType = WORKING_TYPE_SYNC_NOTES;
            // synchronize
 //           m_pSyncThread->setDataUpdatedAt(m_strDataUpdatedAt);
//            m_pSyncThread->start();

            m_serverDbManager.getAmountOfNotesFromServer();
        }

        checkClassifyStringServer(m_serverDbManager.getClassifyString());
    }
    else if(URL_UPDATE_CLASSIFY_STRING == strActionValue)
    {
        if(!isLogin())
            return;

        QString strClassifyServer = m_serverDbManager.getClassifyString();
        m_localDbManager.updateClassifyLabels(strClassifyServer, strClassifyServer);
    }
}

void CMainFrame::beforeSync()
{
    m_localDbManager.loadAllNoteItems();
    QList<CNoteItem*>& lstNoteItems = m_localDbManager.getNoteItemList();

    foreach(CNoteItem* pItem, lstNoteItems)
    {
        pItem->setFoundIt(false);
    }

    m_nCurrentOffsetOfNotes = 0;
    m_nCurrentPosOfNotes = 0;
}

void CMainFrame::checkDataUpdateAtString()
{
    if(m_localDbManager.getDataUpdatedAtString() == m_strDataUpdatedAt)
        return;

    bool bAllNormalState = true;
    QList<CNoteItem*>& lstNoteItemsFromLocal = m_localDbManager.getNoteItemList();
    foreach(CNoteItem* pNoteItem, lstNoteItemsFromLocal)
    {
        if(!pNoteItem->isWholeNormalState())
        {
            bAllNormalState = false;
            break;
        }
    }

    if(bAllNormalState)
        m_localDbManager.setDataUpdateAtString(m_strDataUpdatedAt);
}

void CMainFrame::updateLocalDataUpdateAtString()
{
    m_strDataUpdatedAt = QDateTime::currentDateTime().toString(DATE_TIME_FORMAT);
    m_localDbManager.setDataUpdateAtString(m_strDataUpdatedAt);
}

void CMainFrame::afterSync()
{
    QList<CNoteItem*> lstNoteItemsUnfoundIt;
    QList<CNoteItem*>& lstNoteItemsLocal = m_localDbManager.getNoteItemList();
    foreach(CNoteItem* pItem, lstNoteItemsLocal)
    {
        if(isInProcessingNoteList(pItem))
            continue;
        if(!pItem->isFoundIt())
        {
            /*
            switch(pItem->getState())
            {
            case STATE_NEW:
                m_serverDbManager.insertNoteItem(pItem);
                removeProcessingNoteItem(pItem);
                break;
            case STATE_DELETE:
            case STATE_MODIFY:
            case STATE_NORMAL:
            default:
                lstNoteItemsUnfoundIt.push_back(pItem);
                break;
            }
            */
            lstNoteItemsUnfoundIt.push_back(pItem);
        }
    }

    QDate dtRefresh;
    foreach(CNoteItem* pItem, lstNoteItemsUnfoundIt)
    {
        if(m_pDlgEditNote && m_pDlgEditNote->isVisible() &&
                m_pDlgEditNote->getNoteItemOrg() == pItem)
            m_pDlgEditNote->convertToNewNote();
        dtRefresh = pItem->getSetAt().date();
        deleteNoteItemM(pItem);
 //     lstNoteItemsLocal.removeOne(pItem);
        refreshDayWidget(dtRefresh);

        m_nCurrentOffsetOfNotes++;
        setProgressBarPos(m_nCurrentOffsetOfNotes);
    }

    if(m_bAllItemsNormalStatus)
        checkDataUpdateAtString();
}

void CMainFrame::onSync()
{
    if(m_nWorkingType == WORKING_TYPE_IDLE)
    {
        m_pDlgProgress->clearContent();
        checkAllLocalNoteStatus();
        addProcessAction(WORKING_TYPE_SYNC_NOTES);
        doProcessAction();
    }
    else
    {
        stopSync();
    }
}

void CMainFrame::stopSync()
{
    m_nWorkingType = WORKING_TYPE_IDLE;
    showProgressBar(false);
    m_pDlgProgress->clearContent();
    clearProcessAction();
}

void CMainFrame::syncLocalAndServerDb()
{
    // get amount of notes
    if(!m_serverDbManager.getAmountOfNotes(m_nAmountOfNotes, m_nAmountOfItems))
        return;

    // no note on server
    if(m_nAmountOfNotes <= 0)
    {
        m_nWorkingType = WORKING_TYPE_IDLE;
        doProcessAction();
        return;
    }

    setProgressBarRange(0, m_localDbManager.GetAmountOfItems() + m_nAmountOfItems);
    setProgressBarPos(0);
    showProgressBar(true);
    m_pStatusBar->showMessage(tr("Synchronizing with server..."));

    beforeSync();

    syncLocalAndServerDbOffset();
}

void CMainFrame::syncLocalAndServerDbOffset()
{
    // get note from server
    if(!m_serverDbManager.isGotNoteList())
    {
        m_serverDbManager.getNoteItemListByOffset(m_nCurrentOffsetOfNotes, m_nNumberPerTimes);
        return;
    }

//    setProgressBarPos(m_nCurrentOffsetOfNotes);

    QList<CNoteItem*>& lstNoteItemsFromServer = m_serverDbManager.getNoteItemsFromServer();

    CNoteItem* pLocalNoteItem = NULL;
    QList<CNoteItem*> addedNoteItems;
    QList<CNoteItem*>& lstNoteItemsFromLocal = m_localDbManager.getNoteItemList();

    // server -> local
    foreach(CNoteItem* pItemServer, lstNoteItemsFromServer)
    {
        // local item
        pLocalNoteItem = G_getNoteItemByServerId(pItemServer->getId(), lstNoteItemsFromLocal);
        if(pLocalNoteItem)
        {
            pLocalNoteItem->setFoundIt(true);
            // skip local items whose status aren't normal
            if(!pLocalNoteItem->isWholeNormalState() ||
                    !pLocalNoteItem->isWholeNormalProcessState()) {
                m_nCurrentPosOfNotes += 2; // local note + server note
                m_nCurrentPosOfNotes += pItemServer->getAmountOfAttachments();
                m_nCurrentPosOfNotes += pLocalNoteItem->getAmountOfAttachments();
                setProgressBarPos(m_nCurrentPosOfNotes);
                m_bAllItemsNormalStatus = false;
                continue;
            }

            if(!pItemServer->equalTo(pLocalNoteItem))
            {
                pItemServer->setLocalId(pLocalNoteItem->getLocalId());
                m_bAllItemsNormalStatus = false;
                // content is different
                if(!pItemServer->isSameNote(pLocalNoteItem))
                {
                    // update local
                    if(pLocalNoteItem->getMd5() != pItemServer->getMd5())
                    {
                        pLocalNoteItem->setState(STATE_NEED_GET_CONTENT);
                        m_serverDbManager.getNoteItem(pItemServer->getId());
                    }

                    QDateTime dtOrg = pLocalNoteItem->getSetAt();

                    updateNoteItemM(pItemServer, false, true);
                    if(dtOrg.date().daysTo(pItemServer->getSetAt().date()) != 0)
                        refreshDayWidget(dtOrg.date());
                    refreshDayWidget(pItemServer->getSetAt().date());
                }
                // contents are same
                else
                {
 //                   m_nCurrentPosOfNotes += 2; // local note + server note
                }

                m_nCurrentPosOfNotes += 2; // local note + server note

                // attachments is different
                if(!pItemServer->isSameAttachments(pLocalNoteItem))
                {
                    QList<CAttachmentItem*> lstAttachmentsServer = pItemServer->getAttachmentList();
                    QList<CAttachmentItem*> lstAttachmentsLocal = pLocalNoteItem->getAttachmentList();
                    CAttachmentItem* pItemLocal2 = NULL;
                    // server -> local
                    foreach(CAttachmentItem* pItemServer2, lstAttachmentsServer)
                    {
                        m_nCurrentPosOfNotes++; // server attachments
                        pItemLocal2 = pLocalNoteItem->getAttachmentItem(pItemServer2->getId());
                        // server item in local
                        if(pItemLocal2)
                        {
                            if(!pItemLocal2->equalTo(pItemServer2))
                            {
                                pItemServer2->setLocalId(pItemLocal2->getLocalId());
                                pItemServer2->copyTo(pItemLocal2);
                                if(pItemServer2->getContentSize() > 0)
                                {
                                    pItemLocal2->setState(STATE_NEED_GET_CONTENT);
                                }
                                else
                                {
                                    pItemLocal2->setState(STATE_NORMAL);
                                }
                                m_localDbManager.updateData(pItemLocal2);
                                if(pItemServer2->getContentSize() > 0)
                                    m_serverDbManager.getAttachmentItem2(pItemLocal2);
                            }
                        }
                        // new attachment
                        else
                        {
                            CAttachmentItem* pAttachmentNew = new CAttachmentItem();
                            pItemServer2->copyTo(pAttachmentNew);
                            if(pAttachmentNew->getContentSize() > 0)
                            {
                                pAttachmentNew->setState(STATE_NEED_GET_CONTENT);
                            }
                            else
                            {
                                pAttachmentNew->setState(STATE_NORMAL);

                            }

                            pLocalNoteItem->addAttachmentItem(pAttachmentNew);
                            m_localDbManager.addData(pAttachmentNew);
                            if(pAttachmentNew->getContentSize() > 0)
                                m_serverDbManager.getAttachmentItem2(pItemServer2);
                        }
                    }

                    // local -> server
                    CAttachmentItem* pItemServer2 = NULL;
                    foreach(CAttachmentItem* pItemLocal2, lstAttachmentsLocal)
                    {
                        pItemServer2 = pItemServer->getAttachmentItem(pItemLocal2->getId());
                        // local item not in server
                        if(!pItemServer2)
                        {
                            pItemLocal2->getParentNoteItem()->removeAttachmentItem(pItemLocal2);
                            delete pItemLocal2, pItemLocal2 = NULL;
                        }
                        m_nCurrentPosOfNotes++; // local attachments
                    }
                }
                // attachments are same
                else
                {
                    m_nCurrentPosOfNotes += pItemServer->getAmountOfAttachments();
                    m_nCurrentPosOfNotes += pLocalNoteItem->getAmountOfAttachments();
                }
            }
            else
            {
                m_nCurrentPosOfNotes += 2; // local note + server note
                m_nCurrentPosOfNotes += pItemServer->getAmountOfAttachments();
                m_nCurrentPosOfNotes += pLocalNoteItem->getAmountOfAttachments();
            }
        }
        // server item cannot be found in local
        else
        {
            // get attachments from server
            QList<CAttachmentItem*> lstAttachments = pItemServer->getAttachmentList();
            foreach(CAttachmentItem* pItem, lstAttachments)
            {
                if(pItem->getContentSize() > 0)
                    pItem->setState(STATE_NEED_GET_CONTENT);
                else
                {
                    pItem->setState(STATE_NORMAL);
                }

                m_nCurrentPosOfNotes++; // server attachments
            }

            // move note to local
            m_nCurrentPosOfNotes++; // server note
            pItemServer->setFoundIt(true);
            if(pItemServer->getContentSize() > 0)
                pItemServer->setState(STATE_NEED_GET_CONTENT);
            else
            {
                pItemServer->setState(STATE_NORMAL);
            }
            insertNoteItemM(pItemServer);
            addedNoteItems.push_back(pItemServer);
            if(pItemServer->getContentSize() > 0)
                m_serverDbManager.getNoteItem(pItemServer->getId());
            else
                refreshDayWidget(pItemServer->getSetAt().date());

            // get attachments from server
 //           QList<CAttachmentItem*> lstAttachments = pItemServer->getAttachmentList();
            foreach(CAttachmentItem* pItem, lstAttachments)
            {
                if(pItem->getContentSize() > 0)
                    m_serverDbManager.getAttachmentItem2(pItem);
            }
        }

        setProgressBarPos(m_nCurrentPosOfNotes);
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
    /*
    foreach(CNoteItem* pItemLocal, lstNoteItemsFromLocal)
    {
        if(pItemLocal->isFoundIt())
            continue;

        CNoteItem* pItem2 = G_getNoteItemByServerId(pItemLocal->getId(), lstNoteItemsFromServer);
        // in server
        if(pItem2)
        {
            // found it in server
            pItemLocal->setFoundIt(true);
            m_nCurrentPosOfNotes++;
        }
    }*/

    setProgressBarPos(m_nCurrentPosOfNotes);

//    setProgressBarPos(m_nCurrentOffsetOfNotes + nSizeFromServer);
    m_serverDbManager.setGotNoteList(false);

    // no next group
    if(nSizeFromServer < m_nNumberPerTimes)
    {
        afterSync();

        m_pStatusBar->clearMessage();
        m_nWorkingType = WORKING_TYPE_IDLE;
        doProcessAction();
        showProgressBar(false);
        m_pDlgFind->onFind();
    }
    // get next group of note items
    else
    {
        m_nCurrentOffsetOfNotes += nSizeFromServer;
        syncLocalAndServerDbOffset();
    }
}

void CMainFrame::addProcessingNoteItem(CNoteItem* pNoteItem)
{
    if(!pNoteItem->isNormalProcessState())
        return;

    m_lstProcessingNoteItems.push_back(pNoteItem);
    pNoteItem->setProcessState(STATE_PS_IN_LIST);
    addProcessAction(WORKING_TYPE_PROCESSING_NOTE_ITEMS);

    doProcessAction();

    addOutputString("addProcessingNoteItem");
}

void CMainFrame::processingNoteItems()
{
//    addOutputString("processingNoteItems 1 ");

    if(WORKING_TYPE_IDLE != m_nWorkingType &&
            WORKING_TYPE_PROCESSING_NOTE_ITEMS != m_nWorkingType)
    {
        return;
    }

 //   addOutputString("processingNoteItems 2");

//    if(WORKING_TYPE_PROCESSING_NOTE_ITEMS != m_nWorkingType)
        m_nWorkingType = WORKING_TYPE_PROCESSING_NOTE_ITEMS;

    // update note item
    while(m_lstProcessingNoteItems.size())
    {
        m_pProcessingNoteItem = m_lstProcessingNoteItems.first();
        m_lstProcessingNoteItems.pop_front();
        m_pProcessingNoteItem->setProcessState(STATE_PS_NORMAL);

        if(m_pProcessingNoteItem->getState() != STATE_NEW &&
                m_pProcessingNoteItem->getState() != STATE_DELETE)
        {
            QList<CAttachmentItem*> lstAttachmentItems = m_pProcessingNoteItem->getAttachmentList();
            foreach(CAttachmentItem* pItem, lstAttachmentItems)
            {
                switch(pItem->getState())
                {
                case STATE_NEW:
                    m_localDbManager.loadAttachmentContent(pItem);
                    m_serverDbManager.insertAttachmentItem2(pItem);
                    pItem->clearContent();
                    break;
                case STATE_DELETE:
                    m_serverDbManager.deleteAttachmentItem(pItem);
                    break;
                case STATE_NEED_GET_CONTENT:
                    m_serverDbManager.getAttachmentItem2(pItem);
                    break;
                default:
                    break;
                }
            }
        }

        switch(m_pProcessingNoteItem->getState())
        {
        case STATE_NEW:
            m_serverDbManager.insertNoteItem(m_pProcessingNoteItem);
            break;
        case STATE_MODIFY:
            m_serverDbManager.updateNoteItem(m_pProcessingNoteItem);
            break;
        case STATE_DELETE:
            m_serverDbManager.deleteNoteItem(m_pProcessingNoteItem);
            break;
        case STATE_NEED_GET_CONTENT:
            m_serverDbManager.getNoteItem(m_pProcessingNoteItem->getId());
            break;
        default:
            break;
        }
    }

    m_nWorkingType = WORKING_TYPE_IDLE;
    doProcessAction();
}

void CMainFrame::removeProcessingNoteItem(CNoteItem* pNoteItem)
{
    m_lstProcessingNoteItems.removeOne(pNoteItem);
}

bool CMainFrame::isInProcessingNoteList(CNoteItem* pNoteItem)
{
    return m_lstProcessingNoteItems.indexOf(pNoteItem) >= 0;
}


void CMainFrame::clearProcessAction()
{
    m_lstActions.clear();
}

void CMainFrame::addProcessAction(ActionType nAction)
{
    m_lstActions.push_back(nAction);
}

void CMainFrame::doProcessAction()
{
    if(m_nWorkingType != WORKING_TYPE_IDLE)
        return;

    addOutputString("doProcessAction");

    if(m_lstActions.size())
    {
        m_nWorkingType = m_lstActions.first();
        m_lstActions.pop_front();

        switch(m_nWorkingType)
        {
        case WORKING_TYPE_LOGIN:
            break;
        case WORKING_TYPE_REGISTER:
            break;
        case WORKING_TYPE_SYNC_NOTES:
            syncLocalAndServerDb();
            break;
        case WORKING_TYPE_PROCESSING_NOTE_ITEMS:
            processingNoteItems();
            break;
        default:
            break;
        }
    }
    else
    {
        m_nWorkingType = WORKING_TYPE_IDLE;
    }

    updateControlState();
}

void CMainFrame::addNetworkReply(QNetworkReply* pNetworkReply, const QString& strDesc)
{
    if(!m_pDlgProgress)
        return;

    NETWORK_ITEM* pNetworkItem = new NETWORK_ITEM;
    pNetworkItem->m_pNetworkReply = pNetworkReply;
    m_pDlgProgress->addNetworkItem(pNetworkItem, strDesc);

    addOutputString("Add network reply: (" + strDesc + ")");
}

void CMainFrame::addFileDownload(CFileDownload* pFileDownload, const QString& strDesc)
{
    if(m_pDlgProgress)
    {
        NETWORK_ITEM* pNetworkItem = new NETWORK_ITEM;
        pNetworkItem->m_pFileDownload = pFileDownload;
        m_pDlgProgress->addNetworkItem(pNetworkItem, strDesc);

        addOutputString("File Download: add (" + strDesc + ")" + QString::number(pFileDownload->getAttachmentItem()->getId()));
    }
}

bool CMainFrame::hasNetReplyWorking()
{
    if(WORKING_TYPE_IDLE == m_nWorkingType)
        return false;

    if(m_pDlgProgress)
        return m_pDlgProgress->hasNetworkItemWorking();

    return false;
}

/*
void CMainFrame::onNetworkReplyError(QNetworkReply::NetworkError nNetworkError)
{
    switch(nNetworkError)
    {
    case QNetworkReply::TimeoutError:
        addOutputString(tr("Error: ") + tr("Network timeout."));
        break;
    default:
        break;
    }
}
*/

// void CMainFrame::onServerDbManagerFinished()
// {

// }


void CMainFrame::onFinishedDownloadAttachmentItem(CAttachmentItem* pAttachmentItem, QString str)
{
    if(!pAttachmentItem)
        return;

    if(str.isEmpty())
    {
        pAttachmentItem->setState(STATE_NORMAL);
        m_localDbManager.updateData(pAttachmentItem, false);
    }

    addOutputString("File Download: finished " + QString::number(pAttachmentItem->getId()));
    addOutputString("File Download: finished " + str);

 //   if(m_pDlgProgress) {
 //       qlonglong nItemPointer = (qlonglong)pAttachmentItem;
     //   QObject* pObj = sender();
  //      m_pDlgProgress->removeItemPointer(nItemPointer);
 //   }
}

/*
void CMainFrame::onErrorFinishedDownloadAttachmentItem(CAttachmentItem* pAttachmentItem, QString strErr)
{
    if(!pAttachmentItem)
        return;

    addOutputString("File Download: error finished " + QString::number(pAttachmentItem->getId()));

    if(m_pDlgProgress) {
        m_pDlgProgress->addMemo((qlonglong)pAttachmentItem, strErr);
    }
}

void CMainFrame::onFinishedDownloadAttachment(const QString& strErr)
{
    if(strErr.length() > 0)
    {
        addOutputString("File Download: error " + strErr);
        m_pStatusBar->showMessage(strErr);
    }
}
*/
