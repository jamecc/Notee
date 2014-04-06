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
#include "MainFrame.h"

///////////////////////////////////////////////////////////////////////////////////////////
// class CSimplePage
CSimplePage::CSimplePage(QWidget *parent, CSettings* pSettings) :
    QWidget(parent),
    m_pSettings(pSettings),
    m_bModified(false)
{

}

///////////////////////////////////////////////////////////////////////////////////////////
// class CConfigurationPage
CConfigurationPage::CConfigurationPage(QWidget *parent, CSettings* pSettings) :
    CSimplePage(parent, pSettings)
{
 //   QGroupBox *configGroup = new QGroupBox(tr("Server configuration"));

//    QLabel *serverLabel = new QLabel(tr("Server:"));

    m_pChkShowLunar = new QCheckBox(this);
    m_pChkShowLunar->setText(tr("Show Lunar Calendar"));
    connect(m_pChkShowLunar, SIGNAL(stateChanged(int)), this, SLOT(onModified()));

    QLabel* pLabelEditNoteFontSize = new QLabel(this);
    pLabelEditNoteFontSize->setText(tr("Font size of plain note:"));
    pLabelEditNoteFontSize->setAlignment(Qt::AlignLeft);
    m_pEditNoteFontSize = new QLineEdit(this);
    m_pEditNoteFontSize->setMaximumSize(QSize(30,20));
    m_pEditNoteFontSize->setInputMask("99");
    connect(m_pEditNoteFontSize, SIGNAL(textChanged(QString)), this, SLOT(onModified()));
    m_pEditNoteFontSize->setEnabled(false);

    m_pSliderFontSize = new QSlider(Qt::Horizontal, this);
    m_pSliderFontSize->setRange(6,64);
    m_pSliderFontSize->setSingleStep(2);
    m_pSliderFontSize->setPageStep(10);
    connect(m_pSliderFontSize, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));

    QHBoxLayout* pLayoutFontSize = new QHBoxLayout;
    pLayoutFontSize->addWidget(pLabelEditNoteFontSize);
    pLayoutFontSize->addWidget(m_pEditNoteFontSize);
    pLayoutFontSize->addWidget(m_pSliderFontSize);

 /*   QComboBox *serverCombo = new QComboBox;
    serverCombo->addItem(tr("Qt (Australia)"));
    serverCombo->addItem(tr("Qt (Germany)"));
    serverCombo->addItem(tr("Qt (Norway)"));
    serverCombo->addItem(tr("Qt (People's Republic of China)"));
    serverCombo->addItem(tr("Qt (USA)"));
    */
/*
    QHBoxLayout *serverLayout = new QHBoxLayout;
    serverLayout->addWidget(serverLabel);
    serverLayout->addWidget(serverCombo);

    QVBoxLayout *configLayout = new QVBoxLayout;
    configLayout->addLayout(serverLayout);
    configGroup->setLayout(configLayout);

    */

    // first day
    QGroupBox* pGroupFirstDayOfWeek = new QGroupBox(this);
    pGroupFirstDayOfWeek->setTitle(tr("First day of a week"));
    QVBoxLayout* pLayoutFirstDay = new QVBoxLayout;
    pGroupFirstDayOfWeek->setLayout(pLayoutFirstDay);

    m_pRadioMonday = new QRadioButton(this);
    m_pRadioMonday->setText(tr("Monday"));
    m_pRadioSaturday = new QRadioButton(this);
    m_pRadioSaturday->setText(tr("Saturday"));
    m_pRadioSunday = new QRadioButton(this);
    m_pRadioSunday->setText(tr("Sunday"));

    pLayoutFirstDay->addWidget(m_pRadioMonday);
    pLayoutFirstDay->addWidget(m_pRadioSaturday);
    pLayoutFirstDay->addWidget(m_pRadioSunday);

    connect(m_pRadioMonday, SIGNAL(clicked(bool)), this, SLOT(onModified()));
    connect(m_pRadioSaturday, SIGNAL(clicked(bool)), this, SLOT(onModified()));
    connect(m_pRadioSunday, SIGNAL(clicked(bool)), this, SLOT(onModified()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_pChkShowLunar);
    mainLayout->addLayout(pLayoutFontSize);
    mainLayout->addWidget(pGroupFirstDayOfWeek);
    setLayout(mainLayout);

    // init
    initControls();
}

void CConfigurationPage::initControls()
{
    m_pChkShowLunar->setChecked(m_pSettings->isShowLunar());
 //   m_pEditNoteFontSize->setText(QString::number(m_pSettings->getEditNoteFontSize()));
    m_pSliderFontSize->setValue(m_pSettings->getEditNoteFontSize());
    // saturday
    if(FIRST_DAY_SATURDAY == m_pSettings->getFirstDayOfWeek())
    {
        m_pRadioSaturday->setChecked(true);
    }
    // sunday
    else if(FIRST_DAY_SUNDAY == m_pSettings->getFirstDayOfWeek())
    {
        m_pRadioSunday->setChecked(true);
    }
    // monday
    else
        m_pRadioMonday->setChecked(true);
    m_bModified = false;
}

