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

#include "FileUpload.h"

CFileUpload::CFileUpload(const QString& strPathFileName, const QByteArray& arrHeader, const QByteArray& arrTail, QObject* pParent)
    : QIODevice(pParent)
{
    m_pFile = new QFile(strPathFileName);
    m_pHeader = new QByteArray(arrHeader);
    m_pTail = new QByteArray(arrTail);
    m_nPosition = 0;
}

CFileUpload::CFileUpload(const QByteArray& arrByteArray, const QByteArray& arrHeader, const QByteArray& arrTail, QObject* pParent)
    : QIODevice(pParent)
{
    m_pFile = NULL;
    m_arrContent = arrByteArray;
    m_pHeader = new QByteArray(arrHeader);
    m_pTail = new QByteArray(arrTail);
    m_nPosition = 0;
}

CFileUpload::~CFileUpload()
{
    if(m_pFile)
        delete m_pFile;
    delete m_pHeader;
    delete m_pTail;
}

bool CFileUpload::openFile()
{
    if(!m_pFile)
    {
        return open(QIODevice::ReadOnly);
    }

    if(m_pFile->open(QIODevice::ReadOnly))
        return open(QIODevice::ReadOnly);

    return false;
}

qint64 CFileUpload::pos() const
{
    return m_nPosition;
}

qint64 CFileUpload::size() const
{
    return m_pHeader->size() + contentSize() + m_pTail->size();
}

qint64 CFileUpload::contentSize() const
{
    if(m_pFile)
        return m_pFile->size();

    return m_arrContent.size();
}

qint64 CFileUpload::readData(char *data, qint64 maxlen)
{
    if (m_pFile && !m_pFile->isOpen())
    {
        return -1;
    }

    // header
    char *pointer = data;
    qint64 nReadAmount = 0;
    if(maxlen > 0 &&
            m_nPosition < m_pHeader->size()) {
        qint64 count = qMin(maxlen, (qint64)m_pHeader->size()-m_nPosition);
        memcpy(pointer, m_pHeader->data()+m_nPosition, count);
        pointer += count;
        m_nPosition += count;
        nReadAmount += count;
        maxlen -= count;
    }

    // content
    if(maxlen > 0 &&
            m_nPosition >=m_pHeader->size() &&
            m_nPosition < (contentSize() + m_pHeader->size()))
    {
        qint64 count = 0;
        if(m_pFile)
        {
            count = qMin(maxlen, m_pFile->bytesAvailable());
            count = m_pFile->read(pointer, count);
        }
        else
        {
            count = qMin(maxlen, contentSize()-(m_nPosition-m_pHeader->size()));
            memcpy(pointer, m_arrContent.data() + (m_nPosition-m_pHeader->size()), count);
        }
        pointer += count;
        m_nPosition += count;
        nReadAmount += count;
        maxlen -= count;
    }

    // tail
    if(maxlen > 0 &&
            m_nPosition >= (m_pHeader->size() + contentSize()) &&
            m_nPosition < size()) {
        qint64 count = qMin(maxlen, (qint64)m_pTail->size()-(m_nPosition - m_pHeader->size() - contentSize()));
        memcpy(pointer, m_pTail->data()+(m_nPosition - m_pHeader->size() - contentSize()), count);
        m_nPosition += count;
        nReadAmount += count;
    }

    return nReadAmount;
}

qint64 CFileUpload::writeData(const char *data, qint64 len)
{
    return -1;
}
