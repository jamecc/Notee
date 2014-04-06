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

#include "HtmlHighlighter.h"

CHtmlHighlighter::CHtmlHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document)
{
    QTextCharFormat entityFormat;
    entityFormat.setForeground(QColor(0, 128, 0));
//    entityFormat.setFontWeight(QFont::Bold);
    setFormatFor(Entity, entityFormat);

    QTextCharFormat tagFormat;
    tagFormat.setForeground(QColor(136, 18, 128));
//    tagFormat.setFontWeight(QFont::Bold);
    setFormatFor(Tag, tagFormat);

    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(26, 26, 166));
//    tagFormat.setFontWeight(QFont::Bold);
    setFormatFor(String, stringFormat);

    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(128, 10, 74));
    commentFormat.setFontItalic(true);
    setFormatFor(Comment, commentFormat);
}

void CHtmlHighlighter::setFormatFor(Construct construct,
                            const QTextCharFormat &format)
{
    m_formats[construct] = format;
    rehighlight();
}

void CHtmlHighlighter::highlightBlock(const QString& text)
{
    /*
    QTextCharFormat myClassFormat;
    myClassFormat.setFontWeight(QFont::Bold);
    myClassFormat.setForeground(Qt::darkMagenta);
    QString pattern = "\\bMy[A-Za-z]+\\b";

    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
        int length = expression.matchedLength();
        setFormat(index, length, myClassFormat);
        index = text.indexOf(expression, index + length);
    }
    */

//QTextCharFormat multiLineCommentFormat;
// multiLineCommentFormat.setForeground(Qt::red);

// QRegExp startExpression("/\\*");
// QRegExp endExpression("\\*/");

/*
 setCurrentBlockState(0);

 int startIndex = 0;
 if (previousBlockState() != 1)
     startIndex = text.indexOf(startExpression);

 while (startIndex >= 0) {
    int endIndex = text.indexOf(endExpression, startIndex);
    int commentLength;
    if (endIndex == -1) {
        setCurrentBlockState(1);
        commentLength = text.length() - startIndex;
    } else {
        commentLength = endIndex - startIndex
                        + endExpression.matchedLength();
    }
    setFormat(startIndex, commentLength, multiLineCommentFormat);
    startIndex = text.indexOf(startExpression,
                              startIndex + commentLength);
 }
    */
    int state = previousBlockState();
    int len = text.length();
    int start = 0;
    int pos = 0;
    int nQuotePos = -1;

    while (pos < len) {
        switch (state) {
        case NormalState:
        default:
          while (pos < len) {
              QChar ch = text.at(pos);
              if (ch == '<') {
                  if (text.mid(pos, 4) == "<!--") {
                      state = InComment;
                  } else {
                      state = InTag;
                  }
                  break;
              } else if (ch == '&') {
                  start = pos;
                  while (pos < len
                         && text.at(pos++) != ';')
                      ;
                  setFormat(start, pos - start,
                            m_formats[Entity]);
              } else {
                  ++pos;
              }
          }
          break;

        case InComment:
            start = pos;
            while (pos < len) {
                if (text.mid(pos, 3) == "-->") {
                    pos += 3;
                    state = NormalState;
                    break;
                } else {
                    ++pos;
                }
            }
            setFormat(start, pos - start,
                      m_formats[Comment]);
            break;

        case InTag:
            QChar quote = QChar::Null;
            start = pos;
            while (pos < len) {
                QChar ch = text.at(pos);
                if (quote.isNull()) {
                    if (ch == '\"' || ch == '"') {
                        quote = ch;
                        nQuotePos = pos+1;
                        setFormat(start, nQuotePos - start, m_formats[Tag]);
                    } else if (ch == '>') {
                        ++pos;
                        state = NormalState;
                        break;
                    }
                } else if (ch == quote) {
                    quote = QChar::Null;
                    setFormat(nQuotePos, pos - nQuotePos, m_formats[String]);
                    nQuotePos = -1;
                    start = pos;
                }
                ++pos;
            }
            if(nQuotePos >= 0)
                setFormat(start, pos - nQuotePos, m_formats[String]);
            else
                setFormat(start, pos - start, m_formats[Tag]);
            break;
        }
    }

    setCurrentBlockState(state);
}
