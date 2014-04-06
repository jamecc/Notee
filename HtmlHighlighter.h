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

#ifndef HTMLHIGHLIGHTER_H
#define HTMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class CHtmlHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    enum Construct {
        Entity,
        Tag,
        Comment,
        String,
        LastConstruct = String
    };

    CHtmlHighlighter(QTextDocument *document);

    void setFormatFor(Construct construct,
                      const QTextCharFormat &format);
    QTextCharFormat formatFor(Construct construct) const
        { return m_formats[construct]; }
    
signals:
    
public slots:

protected:
    enum State {
        NormalState = 0,
        InComment,
        InTag,
        InString
    };

    virtual void highlightBlock(const QString& text);

private:
    QTextCharFormat m_formats[LastConstruct + 1];
    
};

#endif // HTMLHIGHLIGHTER_H
