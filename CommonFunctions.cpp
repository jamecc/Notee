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

#include "stdafx.h"
#include <QString>
#include <QNetworkInterface>
#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>
#include "CommonFunctions.h"

//天干
char* arrTianGan[] = {"甲","乙","丙","丁","戊","己","庚","辛","壬","癸"};

//地支
char* arrDiZhi[] = {"子","丑","寅","卯","辰","巳","午","未","申","酉","戌","亥"};

//十二生肖 
char* arrShengXiao[] = {"鼠","牛","虎","兔","龙","蛇","马","羊","猴","鸡","狗","猪"};

//农历日期 
char* arrDayName[] = {"*","初一","初二","初三","初四","初五",
"初六","初七","初八","初九","初十", 
"十一","十二","十三","十四","十五", 
"十六","十七","十八","十九","二十", 
"廿一","廿二","廿三","廿四","廿五", 
"廿六","廿七","廿八","廿九","三十"}; 

//农历月份 
char* arrMonthName[] = {"*","正","二","三","四","五","六","七","八","九","十","十一","腊"};

// 节气
char* arrJieQi[] = {"小寒","大寒","立春","雨水","惊蛰","春分","清明","谷雨","立夏","小满","芒种","夏至","小暑","大暑","立秋","处暑","白露","秋分","寒露","霜降","立冬","小雪","大雪","冬至"};


//公历月计数天 
int arrMonthAdd[] = {0,31,59,90,120,151,181,212,243,273,304,334}; 
//农历数据 
int arrLunarData[] = {2635,333387,1701,1748,267701,694,2391,133423,1175,396438 
,3402,3749,331177,1453,694,201326,2350,465197,3221,3402 
,400202,2901,1386,267611,605,2349,137515,2709,464533,1738 
,2901,330421,1242,2651,199255,1323,529706,3733,1706,398762 
,2741,1206,267438,2647,1318,204070,3477,461653,1386,2413 
,330077,1197,2637,268877,3365,531109,2900,2922,398042,2395 
,1179,267415,2635,661067,1701,1748,398772,2742,2391,330031 
,1175,1611,200010,3749,527717,1452,2742,332397,2350,3222 
,268949,3402,3493,133973,1386,464219,605,2349,334123,2709 
,2890,267946,2773,592565,1210,2651,395863,1323,2707,265877}; 

QString CLunDay::GetLunarCalendar(QDate dtDay, int nMode/* = LUNDAY_YEAR*/) 
{ 
	int year = dtDay.year(); 
	int month = dtDay.month(); 
	int day = dtDay.day();

	int nTheDate; 
	int nIsEnd; 
	int k,m,n,nBit,i; 
	//计算到初始时间1921年2月8日的天数：1921-2-8(正月初一) 
	nTheDate = (year-1921)*365+(year-1921)/4+day+arrMonthAdd[month-1] -38; 
	if((year%4 == 0) && (month > 2)) 
		nTheDate += 1; 
	//计算天干，地支，月，日 
	nIsEnd = 0; 
	m = 0; 
	k = 0; 
	n = 0; 
	while(nIsEnd != 1) 
	{ 
		if(arrLunarData[m] < 4095) 
			k = 11; 
		else 
			k = 12; 
		n = k; 
		while(n>=0) 
		{ 
			//获取LunarData[m]的第n个二进制位的值 
			nBit = arrLunarData[m]; 
			for(i=1;i<n+1;i++) 
				nBit = nBit/2; 
			nBit = nBit % 2; 
			if (nTheDate <= (29 + nBit)) 
			{ 
				nIsEnd = 1; 
				break; 
			} 
			nTheDate = nTheDate - 29 - nBit; 
			n = n - 1; 
		} 
		if(nIsEnd == 1) 
			break; 
		m = m + 1; 
	} 
	year = 1921 + m; 
	month = k - n + 1; 
	day = nTheDate; 
	//		return year+"-"+month+"-"+day; 
	// #region 格式化日期显示为三月廿四 
	if (k == 12) 
	{ 
		if (month == arrLunarData[m] / 65536 + 1) 
			month = 1 - month; 
		else if (month > arrLunarData[m] / 65536 + 1) 
			month = month - 1; 
	}

	//生肖
	QString strCalendar;
    strCalendar = QString::fromUtf8(arrShengXiao[(year-4)%60%12]) + QString::fromUtf8("年 ");
	//天干 
    strCalendar += QString::fromUtf8(arrTianGan[(year-4)%60%10]) + " ";
	//地支 
    strCalendar += QString::fromUtf8(arrDiZhi[(year-4)%60%12]) + " ";

	switch(nMode)
	{
	case LUNDAY_YEAR:
		return strCalendar;
	case LUNDAY_MONTH:
		if(month < 1) 
            return QString::fromUtf8("闰") + QString::fromUtf8(arrMonthName[-1*month]) + QString::fromUtf8("月");
		else 
            return QString::fromUtf8(arrMonthName[month]) + QString::fromUtf8("月");
		break;
	case LUNDAY_DAY:
		{
			strCalendar = "";
			if(day == 1)
			{
				if(month < 1) 
                    strCalendar = QString::fromUtf8("闰") + QString::fromUtf8(arrMonthName[-1*month]) + QString::fromUtf8("月 ");
				else 
                    strCalendar = QString::fromUtf8(arrMonthName[month]) + QString::fromUtf8("月 ");
			}

			//农历日
			if(day != 1)
                strCalendar += QString::fromUtf8(arrDayName[day]);

			return strCalendar;
		}
		
	default:
		break;
	}

	//农历月 
	if(month < 1) 
        strCalendar += QString::fromUtf8("闰") + QString::fromUtf8(arrMonthName[-1*month]) + QString::fromUtf8("月 ");
	else 
        strCalendar += QString::fromUtf8(arrMonthName[month]) + QString::fromUtf8("月 ");

	//农历日
    strCalendar += QString::fromUtf8(arrDayName[day]);

	return strCalendar; 
} 

