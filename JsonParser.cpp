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
#include "JsonParser.h"

////////////////////////////////////////////////////////////////////////
// CJsonSeperator
CJsonSeperator::CJsonSeperator(QChar cChar, int nPos, int nCharPos)
    : m_cChar(cChar)
    , m_nPos(nPos)
    , m_nCharPos(nCharPos)
{
}


////////////////////////////////////////////////////////////////////////////
// CSeperatorMatch
CSeperatorMatch::CSeperatorMatch()
{
    m_strMatchChars  = "\"'[{(<";
    m_strMatchChars2 = "\"']})>";
}

CSeperatorMatch::~CSeperatorMatch()
{
    reset();
}

void CSeperatorMatch::reset()
{
    for(int i = m_lstJsonSeperators.length() - 1; i >= 0; i--)
    {
        delete m_lstJsonSeperators.at(i);
    }

    m_lstJsonSeperators.clear();
}

bool CSeperatorMatch::isSeperator(QChar cChar)
{
    return m_strMatchChars.indexOf(cChar) >= 0;
}

bool CSeperatorMatch::isMatchedSeperator(QChar cChar)
{
    return m_strMatchChars2.indexOf(cChar) >= 0;
}

bool CSeperatorMatch::matchLastSeperator(QChar cChar)
{
    if(!m_lstJsonSeperators.length())
        return false;

    CJsonSeperator* pLastOne = m_lstJsonSeperators.at(m_lstJsonSeperators.length() - 1);
    return m_strMatchChars2.at(pLastOne->getCharPos()) == cChar;
}

int CSeperatorMatch::getLastSeperatorPos()
{
    if(!m_lstJsonSeperators.length())
        return -1;

    CJsonSeperator* pLastOne = m_lstJsonSeperators.at(m_lstJsonSeperators.length() - 1);
    return pLastOne->getPos();
}

bool CSeperatorMatch::addSeperator(QChar cChar, int nPos)
{
    int nCharPos = m_strMatchChars.indexOf(cChar);
    if(nCharPos < 0)
        return false;

    m_lstJsonSeperators.push_back(new CJsonSeperator(cChar, nPos, nCharPos));
    return true;
}

bool CSeperatorMatch::removeLastSeperator()
{
    if(!m_lstJsonSeperators.length())
        return false;

    CJsonSeperator* pLastOne = m_lstJsonSeperators.at(m_lstJsonSeperators.length() - 1);
    m_lstJsonSeperators.pop_back();
    delete pLastOne;
    return true;
}

bool CSeperatorMatch::hasSeperator() const
{
    return !!m_lstJsonSeperators.length();
}

bool CSeperatorMatch::lastIsQuote() const
{
    return m_lstJsonSeperators.length() &&
            m_lstJsonSeperators.at(m_lstJsonSeperators.length() - 1)->getChar() == '\"';
}

QString CSeperatorMatch::unwrapSeperators(const QString& strContent)
{
    QChar cCharFrom, cCharEnd;
    int iFrom = 0, iEnd = strContent.length() - 1;
    int iFromCharPos, iEndCharPos;
    while(iEnd > iFrom)
    {
        cCharFrom = strContent.at(iFrom);
        if(cCharFrom == ' ' || cCharFrom == '\t')
        {
            iFrom++;
            continue;
        }
        cCharEnd = strContent.at(iEnd);
        if(cCharEnd == ' ' || cCharEnd == '\t')
        {
            iEnd--;
            continue;
        }

        iFromCharPos = m_strMatchChars.indexOf(cCharFrom);
        if(iFromCharPos < 0)
            break;

        iEndCharPos = m_strMatchChars.indexOf(iEnd);
        if(iEndCharPos < 0)
            break;

        if(iFromCharPos == iEndCharPos)
        {
            iFrom++;
            iEnd--;
            continue;
        }


        break;
    }

    return strContent.mid(iFrom, iEnd - iFrom + 1);
}

////////////////////////////////////////////////////////////////////////
// CJsonItem
CJsonItem::~CJsonItem()
{
    clearContent();
}

void CJsonItem::clearContent()
{
    setKey("");
    setValue("");

    foreach(CJsonItem* pItem, m_lstValues)
    {
        pItem->clearContent();
        delete pItem;
    }
    m_lstValues.clear();
}

QString CJsonItem::key() const
{
    return m_strKey;
}

