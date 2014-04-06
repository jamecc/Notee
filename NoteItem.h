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

#ifndef NOTEITEM_H
#define NOTEITEM_H

#include <QList>
#include <QIcon>
#include <QImage>
#include "EntityItem.h"
#include "AttachmentItem.h"

enum {
    NOTE_TYPE_UNKNOWN = 0,
    NOTE_TYPE_NOTE,
    NOTE_TYPE_SCHEDULE
};

enum {
    SHARE_TYPE_PRIVATE = 0,
    SHARE_TYPE_PUBLIC
};

class CAttachmentItem;
class QGraphicsItem;
class CNoteItem : public CEntityItem
{
    Q_OBJECT
public:
    CNoteItem(QObject* pParent = NULL);
    virtual ~CNoteItem(void);

    int getType() const;
    void setType(int nType);

    int getShareType() const   {return m_nShareType;}
    void setShareType(int nType)   {m_nShareType = nType;}

    bool isNote() const;
    bool isSchedule() const;

    const QString& getTitle() const;
    void setTitle(const QString& strTitle);

    const QDateTime& getSetAt() const;
    void setSetAt(const QDateTime& dt);
    const QDateTime& getSetAtBk() const {return m_dtSetAtBk;}

    bool hasAttachment() const;
    void addAttachmentItem(CAttachmentItem* pItem);
    void removeAttachmentItem(CAttachmentItem* pItem);
    void clearAllAttachmentItems();
    bool loadAttachmentContents(QWidget* pWidget);
    void clearAttachmentContents();

    const QList<CAttachmentItem*>& getAttachmentList() const;
    void setAttachmentList(const QList<CAttachmentItem*>& lstAttachmentItems);
    CAttachmentItem* getAttachmentItemByFileName(const QString& strFileName);

    CNoteItem& operator = (const CNoteItem& noteItem);
    bool operator == (const CNoteItem& noteItem) const;
    bool operator != (const CNoteItem& noteItem) const;
    bool isSameNote(const CNoteItem *pOther) const;
    bool isSameAttachments(const CNoteItem *pOther) const;

    bool equalTo(const CNoteItem* pOther) const;
    bool copyTo(CNoteItem* pOther, bool bWithoutContent = true, bool bWithoutAttach = true) const;

    bool isNewerThan(const CNoteItem* pOther) const;
    CAttachmentItem* getAttachmentItem(int nId) const;
    CAttachmentItem* getAttachmentItemByLocalId(int nLocalId) const;
    int getAmountOfAttachments() const;

    QGraphicsItem* getGraphicsItem() const;
    void setGraphicsItem(QGraphicsItem* pGraphicsItem);

    bool isSelected() const             {return m_bSelected;}
    void setSelected(bool bSelected);
    bool isFoundIt() const              {return m_bFoundIt;}
    void setFoundIt(bool bVal)          {m_bFoundIt = bVal;}
    bool isNormalState() const;
    bool isWholeNormalState() const;
    bool isNormalProcessState() const;
    bool isWholeNormalProcessState() const;
    QIcon getIcon() const;
    QImage getImg() const;

    void emitDataChanged();

Q_SIGNALS:
    void typeChanged();
    void titleChanged();
    void setAtChanged();
    void attachmentListChanged();
    void dataChanged();
    void selectedChanged();

protected:
    int m_nNoteType;
    int m_nShareType;
    bool m_bSelected;
    bool m_bFoundIt;
    QString m_strTitle;
    QDateTime m_dtSetAt;
    QDateTime m_dtSetAtBk;

    QList<CAttachmentItem*> m_lstAttachmentItems;
    QGraphicsItem* m_pRelatedGraphicsItem;
};

#endif // #ifndef NOTEITEM_H
