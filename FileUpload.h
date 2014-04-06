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

#ifndef FILEUPLOAD_H
#define FILEUPLOAD_H

#include <QIODevice>
#include <QFile>

class CFileUpload : public QIODevice
{
public:
    CFileUpload(const QString& strPathFileName, const QByteArray& arrHeader, const QByteArray& arrTail, QObject* pParent = 0);
    CFileUpload(const QByteArray& arrByteArray, const QByteArray& arrHeader, const QByteArray& arrTail, QObject* pParent = 0);
    virtual ~CFileUpload();

    bool openFile();

    virtual qint64 size() const;
    qint64 contentSize() const;

protected:
    virtual qint64 pos() const;
    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);

    QFile* m_pFile;
    QByteArray* m_pHeader;
    QByteArray* m_pTail;
    qint64 m_nPosition;
    QByteArray m_arrContent;
};

#endif // FILEUPLOAD_H
