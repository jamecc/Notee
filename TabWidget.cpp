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

#include "TabWidget.h"
#include <QVBoxLayout>

#include "MainFrame.h"

#define STR_NO_CLASSIFICATION   "(No Classify)"
CTabWidget::CTabWidget(QWidget *parent) :
    QWidget(parent)
{
    m_bInit = true;
    QVBoxLayout* pLayout = new QVBoxLayout(this);
    pLayout->setMargin(0);

    // Tab Bar
    m_pTabBar = new QTabBar(this);
    m_pTabBar->setElideMode(Qt::ElideRight);
    connect(m_pTabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabCurrentChanged(int)));

    // table widget
    m_pTableWidget = new CDCTableWidget(this);
    m_pTableWidget->setColumnWidth(2, 300);

    pLayout->addWidget(m_pTabBar);
    pLayout->addWidget(m_pTableWidget);
//    setLayout(pLayout);
    initControls();
    m_bInit = false;
}

void CTabWidget::initControls()
{
    updateTabLabels(QStringList());
}

void CTabWidget::updateTabLabels(const QStringList& lstStrings)
{
    int nIdx2 = m_pTabBar->currentIndex();
    QString strCurrentText = m_pTabBar->tabText(nIdx2);

    while(m_pTabBar->count())
        m_pTabBar->removeTab(0);

    int nIdx = 0;
    int nSelectedIdx = 0;
    if(lstStrings.length())
    {
        foreach(QString str, lstStrings)
        {
            nIdx = m_pTabBar->addTab(str);
            if(strCurrentText == str)
            {
                nSelectedIdx = nIdx;
            }
            m_pTabBar->setTabToolTip(nIdx, g_getClassStringToolTip(str));
        }
    }
    else
    {
        m_pTabBar->addTab(tr(STR_NO_CLASSIFICATION));
        m_pTabBar->setTabToolTip(0, tr("Set classification in \"Option\\Classification\""));
    }

    m_pTabBar->setCurrentIndex(nSelectedIdx);
}

void CTabWidget::clearContent()
{
    m_pTableWidget->clearContent();
}

void CTabWidget::onTabCurrentChanged(int)
{
    if(m_bInit)
        return;

    refresh();
}

void CTabWidget::refresh()
{
    QString strTab = m_pTabBar->tabText(m_pTabBar->currentIndex());

    if(strTab == tr(STR_NO_CLASSIFICATION))
    {
        clearContent();
        return;
    }

    ST_CLASSIFY_TYPE classifyType;
    if(!g_parseClassifyString(strTab, &classifyType) || classifyType.strClassifyString.isEmpty())
    {
        return;
    }

    QList<CNoteItem*> lstNoteItems = G_MainFrame()->getLocalDbManager()->getNotesByString(classifyType.strClassifyString, true, classifyType.nIncludeType);
    m_pTableWidget->setNoteItemList(lstNoteItems);
}

void CTabWidget::selectNoteItem(CNoteItem* pNoteItem)
{
    m_pTableWidget->selectNoteItem(pNoteItem);
}

