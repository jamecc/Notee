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

#ifndef ATTACHMENTITEM_H
#define ATTACHMENTITEM_H

#include "EntityItem.h"

class CNoteItem;
class CFileDownload;
class CAttachmentItem : public CEntityItem
{
public:
    CAttachmentItem(QObject* pParent = NULL);
    virtual ~CAttachmentItem(void);

    CNoteItem* getParentNoteItem() const;
    QString getFileName() const;
    void setFileName(const QString& str);
    bool loadContent(QWidget* pWidget = NULL);
    void setLoadFromFile(bool bVal = true)  {m_bLoadFromFile = bVal;}
    bool isLoadFromFile() const             {return m_bLoadFromFile;}

    QString getFileNameDisplay() const      {return m_strFileNameDisplay;}
    void setFileNameDisplay(const QString& str) {m_strFileNameDisplay = str;}

    CAttachmentItem& operator = (const CAttachmentItem& item);
    bool operator == (const CAttachmentItem& item) const;
    bool operator != (const CAttachmentItem& item) const;

    bool equalTo(const CAttachmentItem* pOther) const;
    void copyTo(CAttachmentItem* pOther) const;

    void setFileDownload(CFileDownload* pFileDownload);

private:
    QString m_strFileName;
    QString m_strFileNameDisplay;
    bool m_bLoadFromFile;
    CFileDownload* m_pFileDownload;
};

#endif // #ifndef ATTACHMENTITEM_H
