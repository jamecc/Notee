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

#ifndef CONFIGPAGES_H
#define CONFIGPAGES_H

#include <QWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <Settings.h>
#include <QPushButton>
#include <QTableWidget>


///////////////////////////////////////////////////////////////////////////////////////////
// class CSimplePage
class CSimplePage : public QWidget
{
    Q_OBJECT
public:
    CSimplePage(QWidget *parent = 0, CSettings* pSettings = 0);

    bool isModified() const         {return m_bModified;}
    virtual void saveToSettings() = 0;

signals:

public slots:

protected:
    CSettings* m_pSettings;
    bool m_bModified;
};

///////////////////////////////////////////////////////////////////////////////////////////
// class CConfigurationPage
class CConfigurationPage : public CSimplePage
{
    Q_OBJECT
public:
    CConfigurationPage(QWidget *parent = 0, CSettings* pSettings = 0);

    virtual void saveToSettings();

signals:

public slots:
    void onModified();
    void onSliderValueChanged(int);

protected:
    void initControls();

private:
    QCheckBox* m_pChkShowLunar;
    QRadioButton* m_pRadioMonday;
    QRadioButton* m_pRadioSaturday;
    QRadioButton* m_pRadioSunday;
    QLineEdit* m_pEditNoteFontSize;
    QSlider* m_pSliderFontSize;
};

///////////////////////////////////////////////////////////////////////////////////////////
// class CClassifyPage
class CClassifyPage : public CSimplePage
{
    Q_OBJECT
public:
    CClassifyPage(QWidget *parent = 0, CSettings* pSettings = 0);

    virtual void saveToSettings();

signals:

public slots:
    void onModified();
    void onAdd();
    void onDelete();
    void onUp();
    void onDown();
    void onTableSelectedChanged();

protected:
    void initControls();
    void updateControlState();

private:
    QPushButton* m_pBtnAdd;
    QPushButton* m_pBtnDelete;
    QPushButton* m_pBtnUp;
    QPushButton* m_pBtnDown;
    QTableWidget* m_pTableWidget;
};

#endif // CONFIGPAGES_H
