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

#include "UnicodeConvertString.h"

CUnicodeConvertString::CUnicodeConvertString(const QString &strData) :
    QString(strData)
{
    // \uXXXX
    int idx = 0;
    while ( ( idx = indexOf("\\u", idx) ) != -1 ) {
        if(idx > 0 && '\\' == at(idx - 1))
        {
            idx += 2;
        }
        else
        {
            int nHex = mid(idx + 2, 4).toInt(0, 16);
            replace(idx++, 6, QChar(nHex));
        }
    }

    // \n
    idx = 0;
    while ( ( idx = indexOf("\\n", idx) ) != -1 ) {
        if(idx > 0 && '\\' == at(idx - 1))
        {
            idx += 2;
        }
        else
        {
            replace(idx++, 2, '\n');
        }
    }

    // \/
    idx = 0;
    while ( ( idx = indexOf("\\/", idx) ) != -1 ) {
        if(idx > 0 && '\\' == at(idx - 1))
        {
            idx += 2;
        }
        else
        {
            replace(idx++, 2, '/');
        }
    }

    // \t
    idx = 0;
    while ( ( idx = indexOf("\\t", idx) ) != -1 ) {
        if(idx > 0 && '\\' == at(idx - 1))
        {
            idx += 2;
        }
        else
        {
            replace(idx++, 2, '\t');
        }
    }

    // \r
    idx = 0;
    while ( ( idx = indexOf("\\r", idx) ) != -1 ) {
        if(idx > 0 && '\\' == at(idx - 1))
        {
            idx += 2;
        }
        else
        {
            remove(idx, 2);
        }
    }

    // \n
    idx = 0;
    while ( ( idx = indexOf("\\n", idx) ) != -1 ) {
        if(idx > 0 && '\\' == at(idx - 1))
        {
            idx += 2;
        }
        else
        {
            replace(idx++, 2, '\n');
        }
    }

    // \"
    idx = 0;
    while ( ( idx = indexOf("\\\"", idx) ) != -1 ) {
        replace(idx++, 2, '"');
    }

    // \\
    idx = 0;
    idx = indexOf("\\\\", 0);
    while ( idx != -1 ) {
        remove(idx++, 1);
        idx = indexOf("\\\\", idx);
    }
}