void CConfigurationPage::saveToSettings()
{
    if(m_pSettings && m_bModified)
    {
        m_pSettings->setShowLunar(m_pChkShowLunar->isChecked());
        QString strFontSize = m_pEditNoteFontSize->text().trimmed();
        if(strFontSize.isEmpty())
            m_pSettings->setEditNoteFontSize(12);
        else
            m_pSettings->setEditNoteFontSize(strFontSize.toInt());

        if(m_pRadioSaturday->isChecked())
        {
            m_pSettings->setFirstDayOfWeek(FIRST_DAY_SATURDAY);
        }
        else if(m_pRadioSunday->isChecked())
        {
            m_pSettings->setFirstDayOfWeek(FIRST_DAY_SUNDAY);
        }
        else
        {
            m_pSettings->setFirstDayOfWeek(FIRST_DAY_MONDAY);
        }
        m_pSettings->save();
        m_bModified = false;
    }
}

void CConfigurationPage::onModified()
{
    m_bModified = true;
}

void CConfigurationPage::onSliderValueChanged(int nValue)
{
    m_pEditNoteFontSize->setText(QString::number(nValue));
    onModified();
}

///////////////////////////////////////////////////////////////////////////////////////////
// class CClassifyPage
enum {
    IDX_ROW_RULE = 0,
    IDX_ROW_TEXT
};
CClassifyPage::CClassifyPage(QWidget *parent, CSettings* pSettings)
    : CSimplePage(parent, pSettings)
{
    m_pBtnAdd = new QPushButton(this);
    m_pBtnAdd->setToolTip(tr("Add"));
    m_pBtnAdd->setIconSize(QSize(16,16));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/res/cls_add.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pBtnAdd->setIcon(icon1);

    m_pBtnDelete = new QPushButton(this);
    m_pBtnDelete->setToolTip(tr("Add"));
    m_pBtnDelete->setIconSize(QSize(16,16));
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/res/cls_remove.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pBtnDelete->setIcon(icon2);

    m_pBtnUp = new QPushButton(this);
    m_pBtnUp->setToolTip(tr("Up"));
    m_pBtnUp->setIconSize(QSize(16,16));
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/res/cls_up.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pBtnUp->setIcon(icon3);

    m_pBtnDown = new QPushButton(this);
    m_pBtnDown->setToolTip(tr("Down"));
    m_pBtnDown->setIconSize(QSize(16,16));
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/res/cls_down.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pBtnDown->setIcon(icon4);

    m_pTableWidget = new QTableWidget(this);
    m_pTableWidget->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
    m_pTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_pTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_pTableWidget->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModified()));
    connect(m_pTableWidget->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onTableSelectedChanged()));

    QHBoxLayout* pLayoutBtns = new QHBoxLayout;
    pLayoutBtns->addWidget(m_pBtnAdd);
    pLayoutBtns->addWidget(m_pBtnDelete);
    pLayoutBtns->addWidget(m_pBtnUp);
    pLayoutBtns->addWidget(m_pBtnDown);

    QVBoxLayout* pLayoutMain = new QVBoxLayout(this);
    pLayoutMain->addLayout(pLayoutBtns);
    pLayoutMain->addWidget(m_pTableWidget);
    pLayoutMain->setMargin(0);

    connect(m_pBtnAdd, SIGNAL(clicked()), this, SLOT(onAdd()));
    connect(m_pBtnDelete, SIGNAL(clicked()), this, SLOT(onDelete()));
    connect(m_pBtnUp, SIGNAL(clicked()), this, SLOT(onUp()));
    connect(m_pBtnDown, SIGNAL(clicked()), this, SLOT(onDown()));

    // init
    initControls();

    m_bModified = false;
}

void CClassifyPage::saveToSettings()
{
    QString str, strString;
    for(int i = 0; i < m_pTableWidget->rowCount(); ++i)
    {
        if(!str.isEmpty())
            str += ";";
        strString = m_pTableWidget->item(i, IDX_ROW_TEXT)->text();
        if(strString.isEmpty())
            continue;
        strString = strString.replace('\\', "\\\\");
        strString = strString.replace(';', "\\;");

        QComboBox* pComboBox = qobject_cast<QComboBox*>(m_pTableWidget->cellWidget(i, IDX_ROW_RULE));
        str += g_getClassString(g_getClassTypeInt(pComboBox->currentText()), strString);
    }

    if(G_MainFrame()->getLocalDbManager()->getClassifyLabels() != str)
    {
        G_MainFrame()->getLocalDbManager()->updateClassifyLabels(str, G_MainFrame()->getLocalDbManager()->getClassifyLabelsAtServer());
        G_MainFrame()->updateClassifyString(str);
        G_MainFrame()->refreshTabClassifyStrings();
    }
}

