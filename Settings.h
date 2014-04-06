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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QSettings>
#include <QDateTime>

#define FIRST_DAY_MONDAY    1
#define FIRST_DAY_SATURDAY  6
#define FIRST_DAY_SUNDAY    7

class CSettings : public QSettings
{
public:
    CSettings(const QString &organization, const QString &application = QString());
    ~CSettings();

    void read();
    void save();

    bool isAlwaysLogin();
    QString getLoginName();
    QString getLoginPassword();
    void setAlwaysLogin(bool bVal);
    void setLoginName(const QString& str);
    void setLoginPassword(const QString& str);

    int getFirstDayOfWeek() const       {return m_nFirstDayOfWeek;}
    void setFirstDayOfWeek(int nVal)    {m_nFirstDayOfWeek = nVal;}

    bool isShowLunar();
    void setShowLunar(bool bShowLunar);

    QString getLang() const;
    void setLang(const QString& strLang);

    const QByteArray& getMainFrameState();
    void setMainFrameState(const QByteArray& arr);

    bool getCheckPadRegular() const;
    QDateTime getPadFileTime() const;
    QString getNewVersion() const;
    QString getReleaseDate() const;
    QString getChangeInfo() const;
    QString getFileSizeMB() const;
    QString getDownloadUrl() const;
    void setCheckPadRegular(bool bCheck);
    void setPadFileTime(const QDateTime& dt);
    void setNewVersion(const QString& str);
    void setReleaseDate(const QString& str);
    void setChangeInfo(const QString& str);
    void setFileSizeMB(const QString& str);
    void setDownloadUrl(const QString& str);

    bool isShowViewFind() const;
    bool isShowViewContent() const;
    void setShowViewFind(bool bValue);
    void setShowViewContent(bool bValue);

    bool isShowViewProgress() const;
    void setShowViewProgress(bool bValue);

    bool isShowCalendarTable() const;
    void setShowCalendarTable(bool bVal);

    bool isEditNoteHtml() const;
    void setEditNoteHtml(bool bVal);
    int getEditNoteFontSize() const;
    void setEditNoteFontSize(int nVal);

private:
    bool m_bAlwaysLogin;
    QString m_strLoginName;
    QString m_strLoginPassword;
    QString m_strLanguage;
    int m_nEditNoteFontSize;

    bool m_bShowLunar;
    bool m_bShowViewFind;
    bool m_bShowViewContent;
    bool m_bShowViewProgress;
    bool m_bEditNoteHtml;
    bool m_bShowCalendarTable;

    QByteArray m_arrMainFrameState;

    // pad file
    bool m_bCheckPadRegular;
    QDateTime m_dtPadFileTime;
    QString m_strNewVersion;
    QString m_strReleaseDate;
    QString m_strChangeInfo;
    QString m_strFileSizeMB;
    QString m_strDownloadUrl;

    int m_nFirstDayOfWeek;
};

#endif // SETTINGS_H