QString CJsonItem::value() const
{
    return m_strValue;
}

void CJsonItem::setKey(const QString& strKey)
{
    m_strKey = strKey;
}

void CJsonItem::setValue(const QString& strValue)
{
    m_strValue = strValue;
}

void CJsonItem::addItem(CJsonItem* pItem)
{
    m_lstValues.push_back(pItem);
}

CJsonItem* CJsonItem::getItem(const QString& strKey, bool bSearchSubChild)
{
    if(key() == strKey)
    {
        return this;
    }

    foreach(CJsonItem* pItem, m_lstValues)
    {
        if(pItem->key() == strKey)
        {
            return pItem;
        }

        if(bSearchSubChild)
        {
            CJsonItem* pSubItem = pItem->getItem(strKey, bSearchSubChild);
            if(pSubItem)
                return pSubItem;
        }
    }

    return NULL;
}

QString CJsonItem::getValueByKey(const QString& strKey, bool bSearchSubChild) const
{
    QString strValue;
    getValueByKey(strKey, strValue);

    return strValue;
}

bool CJsonItem::getValueByKey(const QString& strKey, QString& strValue, bool bSearchSubChild) const
{
    if(key() == strKey)
    {
        strValue = value();
        return true;
    }

    foreach(CJsonItem* pItem, m_lstValues)
    {
        if(pItem->key() == strKey)
        {
            strValue = pItem->value();
            return true;
        }

        if(bSearchSubChild)
        {
            if(pItem->getValueByKey(strKey, strValue))
                return true;
        }
    }

    return false;
}

const QList<CJsonItem*>& CJsonItem::getChildNodes() const
{
    return m_lstValues;
}

////////////////////////////////////////////////////////////////////////
// CJsonParser
CJsonParser::CJsonParser()
{
}

CJsonParser::~CJsonParser()
{

}

CJsonItem* CJsonParser::getItem(const QString& strKey, bool bSearchSubChild)
{
    return m_JsonItemRoot.getItem(strKey, bSearchSubChild);
}

QString CJsonParser::getValueByKey(const QString& strKey) const
{
    return m_JsonItemRoot.getValueByKey(strKey);
}

void CJsonParser::setKeyValue(const QString& strKey, const QString& strValue)
{
    m_JsonItemRoot.setKey(strKey);
    m_JsonItemRoot.setValue(strValue);
}

void CJsonParser::addKeyValue(const QString& strKey, const QString& strValue)
{
    CJsonItem* pItem = new CJsonItem;
    pItem->setKey(strKey);
    pItem->setValue(strValue);
    m_JsonItemRoot.addItem(pItem);
}

void CJsonParser::clearJsonItems()
{
    m_JsonItemRoot.clearContent();
    m_strLastError = "";
}

// {"result":0,"user_id":"3","user_name":"aaaaaa","login_token":"r5f68lrl",
//      "others":[
//                  {"a":"1","b":2,"c":3},
//                  {"a":"1","b":2,"c":3},
//                  {"a":"1","b":2,"c":3}
//            ],
//      "memo":"memo 2"
// }
#include <QFile>

