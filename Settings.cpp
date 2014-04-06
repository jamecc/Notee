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

#include "Settings.h"

CSettings::CSettings(const QString &organization, const QString &application)
    : QSettings(organization, application)
    , m_bAlwaysLogin(false)
{
    m_bCheckPadRegular = true;
    read();
}

CSettings::~CSettings()
{
    save();
}

void CSettings::read()
{
    m_bAlwaysLogin = value("Always Login", false).toBool();
    m_strLoginName = value("Login Name", "").toString().trimmed();
    m_strLoginPassword = value("Login Password", "").toString().trimmed();

    m_nFirstDayOfWeek = value("First day of week", FIRST_DAY_SUNDAY).toInt();

    m_bShowLunar = value("Show Lunar", false).toBool();
    m_strLanguage = value("Language", "cn").toString().trimmed();

    // show view
    m_bShowViewFind = value("Show View Find", true).toBool();
    m_bShowViewContent = value("Show View Content", true).toBool();
    m_bShowViewProgress = value("Show View Progress", true).toBool();

    m_bShowCalendarTable = value("Show Calendar Table", true).toBool();

    m_bEditNoteHtml = value("Edit Note HTML", true).toBool();
    m_nEditNoteFontSize = value("Edit Note Font Size", 12).toInt();

    QByteArray arrBuffers = value("Main Frame State").toByteArray();
    if(arrBuffers.length())
        m_arrMainFrameState = arrBuffers;

    // pad file
    m_dtPadFileTime = value("PadFileTime").toDateTime();
    m_strNewVersion = value("NewVersion").toString();
    m_strReleaseDate = value("ReleaseDate").toString();
    m_strChangeInfo = value("ChangeInfo").toString();
    m_strFileSizeMB = value("FileSizeMB").toString();
    m_strDownloadUrl = value("DownloadUrl").toString();
}

void CSettings::save()
{
    setValue("Always Login", m_bAlwaysLogin);
    setValue("Login Name", m_strLoginName);
    setValue("Login Password", m_strLoginPassword);

    setValue("First day of week", m_nFirstDayOfWeek);

    setValue("Show Lunar", m_bShowLunar);
    setValue("Language", m_strLanguage);

    // show view
    setValue("Show View Find", m_bShowViewFind);
    setValue("Show View Content", m_bShowViewContent);
    setValue("Show View Progress", m_bShowViewProgress);

    setValue("Show Calendar Table", m_bShowCalendarTable);

    setValue("Edit Note HTML", m_bEditNoteHtml);
    setValue("Edit Note Font Size", m_nEditNoteFontSize);

    setValue("Main Frame State", m_arrMainFrameState);

    // pad file
    setValue("PadFileTime", m_dtPadFileTime);
    setValue("NewVersion", m_strNewVersion);
    setValue("ReleaseDate", m_strReleaseDate);
    setValue("ChangeInfo", m_strChangeInfo);
    setValue("FileSizeMB", m_strFileSizeMB);
    setValue("DownloadUrl", m_strDownloadUrl);
}

bool CSettings::isAlwaysLogin()
{
    return m_bAlwaysLogin;
}

QString CSettings::getLoginName()
{
    return m_strLoginName;
}

QString CSettings::getLoginPassword()
{
    return m_strLoginPassword;
}

void CSettings::setAlwaysLogin(bool bVal)
{
    m_bAlwaysLogin = bVal;
}

void CSettings::setLoginName(const QString& str)
{
    m_strLoginName = str;
}

void CSettings::setLoginPassword(const QString& str)
{
    m_strLoginPassword = str;
}

bool CSettings::isShowLunar()
{
    return m_bShowLunar;
}

void CSettings::setShowLunar(bool bShowLunar)
{
    m_bShowLunar = bShowLunar;
}

QString CSettings::getLang() const
{
    return m_strLanguage;
}

void CSettings::setLang(const QString& strLang)
{
    m_strLanguage = strLang;
}

const QByteArray& CSettings::getMainFrameState()
{
    return m_arrMainFrameState;
}

void CSettings::setMainFrameState(const QByteArray& arr)
{
    m_arrMainFrameState = arr;
}

// pad file - get
bool CSettings::getCheckPadRegular() const
{
    return m_bCheckPadRegular;
}

QDateTime CSettings::getPadFileTime() const
{
    return m_dtPadFileTime;
}

QString CSettings::getNewVersion() const
{
    return m_strNewVersion;
}

QString CSettings::getReleaseDate() const
{
    return m_strReleaseDate;
}

QString CSettings::getChangeInfo() const
{
    return m_strChangeInfo;
}

QString CSettings::getFileSizeMB() const
{
    return m_strFileSizeMB;
}

QString CSettings::getDownloadUrl() const
{
    return m_strDownloadUrl;
}

// pad file - set
void CSettings::setCheckPadRegular(bool bCheck)
{
    m_bCheckPadRegular = bCheck;
}

void CSettings::setPadFileTime(const QDateTime& dt)
{
    m_dtPadFileTime = dt;
}

void CSettings::setNewVersion(const QString& str)
{
    m_strNewVersion = str;
}

void CSettings::setReleaseDate(const QString& str)
{
    m_strReleaseDate = str;
}

void CSettings::setChangeInfo(const QString& str)
{
    m_strChangeInfo = str;
}

void CSettings::setFileSizeMB(const QString& str)
{
    m_strFileSizeMB = str;
}

void CSettings::setDownloadUrl(const QString& str)
{
    m_strDownloadUrl = str;
}

bool CSettings::isShowViewFind() const
{
    return m_bShowViewFind;
}

bool CSettings::isShowViewContent() const
{
    return m_bShowViewContent;
}

bool CSettings::isShowViewProgress() const
{
    return m_bShowViewProgress;
}

void CSettings::setShowViewProgress(bool bValue)
{
    m_bShowViewProgress = bValue;
}

void CSettings::setShowViewFind(bool bValue)
{
    m_bShowViewFind = bValue;
}

void CSettings::setShowViewContent(bool bValue)
{
    m_bShowViewContent = bValue;
}

bool CSettings::isShowCalendarTable() const
{
    return m_bShowCalendarTable;
}

void CSettings::setShowCalendarTable(bool bVal)
{
    m_bShowCalendarTable = bVal;
}

bool CSettings::isEditNoteHtml() const
{
    return m_bEditNoteHtml;
}

void CSettings::setEditNoteHtml(bool bVal)
{
    m_bEditNoteHtml = bVal;
}

int CSettings::getEditNoteFontSize() const
{
    return m_nEditNoteFontSize;
}

void CSettings::setEditNoteFontSize(int nVal)
{
    m_nEditNoteFontSize = nVal;
}

