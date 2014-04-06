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

#include "TextOutput.h"
#include <QCursor>
#include <QMenu>
#include <QKeySequence>

CTextOutput::CTextOutput(QWidget *parent) :
    QTextEdit(parent)
{
    setReadOnly(true);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onCustomContextMenuRequested(QPoint)));
}

void CTextOutput::onCustomContextMenuRequested(const QPoint& )
{
    QMenu menu;

    menu.addAction(tr("Copy"), this, SLOT(copy()), QKeySequence(QKeySequence::Copy));
    menu.addAction(tr("Select all"), this, SLOT(selectAll()), QKeySequence(QKeySequence::SelectAll));
    menu.addSeparator();
    menu.addAction(tr("Clear all"), this, SLOT(clear()));
    menu.exec(QCursor::pos());
}
