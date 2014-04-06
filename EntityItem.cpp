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

#include <QCryptographicHash>
#include "EntityItem.h"

CEntityItem::CEntityItem(QObject *parent)
    : QObject(parent)
{
    m_id = 0;
    m_nLocalId = 0;
    m_lContentSize = 0;
    m_bLoadedContent = false;
    m_nState = STATE_NORMAL;
    m_nProcessState = STATE_PS_NORMAL;
}

int CEntityItem::getId() const
{
    return m_id;
}

void CEntityItem::setId(int id)
{
    m_id = id;
}

int CEntityItem::getLocalId() const
{
    return m_nLocalId;
}

void CEntityItem::setLocalId(int id)
{
    m_nLocalId = id;
}

const QString& CEntityItem::getMd5() const
{
    return m_strMd5;
}

void CEntityItem::setMd5(const QString& str)
{
    m_strMd5 = str;
}

const QDateTime& CEntityItem::getCreatedAt() const
{
    return m_dtCreatedAt;
}

void CEntityItem::setCreatedAt(const QDateTime& dt)
{
    m_dtCreatedAt = dt;
}

const QDateTime& CEntityItem::getUpdatedAt() const
{
    return m_dtUpdatedAt;
}

void CEntityItem::setUpdatedAt(const QDateTime& dt)
{
    m_dtUpdatedAt = dt;
}

const QByteArray& CEntityItem::getContent() const
{
    return m_arrContent;
}

void CEntityItem::setContent(const QByteArray& arr, bool bCalMd5)
{
    m_arrContent = arr;
    m_lContentSize = m_arrContent.size();
    m_bLoadedContent = true;
    if(bCalMd5 && m_lContentSize > 0)
        calContentMd5();
    else if(m_lContentSize == 0)
        m_strMd5 = "";
}

void CEntityItem::clearContent()
{
    // to save memory
    m_arrContent.clear();
    m_bLoadedContent = false;
}

void CEntityItem::calContentMd5()
{
    m_strMd5 = QString(QCryptographicHash::hash(m_arrContent,QCryptographicHash::Md5).toHex());
}

void CEntityItem::setContentSize(long lSize)
{
    m_lContentSize = lSize;
}

long CEntityItem::getContentSize() const
{
    return m_lContentSize;
}

CEntityItem& CEntityItem::operator = (const CEntityItem& item)
{
    item.copyTo(this);

    return *this;
}

bool CEntityItem::operator == (const CEntityItem& item) const
{
    return equalTo(&item);
}

bool CEntityItem::operator != (const CEntityItem& item) const
{
    return !equalTo(&item);
}

bool CEntityItem::equalTo(const CEntityItem* pOther) const
{
    return getId() == pOther->getId() &&
            getState() == pOther->getState() &&
//            getLocalId() == pOther->getLocalId() &&
            getMd5() == pOther->getMd5();
}

void CEntityItem::copyTo(CEntityItem* pOther, bool bWithoutContent) const
{
    pOther->setId(getId());
    pOther->setLocalId(getLocalId());
    pOther->setState(getState());
    pOther->setMd5(getMd5());
    pOther->setContentSize(getContentSize());
    pOther->setCreatedAt(getCreatedAt());
    pOther->setUpdatedAt(getUpdatedAt());
    if(!bWithoutContent)
        pOther->setContent(getContent());
}

int CEntityItem::getState() const
{
    return m_nState;
}

void CEntityItem::setState(int nState)
{
    if(m_nState != nState)
    {
        m_nState = nState;
        emit itemStateChanged();
    }
}

bool CEntityItem::isAllNormalState() const
{
    return isNormalState() && isNormalProcessState();
}

bool CEntityItem::isNormalState() const
{
    return STATE_NORMAL == m_nState;
}

bool CEntityItem::isNormalProcessState() const
{
    return STATE_PS_NORMAL == m_nProcessState;
}

bool CEntityItem::isShowByState() const
{
    return m_nState != STATE_DELETE;
}

int CEntityItem::getProcessState() const
{
    return m_nProcessState;
}

void CEntityItem::setProcessState(int nState)
{
    m_nProcessState = nState;
}
