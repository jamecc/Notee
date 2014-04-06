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

#ifndef ATTACHMENTLISTWIDGET_H
#define ATTACHMENTLISTWIDGET_H

#include <QListWidget>
#include <QKeyEvent>
#include "AttachmentItem.h"

class CAttachmentListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit CAttachmentListWidget(QWidget *parent = 0);

    QListWidgetItem* getWidgetItem(CAttachmentItem* pItem);
    CAttachmentItem* getAttachmentItemByPathFileName(const QString& strPathFileName);
    CAttachmentItem* getAttachmentItemByFileNameDisplay(const QString& strFileNameDisplay);
    bool addAttachmentItem(CAttachmentItem* pItem);
    void delAttachmentItem(CAttachmentItem* pItem = NULL);
    QList<CAttachmentItem*> getSelectedAttachments();

signals:
    void dataChanged();
    void emitAdd();
    void emitDelete();
    void emitSaveAs();
    void addAttachment(QString);

public slots:
    void onContextMenu(const QPoint& pt);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void keyPressEvent( QKeyEvent * event );
};

#endif // ATTACHMENTLISTWIDGET_H