void CClassifyPage::onTableSelectedChanged()
{
    updateControlState();
}

void CClassifyPage::onModified()
{
    m_bModified = true;
}

void CClassifyPage::initControls()
{
    G_MainFrame()->getLocalDbManager()->getClassifyTypesList();
    m_pTableWidget->setColumnCount(2);
    QStringList header;
    header << tr("Rule of title") << tr("Text");
    m_pTableWidget->setHorizontalHeaderLabels(header);


    int nIdx = 0;
    QTableWidgetItem* pWidgetItem = NULL;
    foreach(ST_CLASSIFY_TYPE* pType, G_MainFrame()->getLocalDbManager()->getClassifyTypesList())
    {
        nIdx = m_pTableWidget->rowCount();
        m_pTableWidget->insertRow(nIdx);

 //       pWidgetItem = new QTableWidgetItem;
        QComboBox* pComboBox = new QComboBox;
        connect(pComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onModified()));
        pComboBox->addItem(g_getClassTypeString(INCLUDE_TYPE_LEADING), QVariant(INCLUDE_TYPE_LEADING));
        pComboBox->addItem(g_getClassTypeString(INCLUDE_TYPE_INCLUDE), QVariant(INCLUDE_TYPE_INCLUDE));
        pComboBox->addItem(g_getClassTypeString(INCLUDE_TYPE_ENDING), QVariant(INCLUDE_TYPE_ENDING));
        pComboBox->setCurrentIndex(pType->nIncludeType);
//        pWidgetItem->setText(g_getClassTypeString(pType->nIncludeType));
//        m_pTableWidget->setItem(nIdx, 1, pWidgetItem);
        m_pTableWidget->setCellWidget(nIdx, IDX_ROW_RULE, pComboBox);

        pWidgetItem = new QTableWidgetItem;
        pWidgetItem->setText(pType->strClassifyString);
        m_pTableWidget->setItem(nIdx, IDX_ROW_TEXT, pWidgetItem);
    }

//    m_pTableWidget->setCurrentCell(0,0, QItemSelectionModel::Select);

//    m_pTableWidget->selectionModel()->select(m_pTableWidget->model()->index(0, IDX_ROW_TEXT), QItemSelectionModel::ClearAndSelect);
//    m_pTableWidget->setFocus();
    updateControlState();
}

void CClassifyPage::onAdd()
{
    int nIdx = m_pTableWidget->rowCount();
    m_pTableWidget->insertRow(nIdx);

    QComboBox* pComboBox = new QComboBox;
    connect(pComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onModified()));
    pComboBox->addItem(g_getClassTypeString(INCLUDE_TYPE_LEADING), QVariant(INCLUDE_TYPE_LEADING));
    pComboBox->addItem(g_getClassTypeString(INCLUDE_TYPE_INCLUDE), QVariant(INCLUDE_TYPE_INCLUDE));
    pComboBox->addItem(g_getClassTypeString(INCLUDE_TYPE_ENDING), QVariant(INCLUDE_TYPE_ENDING));
    pComboBox->setCurrentIndex(INCLUDE_TYPE_LEADING);
    m_pTableWidget->setCellWidget(nIdx, IDX_ROW_RULE, pComboBox);

    QTableWidgetItem* pItem = new QTableWidgetItem;
    m_pTableWidget->setItem(nIdx, IDX_ROW_TEXT, pItem);

//    m_pTableWidget->setCurrentIndex(m_pTableWidget->model()->index(nIdx, 0));
//    m_pTableWidget->selectionModel()->clearSelection();
    m_pTableWidget->selectionModel()->select(m_pTableWidget->model()->index(nIdx, IDX_ROW_TEXT),
                                             QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
//    m_pTableWidget->setCurrentCell(nIdx,0, QItemSelectionModel::Select);
    m_pTableWidget->setFocus();
//    updateControlState();
}

