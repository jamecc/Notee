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

#ifndef IMAGETEXTEDIT_H
#define IMAGETEXTEDIT_H

#include <QTextEdit>

class QHttp;
class QBuffer;
struct HTTP_IMAGE
{
    int nRequestId;
    QString strImgLink;
    QHttp* pHttp;
    QBuffer* pBuffer;
    QByteArray arrBytes;
};

class CImageTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit CImageTextEdit(QWidget *parent = 0);
    virtual ~CImageTextEdit();

    void clearHttpImages();

    void requestHttp(const QUrl& url, const QString& strImgLink);
    
signals:
    
public slots:
    void onHttpFinished(int, bool);

protected:
    virtual bool canInsertFromMimeData(const QMimeData* source) const;
    virtual void insertFromMimeData(const QMimeData* source);

private:
    void dropImage(const QImage& image);
    void dropTextFile(const QUrl& url);

    QList<HTTP_IMAGE*> m_lstHttpImages;
};

#endif // IMAGETEXTEDIT_H
