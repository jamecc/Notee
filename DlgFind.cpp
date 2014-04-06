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

#include <QtGui>
#include "predefine.h"
#include "MainFrame.h"
#include "NoteItem.h"
#include "DCTableWidget.h"
#include "DlgFind.h"

#define STR_TAB_ROOT "--Current--"
#define IDX_TAB_ROOT    0

CDlgFind::CDlgFind(QWidget *parent) :
    QWidget(parent)
{
    m_bSender = false;

    setContentsMargins(0,0,0,0);
    m_pLineEdit = new QLineEdit(this);
    connect(m_pLineEdit, SIGNAL(returnPressed()), this, SLOT(onFind()));

    // find
    QPushButton* pBtnFind = new QPushButton(this);
    pBtnFind->setIconSize(QSize(16,16));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/res/find.png"), QSize(), QIcon::Normal, QIcon::Off);
    pBtnFind->setIcon(icon1);
//    pBtnFind->setText(tr("Find"));
    pBtnFind->setToolTip(tr("Find"));
    pBtnFind->setStatusTip(tr("Find"));
    connect(pBtnFind, SIGNAL(clicked()), this, SLOT(onFind()));

    // current
    QPushButton* pBtnCurrent = new QPushButton(this);
    pBtnCurrent->setIconSize(QSize(16,16));
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/res/current.png"), QSize(), QIcon::Normal, QIcon::Off);
    pBtnCurrent->setIcon(icon2);
//    pBtnCurrent->setText(tr("Notes on Current Month"));
    pBtnCurrent->setToolTip(tr("All notes on the current month"));
    pBtnCurrent->setStatusTip(tr("All notes on the current month"));
    connect(pBtnCurrent, SIGNAL(clicked()), this, SLOT(onCurrent()));

    // table
    m_pTableWidget = new CDCTableWidget(this);

    // layout
    QVBoxLayout* pMainLayout = new QVBoxLayout(this);

    QHBoxLayout* pTopLayout = new QHBoxLayout;
    pTopLayout->addWidget(m_pLineEdit, 1);
    pTopLayout->addWidget(pBtnFind);
    pTopLayout->addWidget(pBtnCurrent);

    pMainLayout->addLayout(pTopLayout);

    // Tab Bar
    m_pTabBar = new QTabBar(this);
    m_pTabBar->setElideMode(Qt::ElideRight);
    m_pTabBar->addTab(tr(STR_TAB_ROOT));
    m_pTabBar->setTabToolTip(0, tr("Set classification in \"Option\\Classification\""));
    connect(m_pTabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabCurrentChanged(int)));
    m_pTabBar->hide();

    pMainLayout->addWidget(m_pTabBar);
    pMainLayout->addWidget(m_pTableWidget);

    pMainLayout->setContentsMargins(2, 2, 2, 0);

}

void CDlgFind::onFind()
{
    m_strFindText = m_pLineEdit->text();
    if(m_strFindText.isEmpty())
    {
        onCurrent();
    }
    else
    {
        if(m_pTabBar->currentIndex() == IDX_TAB_ROOT)
            refresh();
        else
            m_pTabBar->setCurrentIndex(0);
    }
}

bool CDlgFind::needRefreshAfterChangeMonth()
{
    if(m_strFindText.isEmpty())
    {
        if(!m_pTabBar->isVisible())
            return true;

        if(IDX_TAB_ROOT == m_pTabBar->currentIndex())
            return true;
    }

    return false;
}

void CDlgFind::onCurrent()
{
    m_strFindText.clear();
    m_pLineEdit->clear();
    if(m_pTabBar->currentIndex() == IDX_TAB_ROOT)
        refresh();
    else
        m_pTabBar->setCurrentIndex(0);
}

void CDlgFind::afterAddNoteItem()
{
    refresh();
}

void CDlgFind::refresh()
{
    CNoteItem* pSelectedNoteItem = m_pTableWidget->getSelectedNoteItem();
    clearContent();

    // find tab
    if(!m_pTabBar->isVisible() || IDX_TAB_ROOT == m_pTabBar->currentIndex())
    {
        QList<CNoteItem*> lstNoteItems;
        if(m_strFindText.isEmpty())
            lstNoteItems = G_MainFrame()->getCurrentNotes();
        else
            lstNoteItems = G_MainFrame()->getNotesByString(m_strFindText);

        m_pTableWidget->setNoteItemList(lstNoteItems);
    }
    // classify tab
    else
    {
        QString strTab = m_pTabBar->tabText(m_pTabBar->currentIndex());

        ST_CLASSIFY_TYPE classifyType;
        if(!g_parseClassifyString(strTab, &classifyType) || classifyType.strClassifyString.isEmpty())
        {
            return;
        }

        QList<CNoteItem*> lstNoteItems = G_MainFrame()->getLocalDbManager()->getNotesByString(classifyType.strClassifyString, true, classifyType.nIncludeType);
        m_pTableWidget->setNoteItemList(lstNoteItems);
    }

    G_MainFrame()->selectNoteItem(pSelectedNoteItem);
}

void CDlgFind::clearNoteItems()
{
    m_pTableWidget->clearNoteItems();
}

void CDlgFind::clearContent()
{
    m_pTableWidget->clearContent();
}

void CDlgFind::selectNoteItem(CNoteItem* pNoteItem)
{
    m_pTableWidget->selectNoteItem(pNoteItem);
}

void CDlgFind::onTabCurrentChanged(int)
{
    // tab
    refresh();
}

void CDlgFind::updateTabLabels(const QStringList& lstStrings)
{
    while(m_pTabBar->count() > 1)
        m_pTabBar->removeTab(1);

    if(lstStrings.length())
    {
        m_pTabBar->show();
    }
    else
    {
        m_pTabBar->hide();
        return;
    }

    int nIdx2 = m_pTabBar->currentIndex();

    QString strCurrentText = m_pTabBar->tabText(nIdx2);

    int nIdx = 0;
    int nSelectedIdx = 0;
    foreach(QString str, lstStrings)
    {
        nIdx = m_pTabBar->addTab(str);
        if(strCurrentText == str)
        {
            nSelectedIdx = nIdx;
        }
        m_pTabBar->setTabToolTip(nIdx, g_getClassStringToolTip(str));
    }

    m_pTabBar->setCurrentIndex(nSelectedIdx);
}