void CClassifyPage::onDelete()
{
    int nIdxRow = -1;

    if(m_pTableWidget->selectionModel()->selectedIndexes().length() > 0)
        nIdxRow = m_pTableWidget->selectionModel()->selectedIndexes().at(0).row();

    if(nIdxRow < 0)
    {
        updateControlState();
        return;
    }

    m_pTableWidget->removeRow(nIdxRow);

    if(nIdxRow >= m_pTableWidget->model()->rowCount())
        nIdxRow = m_pTableWidget->model()->rowCount() - 1;

    m_bModified = true;

    if(nIdxRow < 0)
    {
        updateControlState();
        return;
    }

//    m_pTableWidget->selectionModel()->select(m_pTableWidget->model()->index(nIdxRow, 0), QItemSelectionModel::Clear | QItemSelectionModel::Select);
    m_pTableWidget->selectionModel()->select(m_pTableWidget->model()->index(nIdxRow, IDX_ROW_TEXT),
                                             QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    m_pTableWidget->setFocus();
//    updateControlState();
}

void CClassifyPage::onUp()
{
    int nIdxCurrent = -1;

    if(m_pTableWidget->selectionModel()->selectedIndexes().length() > 0)
        nIdxCurrent = m_pTableWidget->selectionModel()->selectedIndexes().at(0).row();

    // current
    QComboBox* pComboBoxCurrent = qobject_cast<QComboBox*>(m_pTableWidget->cellWidget(nIdxCurrent, IDX_ROW_RULE));
    int nRuleCurrent = pComboBoxCurrent->currentIndex();
    QString strTextCurrent = m_pTableWidget->item(nIdxCurrent, IDX_ROW_TEXT)->text();

    // previous one
    QComboBox* pComboBoxPrev = qobject_cast<QComboBox*>(m_pTableWidget->cellWidget(nIdxCurrent-1, IDX_ROW_RULE));
    int nRulePrev = pComboBoxPrev->currentIndex();
    QString strTextPrev = m_pTableWidget->item(nIdxCurrent-1, IDX_ROW_TEXT)->text();

    // set values
    pComboBoxCurrent->setCurrentIndex(nRulePrev);
    m_pTableWidget->item(nIdxCurrent, IDX_ROW_TEXT)->setText(strTextPrev);

    pComboBoxPrev->setCurrentIndex(nRuleCurrent);
    m_pTableWidget->item(nIdxCurrent-1, IDX_ROW_TEXT)->setText(strTextCurrent);


 //   m_pTableWidget->selectionModel()->clearSelection();
    m_pTableWidget->selectionModel()->select(m_pTableWidget->model()->index(nIdxCurrent-1, IDX_ROW_TEXT),
                                             QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    m_pTableWidget->setFocus();
//    updateControlState();
}

void CClassifyPage::onDown()
{
    int nIdxCurrent = -1;

    if(m_pTableWidget->selectionModel()->selectedIndexes().length() > 0)
        nIdxCurrent = m_pTableWidget->selectionModel()->selectedIndexes().at(0).row();

    // current
    QComboBox* pComboBoxCurrent = qobject_cast<QComboBox*>(m_pTableWidget->cellWidget(nIdxCurrent, IDX_ROW_RULE));
    int nRuleCurrent = pComboBoxCurrent->currentIndex();
    QString strTextCurrent = m_pTableWidget->item(nIdxCurrent, IDX_ROW_TEXT)->text();

    // next one
    QComboBox* pComboBoxNext = qobject_cast<QComboBox*>(m_pTableWidget->cellWidget(nIdxCurrent+1, IDX_ROW_RULE));
    int nRuleNext = pComboBoxNext->currentIndex();
    QString strTextNext = m_pTableWidget->item(nIdxCurrent+1, IDX_ROW_TEXT)->text();

    // set values
    pComboBoxCurrent->setCurrentIndex(nRuleNext);
    m_pTableWidget->item(nIdxCurrent, IDX_ROW_TEXT)->setText(strTextNext);

    pComboBoxNext->setCurrentIndex(nRuleCurrent);
    m_pTableWidget->item(nIdxCurrent+1, IDX_ROW_TEXT)->setText(strTextCurrent);

//    m_pTableWidget->selectionModel()->clearSelection();
    m_pTableWidget->selectionModel()->select(m_pTableWidget->model()->index(nIdxCurrent+1, IDX_ROW_TEXT),
                                             QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
//    m_pTableWidget->setCurrentCell(nIdxCurrent+1,0, QItemSelectionModel::Select);
    m_pTableWidget->setFocus();
//    updateControlState();
}

void CClassifyPage::updateControlState()
{
    int nIdxCurrent = -1;

    if(m_pTableWidget->selectionModel()->selectedIndexes().length() > 0)
        nIdxCurrent = m_pTableWidget->selectionModel()->selectedIndexes().at(0).row();

    m_pBtnDelete->setEnabled(nIdxCurrent >=0);
    m_pBtnUp->setEnabled(nIdxCurrent > 0);
    m_pBtnDown->setEnabled(nIdxCurrent >=0 && nIdxCurrent < (m_pTableWidget->model()->rowCount() - 1));
    m_pTableWidget->setFocus();
}
