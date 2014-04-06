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
#include "AttachmentItem.h"
#include "NoteItem.h"

CAttachmentItem* G_getAttachmentItemByServerId(int nId, const QList<CAttachmentItem*>& lstAttachmentItems)
{
    foreach(CAttachmentItem* pItem, lstAttachmentItems)
    {
        if(pItem->getId() == nId)
            return pItem;
    }

    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////
CNoteItem::CNoteItem(QObject* pParent)
    : CEntityItem(pParent)
    , m_nNoteType(NOTE_TYPE_NOTE)
{
    m_pRelatedGraphicsItem = NULL;
    m_bSelected = false;
    m_bFoundIt = false;
    m_nShareType = SHARE_TYPE_PRIVATE;
}

CNoteItem::~CNoteItem(void)
{
    foreach(CAttachmentItem* pItem, m_lstAttachmentItems)
    {
        delete pItem;
    }

    m_lstAttachmentItems.clear();
}

int CNoteItem::getType() const
{
    return m_nNoteType;
}

void CNoteItem::setType(int nType)
{
    m_nNoteType = nType;

    emit typeChanged();
}

bool CNoteItem::isNote() const
{
    return m_nNoteType == NOTE_TYPE_NOTE;
}

bool CNoteItem::isSchedule() const
{
    return m_nNoteType == NOTE_TYPE_SCHEDULE;
}

const QString& CNoteItem::getTitle() const
{
    return m_strTitle;
}

void CNoteItem::setTitle(const QString& strTitle)
{
    m_strTitle = strTitle;
    emit titleChanged();
}

const QDateTime& CNoteItem::getSetAt() const
{
    return m_dtSetAt;
}

void CNoteItem::setSetAt(const QDateTime& dt)
{
    m_dtSetAtBk = m_dtSetAt;
    m_dtSetAt = dt;
    emit setAtChanged();
}

bool CNoteItem::hasAttachment() const
{
    return m_lstAttachmentItems.length() > 0;
}

void CNoteItem::addAttachmentItem(CAttachmentItem* pItem)
{
    pItem->setParent(this);
    m_lstAttachmentItems.push_back(pItem);
}

void CNoteItem::removeAttachmentItem(CAttachmentItem* pItem)
{
    pItem->setParent(NULL);
    m_lstAttachmentItems.removeOne(pItem);
}

void CNoteItem::clearAllAttachmentItems()
{
    foreach(CAttachmentItem* pItem, m_lstAttachmentItems)
    {
        pItem->setParent(NULL);
    }
    m_lstAttachmentItems.clear();
}

bool CNoteItem::loadAttachmentContents(QWidget* pWidget)
{
    foreach(CAttachmentItem* pItem, m_lstAttachmentItems)
    {
        if(!pItem->loadContent(pWidget))
            return false;
    }

    return true;
}

void CNoteItem::clearAttachmentContents()
{
    foreach(CAttachmentItem* pItem, m_lstAttachmentItems)
    {
        pItem->clearContent();
    }
}

const QList<CAttachmentItem*>& CNoteItem::getAttachmentList() const
{
    return m_lstAttachmentItems;
}

void CNoteItem::setAttachmentList(const QList<CAttachmentItem*>& lstAttachmentItems)
{
    foreach(CAttachmentItem* pItem, m_lstAttachmentItems)
    {
        delete pItem;
    }
    m_lstAttachmentItems.clear();

    CAttachmentItem* pItem2 = NULL;
    foreach(CAttachmentItem* pItem, lstAttachmentItems)
    {
        pItem2 = new CAttachmentItem(this);
        m_lstAttachmentItems.push_back(pItem2);
        pItem->copyTo(pItem2);
    }

    emit attachmentListChanged();
}

CAttachmentItem* CNoteItem::getAttachmentItemByFileName(const QString& strFileName)
{
    foreach(CAttachmentItem* pItem, m_lstAttachmentItems)
    {
        if(pItem->getFileNameDisplay() == strFileName)
            return pItem;
    }

    return NULL;
}

CNoteItem& CNoteItem::operator = (const CNoteItem& noteItem)
{
    const CNoteItem* pSource = &noteItem;
    pSource->copyTo(this, false, false);

    return *(this);
}

bool CNoteItem::operator ==(const CNoteItem& other) const
{
    return equalTo(&other);
}

bool CNoteItem::operator !=(const CNoteItem& other) const
{
    return !equalTo(&other);
}

bool CNoteItem::isSameNote(const CNoteItem *pOther) const
{
    if(!pOther)
        return false;

    if(!CEntityItem::equalTo(pOther))
        return false;

    if(getShareType() != pOther->getShareType() ||
            getType() != pOther->getType() ||
            getTitle() != pOther->getTitle() ||
            getSetAt() != pOther->getSetAt())
        return false;

    return true;
}

bool CNoteItem::isSameAttachments(const CNoteItem *pOther) const
{
    if(!pOther || getAttachmentList().size() != pOther->getAttachmentList().size())
        return false;

    CAttachmentItem* pItem2 = NULL;
    foreach(CAttachmentItem* pItem, getAttachmentList())
    {
        pItem2 = G_getAttachmentItemByServerId(pItem->getId(), pOther->getAttachmentList());
        if(pItem2)
        {
            if(!pItem2->equalTo(pItem))
                return false;
        }
        else
            return false;
    }

    return true;
}

bool CNoteItem::equalTo(const CNoteItem *pOther) const
{
    if(!isSameNote(pOther) || !isSameAttachments(pOther))
        return false;

    return true;
}

bool CNoteItem::copyTo(CNoteItem *pOther, bool bWithoutContent, bool bWithoutAttach) const
{
    if(equalTo(pOther))
        return false;

    CEntityItem::copyTo(pOther, bWithoutContent);

    pOther->setShareType(getShareType());
    pOther->setType(getType());
    pOther->setTitle(getTitle());
    pOther->setSetAt(getSetAt());
    if(!bWithoutAttach)
        pOther->setAttachmentList(getAttachmentList());

    pOther->emitDataChanged();
    return true;
}

bool CNoteItem::isNewerThan(const CNoteItem* pOther) const
{
    return getUpdatedAt() > pOther->getUpdatedAt();
}

CAttachmentItem* CNoteItem::getAttachmentItem(int nId) const
{
    return G_getAttachmentItemByServerId(nId, m_lstAttachmentItems);
}

CAttachmentItem* CNoteItem::getAttachmentItemByLocalId(int nLocalId) const
{
    foreach(CAttachmentItem* pItem, m_lstAttachmentItems)
    {
        if(pItem->getLocalId() == nLocalId)
            return pItem;
    }

    return NULL;
}

int CNoteItem::getAmountOfAttachments() const
{
    return m_lstAttachmentItems.count();
}

QGraphicsItem* CNoteItem::getGraphicsItem() const
{
    return m_pRelatedGraphicsItem;
}

void CNoteItem::setGraphicsItem(QGraphicsItem* pGraphicsItem)
{
    m_pRelatedGraphicsItem = pGraphicsItem;
}

void CNoteItem::setSelected(bool bSelected)
{
    if(m_bSelected != bSelected)
    {
        m_bSelected = bSelected;
        emit selectedChanged();
    }
}

bool CNoteItem::isNormalState() const
{
    return CEntityItem::isNormalState();
}

bool CNoteItem::isWholeNormalState() const
{
    if(!isNormalState())
        return false;

    foreach(CAttachmentItem* pItem, m_lstAttachmentItems)
    {
        if(!pItem->isNormalState())
            return false;
    }

    return true;
}

QIcon CNoteItem::getIcon() const
{
    return QIcon(QPixmap::fromImage(getImg()));
}

QImage CNoteItem::getImg() const
{
    QImage img;
    switch(getType())
    {
    case NOTE_TYPE_NOTE:
        if(hasAttachment())
        {
            if(isWholeNormalState())
            {
                img.load(QString::fromUtf8(":/res/itemnote_att.png"));
            }
            else
            {
                img.load(QString::fromUtf8(":/res/itemnote_att_local.png"));
            }
        }
        else
        {
            if(isWholeNormalState())
            {
                img.load(QString::fromUtf8(":/res/itemnote.png"));
            }
            else
            {
                img.load(QString::fromUtf8(":/res/itemnote_local.png"));
            }
        }

        break;
    case NOTE_TYPE_SCHEDULE:
        if(hasAttachment())
        {
            if(isWholeNormalState())
            {
                img.load(QString::fromUtf8(":/res/itemschedule_att.png"));
            }
            else
            {
                img.load(QString::fromUtf8(":/res/itemschedule_att_local.png"));
            }
        }
        else
        {
            if(isWholeNormalState())
            {
                img.load(QString::fromUtf8(":/res/itemschedule.png"));
            }
            else
            {
                img.load(QString::fromUtf8(":/res/itemschedule_local.png"));
            }
        }
        break;
    default:
        break;
    }

    return img;
}

bool CNoteItem::isNormalProcessState() const
{
    return CEntityItem::isNormalProcessState();
}

bool CNoteItem::isWholeNormalProcessState() const
{
    if(!isNormalProcessState())
        return false;

    foreach(CAttachmentItem* pItem, m_lstAttachmentItems)
    {
        if(!pItem->isNormalProcessState())
            return false;
    }

    return true;
}

void CNoteItem::emitDataChanged()
{
    emit dataChanged();
}

