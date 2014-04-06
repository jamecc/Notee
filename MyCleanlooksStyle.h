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

#ifndef MYCLEANLOOKSSTYLE_H
#define MYCLEANLOOKSSTYLE_H

#include <QCleanlooksStyle>

class CMyCleanlooksStyle : public QCleanlooksStyle
{
    Q_OBJECT
public:
    explicit CMyCleanlooksStyle();
    
signals:
    
public slots:

protected:
    virtual void polish(QPalette &pal);
//    virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
//                                    QPainter *painter, const QWidget *widget) const;
    
};

#endif // MYCLEANLOOKSSTYLE_H
