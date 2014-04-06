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
#include "Settings.h"
#include "ConfigPages.h"
#include "ConfigDialog.h"

CConfigDialog::CConfigDialog(QWidget *parent, CSettings* pSettings) :
    QDialog(parent),
    m_pSettings(pSettings),
    m_bModified(false)
{
    setWindowTitle(tr("Configuration"));

    m_pListContentWidget = new QListWidget;
    m_pListContentWidget->setViewMode(QListView::IconMode);
    m_pListContentWidget->setIconSize(QSize(48,48));
    m_pListContentWidget->setMovement(QListView::Static);
    m_pListContentWidget->setMaximumWidth(110);
    m_pListContentWidget->setSpacing(12);
    connect(m_pListContentWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onChangePage(QListWidgetItem*,QListWidgetItem*)));

    // add pages
    m_pPagesWidget = new QStackedWidget;
    m_pPagesWidget->addWidget(new CConfigurationPage(this, m_pSettings));
    m_pPagesWidget->addWidget(new CClassifyPage(this, m_pSettings));

    QPushButton* pOkButton = new QPushButton(tr("Ok"));
    connect(pOkButton, SIGNAL(clicked()), this, SLOT(onOk()));

    QPushButton* pCancelButton = new QPushButton(tr("Cancel"));
    connect(pCancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout* pHorizontalLayout = new QHBoxLayout;
    pHorizontalLayout->addWidget(m_pListContentWidget);
    pHorizontalLayout->addSpacing(5);
    pHorizontalLayout->addWidget(m_pPagesWidget, 1);

    QHBoxLayout* pHButtonsLayout = new QHBoxLayout;
    pHButtonsLayout->addStretch(1);
    pHButtonsLayout->addWidget(pOkButton);
    pHButtonsLayout->addWidget(pCancelButton);

    QVBoxLayout* pMainLayout = new QVBoxLayout;
    pMainLayout->addLayout(pHorizontalLayout);
    pMainLayout->addSpacing(12);
    pMainLayout->addLayout(pHButtonsLayout);

    setLayout(pMainLayout);

    // init pages
    createPageItems();
    m_pListContentWidget->setCurrentRow(0);
}

void CConfigDialog::onChangePage(QListWidgetItem* pCurrent, QListWidgetItem* pPrevious)
{
    if(!pCurrent)
        pCurrent = pPrevious;

    m_pPagesWidget->setCurrentIndex(m_pListContentWidget->row(pCurrent));
}

void CConfigDialog::createPageItems()
{
    QListWidgetItem* pConfigBtn = new QListWidgetItem(m_pListContentWidget);
    pConfigBtn->setIcon(QIcon(":/res/config.png"));
    pConfigBtn->setText(tr("Configuration"));
    pConfigBtn->setTextAlignment(Qt::AlignCenter);
    pConfigBtn->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem* pClassifyBtn = new QListWidgetItem(m_pListContentWidget);
    pClassifyBtn->setIcon(QIcon(":/res/tb_classify.png"));
    pClassifyBtn->setText(tr("Classification"));
    pClassifyBtn->setTextAlignment(Qt::AlignCenter);
    pClassifyBtn->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

void CConfigDialog::onOk()
{
    if(m_pSettings)
    {
        m_bModified = false;
        for(int i = m_pPagesWidget->count() - 1; i >= 0; --i)
        {
            CSimplePage* pPage = qobject_cast<CSimplePage*>(m_pPagesWidget->widget(i));
            if(pPage->isModified())
            {
                pPage->saveToSettings();
                m_bModified = true;
            }
        }

        if(m_bModified)
            m_pSettings->save();
    }
    accept();
}
