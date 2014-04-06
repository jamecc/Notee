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

#ifndef DCTABLEWIDGET_H
#define DCTABLEWIDGET_H

#include <QTableWidget>
#include "NoteItem.h"

// decoration center widget
class CDCTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit CDCTableWidget(QWidget *parent = 0);

    void setNoteItemList(const QList<CNoteItem*>& lstNoteItems);
    void clearNoteItems();
    void clearContent();

    CNoteItem* getSelectedNoteItem();
    int getRowIndex(CNoteItem* pNoteItem);
    void selectNoteItem(CNoteItem* pNoteItem);
    void unselectNoteItem(CNoteItem* pNoteItem);
    void removeNoteItem(CNoteItem* pNoteItem);
    
signals:
    
public slots:
    void onContextMenu(const QPoint& pt);
    void onNoteItemDataChanged();
    void onNoteItemStateChanged();
    void onNoteItemSelectedChanged();
    void onItemSelectionChanged();
    void onItemDoubleClicked(QTableWidgetItem *item);
    void onNewNoteItem();
    void onEditNoteItem();
    void onDeleteNoteItem();

protected:
    virtual QStyleOptionViewItem viewOptions() const;
    virtual void mouseDoubleClickEvent( QMouseEvent *e );

private:
    QList<CNoteItem*> m_lstNoteItems;
};

#endif // DCTABLEWIDGET_H