bool CJsonParser::parse(const QByteArray& arrContent, CJsonItem* pParentJsonItem)
{
    QString strContent = arrContent;

    m_strLastError = "";
    bool bIsRootItem = !pParentJsonItem;

#ifdef NOTEE_DEBUG
    QFile output("C:\\json_parser.txt");
    output.open(QIODevice::WriteOnly | QIODevice::Text);
    output.write(arrContent);
    output.flush();
#endif

    if(!pParentJsonItem)
    {
        pParentJsonItem = &m_JsonItemRoot;
        m_arrLeftContent.clear();
        m_seperatorMatch.reset();
        m_JsonItemRoot.clearContent();
    }

    int nPrevCommaPos = -1;
    bool bHasCommaInValue= false;
    QChar cCurrentChar = 0;
    CJsonItem* pNewCurrentJsonItem = NULL;
    QString strSubContent;

    bool bIsKey = true;
    for(int i = 0; i < strContent.length(); ++i)
    {
        cCurrentChar = strContent.at(i);

        while(cCurrentChar == '\\' && i < strContent.length())
        {
            i += 2;
            if(i < strContent.length())
                cCurrentChar = strContent.at(i);
            else
            {
                i = strContent.length() - 1;
                cCurrentChar = 0;
            }
        }

        if(m_seperatorMatch.hasSeperator() && cCurrentChar > 0)
        {
            // matched
            if(m_seperatorMatch.matchLastSeperator(cCurrentChar))
            {
                int nLastSeperatorPos = m_seperatorMatch.getLastSeperatorPos();
                m_seperatorMatch.removeLastSeperator();
                // the lastest seperator
                if(!m_seperatorMatch.hasSeperator())
                {
                    strSubContent = strContent.mid(nLastSeperatorPos + 1, i - nLastSeperatorPos -1);
                    if(bIsKey && !bHasCommaInValue)
                    {
                        pNewCurrentJsonItem = new CJsonItem;
                        pNewCurrentJsonItem->setKey(strSubContent);
                        pParentJsonItem->addItem(pNewCurrentJsonItem);
                    }
                    else
                    {
                        // there's ',' in
                        if(bHasCommaInValue)
                        {
                            if(bIsKey && !bIsRootItem)
                            {
                                pNewCurrentJsonItem = new CJsonItem;
                                pParentJsonItem->addItem(pNewCurrentJsonItem);
                            }

                            parse(strSubContent.toAscii(), pNewCurrentJsonItem);
                            bHasCommaInValue = false;
                        }
                        else
                        {
                            if(!pNewCurrentJsonItem)
                            {
                                m_strLastError = "Parse error: \"" + strContent + "\"";
                                return false;
                            }

                            CUnicodeConvertString str2(strSubContent);
                        //    QString str = QString::fromUtf8(strSubContent.toAscii().data(), strSubContent.toAscii().size());
                       //     QString str = QString::fromUtf8(str2.toAscii());
                            pNewCurrentJsonItem->setValue(str2);
                        }
                    }

                    bHasCommaInValue = false;
                    nPrevCommaPos = i;

                    // the end of the first quote
                    if(bIsRootItem && !pNewCurrentJsonItem)
                    {
                        CUnicodeConvertString str(strContent.mid(i + 1));
                        m_arrLeftContent = str.toAscii();
                        return true;
                    }
                }
            }
            // isn't matched
            else
            {
                // isn't in ""
                if(!m_seperatorMatch.lastIsQuote())
                {
                    if(cCurrentChar == ',' || cCurrentChar == ':')
                        bHasCommaInValue = true;

                    if(m_seperatorMatch.isSeperator(cCurrentChar))
                    {
                        m_seperatorMatch.addSeperator(cCurrentChar, i);
                    }
                }
            }

            continue;
        }

        if(m_seperatorMatch.isSeperator(cCurrentChar))
        {
            m_seperatorMatch.addSeperator(cCurrentChar, i);
            continue;
        }

        // process key and value
        if(cCurrentChar == ':' ||
                cCurrentChar == ',' ||
                cCurrentChar == 0 ||
                i == (strContent.length()-1))
        {
            if(cCurrentChar == ':' || cCurrentChar == ',')
            {
                strSubContent = strContent.mid(nPrevCommaPos + 1, i - nPrevCommaPos -1);
            }
            else
            {
                strSubContent = strContent.mid(nPrevCommaPos + 1, i - nPrevCommaPos);
            }

            // key
            if(bIsKey)
            {
                if(!strSubContent.isEmpty())
                {
                    pNewCurrentJsonItem = new CJsonItem;
                    pNewCurrentJsonItem->setKey(strSubContent);

                    pParentJsonItem->addItem(pNewCurrentJsonItem);
                }

                nPrevCommaPos = i;
                if(cCurrentChar == ':')
                {
                    bIsKey = false;
                }
            }
            // values
            else
            {
                if(cCurrentChar == ':')
                {
                    m_strLastError = "Unmatched seperator";
                    return false;
                }

                if(!strSubContent.isEmpty())
                {
                    if(bHasCommaInValue)
                    {
                       parse(strSubContent.toAscii(), pNewCurrentJsonItem);

                       bHasCommaInValue = false;
                    }
                    else
                    {
                        CUnicodeConvertString str2(strSubContent);
                     //   QString str = QString::fromUtf8(str2.toAscii());
                        pNewCurrentJsonItem->setValue(str2);
                    }
                }

                nPrevCommaPos = i;
                bIsKey = true;
            }
        }
    }

    if(m_seperatorMatch.hasSeperator())
    {
        m_strLastError = "Unmatched seperator";
        return false;
    }

    return true;
}
