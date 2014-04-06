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

#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <QString>

class CLunDay 
{
public:
	enum EnumLunDayMode{LUNDAY_YEAR = 0, LUNDAY_MONTH, LUNDAY_DAY, LUNDAY_ALL};

public:
	static QString GetLunarCalendar(QDate dtDay, int nMode = LUNDAY_YEAR);
};

QString getCurrentIp();
QString G_getMonthStr(int nYear, int nMonth);
QString G_getMonthString(int nMonth);

QString G_getFileSize(qint64 nSize);

bool G_getFileMd5(const QString& filePath, QByteArray& arrMD5);

#endif // #ifndef COMMONFUNCTIONS_H
