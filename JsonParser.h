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

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QString>
#include <QList>
#include <QChar>

////////////////////////////////////////////////////////////////////////
// CJsonSeperator
class CJsonSeperator
{
public:
    CJsonSeperator(QChar cChar, int nPos, int nCharPos);

    QChar getChar() const {return m_cChar;}
    int getPos() const    {return m_nPos;}
    int getCharPos() const {return m_nCharPos;}

private:
    QChar m_cChar;
    int m_nPos;
    int m_nCharPos;
};

////////////////////////////////////////////////////////////////////////
// CSeperatorMatch
class CSeperatorMatch
{
public:
    CSeperatorMatch();
    ~CSeperatorMatch();
    void reset();

    bool isSeperator(QChar cChar);
    bool isMatchedSeperator(QChar cChar);
    bool matchLastSeperator(QChar cChar);
    int getLastSeperatorPos();
    bool addSeperator(QChar cChar, int nPos);
    bool removeLastSeperator();
    bool hasSeperator() const;
    bool lastIsQuote() const;

    QString unwrapSeperators(const QString& strContent);

private:
    QString m_strMatchChars;
    QString m_strMatchChars2;

    QList<CJsonSeperator*> m_lstJsonSeperators;
};

//////////////////////////////////////////////////////////////////////
// class CJsonItem
class CJsonItem
{
public:
    virtual ~CJsonItem();

    void clearContent();

    QString key() const;
    QString value() const;
    void setKey(const QString& strKey);
    void setValue(const QString& strValue);
    void addItem(CJsonItem* pItem);

    CJsonItem* getItem(const QString& strKey, bool bSearchSubChild = true);
    QString getValueByKey(const QString& strKey, bool bSearchSubChild = true) const;
    bool getValueByKey(const QString& strKey, QString& strValue, bool bSearchSubChild = true) const;

    const QList<CJsonItem*>& getChildNodes() const;

private:
    QString m_strKey;
    QString m_strValue;
    QList<CJsonItem*> m_lstValues;
};

//////////////////////////////////////////////////////////////////////
// class CJsonParser
class CJsonParser
{
public:
    CJsonParser();
    virtual ~CJsonParser();

    CJsonItem* getItem(const QString& strKey, bool bSearchSubChild = true);
    QString getValueByKey(const QString& strKey) const;
    void addKeyValue(const QString& strKey, const QString& strValue);
    void setKeyValue(const QString& strKey, const QString& strValue);
    void clearJsonItems();

    const QByteArray& getLeftContent() const {return m_arrLeftContent;}

    QString getLastError() const    {return m_strLastError;}

    bool parse(const QByteArray& arrContent, CJsonItem* pParentJsonItem = NULL);

private:
    CJsonItem m_JsonItemRoot;
    CSeperatorMatch m_seperatorMatch;
    QString m_strLastError;

    QByteArray m_arrLeftContent;
};

#endif // JSONPARSER_H
