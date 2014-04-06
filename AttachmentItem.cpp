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

#include "NoteItem.h"
#include "AttachmentItem.h"
#include <QFileInfo>
#include <QMessageBox>
#include "FileDownload.h"

CAttachmentItem::CAttachmentItem(QObject* pParent)
    : CEntityItem(pParent)
{
    m_bLoadFromFile = false;
    m_pFileDownload = NULL;
}

CAttachmentItem::~CAttachmentItem(void)
{
    if(m_pFileDownload)
        m_pFileDownload->enforceCancel(false);
}

CNoteItem* CAttachmentItem::getParentNoteItem() const
{
    return qobject_cast<CNoteItem*>(parent());
}

QString CAttachmentItem::getFileName() const
{
    return m_strFileName;
}

void CAttachmentItem::setFileName(const QString& str)
{
    m_strFileName = str;

    QFileInfo fileInfo(m_strFileName);
    m_strFileNameDisplay = fileInfo.fileName();
}

bool CAttachmentItem::loadContent(QWidget* pWidget)
{
    QFileInfo fileInf(m_strFileName);
    QString str = fileInf.path();
    if(str == "." || str.isEmpty())
        return true;

    QFile file(m_strFileName);
    if(!file.open(QFile::ReadOnly))
    {
        QMessageBox::critical(pWidget, tr("Fail to open file"), tr("Fail to open file \"") + m_strFileName + "\".");
        return false;
    }

    setContent(file.readAll());
    setFileName(getFileNameDisplay());
    m_bLoadFromFile = false;
    return true;
}

CAttachmentItem& CAttachmentItem::operator = (const CAttachmentItem& item)
{
    item.copyTo(this);
    return *(this);
}

bool CAttachmentItem::operator == (const CAttachmentItem& item) const
{
    return equalTo(&item);
}

bool CAttachmentItem::operator != (const CAttachmentItem& item) const
{
    return !equalTo(&item);
}

bool CAttachmentItem::equalTo(const CAttachmentItem* pOther) const
{
    if(!CEntityItem::equalTo(pOther))
        return false;

    return getFileName() == pOther->getFileName();
}

void CAttachmentItem::copyTo(CAttachmentItem* pOther) const
{
    CEntityItem::copyTo(pOther);

    pOther->setFileName(getFileName());
}

void CAttachmentItem::setFileDownload(CFileDownload* pFileDownload)
{
    m_pFileDownload = pFileDownload;
}


