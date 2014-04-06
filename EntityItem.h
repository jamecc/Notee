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

#ifndef ENTITYITEM_H
#define ENTITYITEM_H

#include <QObject>
#include <QDateTime>

// modify state
enum {
    STATE_NORMAL = 0,
    STATE_NEW,
    STATE_MODIFY,
    STATE_DELETE,
    STATE_NEED_GET_CONTENT
};

// process state
enum {
    STATE_PS_NORMAL = 0,
    STATE_PS_IN_LIST,
    STATE_PS_UPLOADING,
    STATE_PS_DOWNLOADING,
    STATE_PS_DELETING
};

class CEntityItem : public QObject
{
    Q_OBJECT
public:
    explicit CEntityItem(QObject *parent = 0);

    int getId() const;
    void setId(int id);
    int getLocalId() const;
    void setLocalId(int id);
    const QString& getMd5() const;
    void setMd5(const QString& str);

    const QDateTime& getCreatedAt() const;
    void setCreatedAt(const QDateTime& dt);
    const QDateTime& getUpdatedAt() const;
    void setUpdatedAt(const QDateTime& dt);

    const QByteArray& getContent() const;
    void setContent(const QByteArray& arr, bool bCalMd5 = true);
    void clearContent();  // to save memory
    void calContentMd5();
    void setContentSize(long lSize);
    long getContentSize() const;

    CEntityItem& operator = (const CEntityItem& item);
    bool operator == (const CEntityItem& item) const;
    bool operator != (const CEntityItem& item) const;

    bool equalTo(const CEntityItem* pOther) const;
    void copyTo(CEntityItem* pOther, bool bWithoutContent = true) const;

    int getState() const;
    void setState(int nState);
    bool isAllNormalState() const;
    bool isNormalState() const;
    bool isNormalProcessState() const;

    bool isShowByState() const;

    int getProcessState() const;
    void setProcessState(int nState);

Q_SIGNALS:
    void itemStateChanged();

protected:
    int m_id;
    int m_nLocalId;
    QString m_strMd5;

    QDateTime m_dtCreatedAt;
    QDateTime m_dtUpdatedAt;
    QByteArray m_arrContent;
    long m_lContentSize;
    bool m_bLoadedContent;

    int m_nState;
    int m_nProcessState;
};

#endif // ENTITYITEM_H