QString getCurrentIp()
{
    QString strIp;
    /*
    QList<QHostAddress> lstIpAddresses = QNetworkInterface::allAddresses();
    foreach(QHostAddress hostAddress, lstIpAddresses)
    {
        if(!strIp.isEmpty())
            strIp += " ";
        strIp += QString::number(hostAddress.toIPv4Address());
    }
    */

    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
       if (interface.flags().testFlag(QNetworkInterface::IsUp) &&
               !interface.flags().testFlag(QNetworkInterface::IsLoopBack))
       {
           foreach (QNetworkAddressEntry entry, interface.addressEntries())
           {
               if(entry.ip().toString().contains('.'))
               {
                   if(!strIp.isEmpty())
                       strIp += " ";

                   strIp += interface.hardwareAddress();

                   if(!strIp.isEmpty())
                       strIp += " ";
                   strIp += entry.ip().toString();
               }
           }
       }
    }

    return strIp;
}

QString G_getMonthStr(int nYear, int nMonth)
{
    QString strMonth;
    if(nMonth < 10)
    {
        strMonth = QString("%1%02").arg(nYear).arg(nMonth);
    }
    else
    {
        strMonth = QString("%1%2").arg(nYear).arg(nMonth);
    }
    return strMonth;
}

bool isConnectedToNetwork()
{
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;

    for (int i = 0; i < ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        if ( iface.flags().testFlag(QNetworkInterface::IsUp)
             && !iface.flags().testFlag(QNetworkInterface::IsLoopBack) )
        {

#ifdef DEBUG
            // details of connection
            qDebug() << "name:" << iface.name() << endl
                    << "ip addresses:" << endl
                    << "mac:" << iface.hardwareAddress() << endl;
#endif

            // this loop is important
            for (int j=0; j<iface.addressEntries().count(); j++)
            {
#ifdef DEBUG
                qDebug() << iface.addressEntries().at(j).ip().toString()
                        << " / " << iface.addressEntries().at(j).netmask().toString() << endl;
#endif

                // we have an interface that is up, and has an ip address
                // therefore the link is present

                // we will only enable this check on first positive,
                // all later results are incorrect
                result = true;
                break;
            }
        }

        if(result)
            break;
    }

    return result;
}

QString G_getMonthString(int nMonth)
{
    switch(nMonth)
    {
    case 1:
        return QApplication::tr("January");
    case 2:
        return QApplication::tr("February");
    case 3:
        return QApplication::tr("March");
    case 4:
        return QApplication::tr("April");
    case 5:
        return QApplication::tr("May");
    case 6:
        return QApplication::tr("June");
    case 7:
        return QApplication::tr("July");
    case 8:
        return QApplication::tr("August");
    case 9:
        return QApplication::tr("Septemper");
    case 10:
        return QApplication::tr("October");
    case 11:
        return QApplication::tr("November");
    case 12:
        return QApplication::tr("December");
    default:
        break;
    }

    return "";
}

QString G_getFileSize(qint64 nSize)
{
    double dSize = nSize;
    QString strFormat = "%1 %2";
    // byte
    if(dSize > 1024.0)
    {
        dSize = dSize / 1024.0;
    }
    else
    {
        return QString(strFormat).arg(dSize, 0, 'f', 2).arg("B");
    }

    // kb
    if(dSize > 1024.0)
    {
        dSize = dSize / 1024.0;
    }
    else
    {
        return QString(strFormat).arg(dSize, 0, 'f', 2).arg("KB");
    }

    // mb
    if(dSize > 1024.0)
    {
        dSize = dSize / 1024.0;
    }
    else
    {
        return QString(strFormat).arg(dSize, 0, 'f', 2).arg("MB");
    }

    // gb
    if(dSize > 1024.0)
    {
        dSize = dSize / 1024.0;
    }
    else
    {
        return QString(strFormat).arg(dSize, 0, 'f', 2).arg("GB");
    }

    // tb
    return QString(strFormat).arg(dSize, 0, 'f', 2).arg("TB");
}

bool G_getFileMd5(const QString& filePath, QByteArray& arrMD5)
{
    QFile localFile(filePath);

    if (!localFile.open(QFile::ReadOnly))
    {
        return false;
    }

    QCryptographicHash ch(QCryptographicHash::Md5);

    quint64 totalBytes = 0;
    quint64 bytesWritten = 0;
    quint64 bytesToWrite = 0;
    quint64 loadSize = 1024 * 4;
    QByteArray buf;

    totalBytes = localFile.size();
    bytesToWrite = totalBytes;

    while (1)
    {
        if(bytesToWrite > 0)
        {
            buf = localFile.read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        }
        else
        {
            break;
        }

        if(bytesWritten == totalBytes)
        {
            break;
        }
    }

    localFile.close();
    arrMD5 = ch.result();
    return true;
}
