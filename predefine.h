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

#ifndef NOTE_PREDEFINE_H
#define NOTE_PREDEFINE_H

#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))

#define RET_OK   0

#define DATE_FORMAT "yyyy-MM-dd"

#define DATE_TIME_FORMAT "yyyy-MM-dd hh:mm:ss"

#define DATE_TIME_FORMAT_NZERO "yyyy-M-d h:m:s"

#define DATE_TIME_FORMAT_STRING "yyyyMdhms"

// Color
#define COLOR_BKGND_SCENE               245,239,207
#define CLR_BKGND_HEADER_MONTH          58,88,129
#define CLR_BKGND_HEADER_WEEK           112,146,190
#define CLR_BKGND_HEADER_WEEK_END       150,200,240
#define CLR_BKGND_DAY                   255,255,255
#define CLR_BKGND_WEEK_END              240,240,240

#define CLR_BKGND_HOVER                 181,230,29,120
#define CLR_BKGND_PRESS                 156,201,22,120
#define CLR_BKGND_SELECTED              203,238,98,120

#define CLR_BKGND_HOVER_DAY             10
#define CLR_BKGND_PRESS_DAY             10
#define CLR_BKGND_SELECTED_DAY          10
#define CLR_BKGND_HOVER_NOTE_ITEM       255,160,100
#define CLR_BKGND_PRESS_NOTE_ITEM       206,83,0
#define CLR_BKGND_SELECTED_NOTE_ITEM    255,127,39

#define CLR_FONT_MONTH                  255,255,255  // white
#define CLR_FONT_WEEK                   255,255,255
#define CLR_FONT_DAY                    200,200,200
#define CLR_FONT_DAY_CURRENT_DAY        255,117,26
#define CLR_FONT_DAY_CURRENT_MONTH      0,0,0        // black
#define CLR_FONT_NOTE_TEXT              120,120,120

#define STR_VERSION                     "2.2"
#define DB_VERSION                      "1.2"

#define MB_SIZE                         1024*1024
#define MAX_ATTACHMENT_SIZE             500 * MB_SIZE
#define MAX_NOTE_CONTENT_SIZE           500 * MB_SIZE
#define MAX_NOTE_IMAGE_SIZE             100 * MB_SIZE

#if defined(Q_WS_MAC)
#define LOGIN_FROM "MAC_APP_2_2"
#elif defined (Q_WS_WIN)
#define LOGIN_FROM "WIN_APP_2_2"
#else
#define LOGIN_FROM "X11_APP_2_2"
#endif

#endif // #ifndef NOTE_PREDEFINE_H
