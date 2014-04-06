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

#include <QBuffer>
#include <QMimeData>
#include <QHttp>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QImageReader>
#include "predefine.h"
#include "ImageTextEdit.h"

struct IMG_POS
{
    int nStart;
    int nEnd;
    QString strUrl;
    QString strImageLink;
};

void G_parseImage(const QString& strHTML, QList<IMG_POS>& lstImgPos)
{
    static int i = 0;
    QString strTime = QDateTime::currentDateTime().toString(DATE_TIME_FORMAT_STRING);

    // search "<img", " src="
    int nSearchStart = 0, nStart = 0, nEnd = 0, nSrcPos = 0;
    QRegExp expImg("<[ \t]*img[ \t]*");
    QRegExp expSrc("[ \t]*src=");
    while((nStart = strHTML.indexOf(expImg, nSearchStart)) >= 0)
    {
        nSearchStart = nStart + 1;
        nEnd = strHTML.indexOf(">", nStart);
        if(nEnd < 0)
            continue;

        nSearchStart = nEnd + 1;

        nSrcPos = strHTML.indexOf(expSrc, nStart);
        if(nSrcPos < 0)
            continue;

        int nLeftQuote = strHTML.indexOf("\"", nSrcPos);
        int nRightQuote = strHTML.indexOf("\"", nLeftQuote + 1);

        IMG_POS imgPos;
        imgPos.nStart = nStart;
        imgPos.nEnd = nEnd;
        imgPos.strUrl = strHTML.mid(nLeftQuote + 1, nRightQuote - nLeftQuote - 1).trimmed();
        imgPos.strImageLink = QString("<img src=\"img_%1_%2\" />").arg(strTime).arg(i++);

        if(imgPos.strUrl.isEmpty())
            continue;

        lstImgPos.push_back(imgPos);
    }
}

QString G_replaceImage(const QString& strHTML, const QList<IMG_POS>& lstImgPos)
{
    if(lstImgPos.isEmpty())
        return strHTML;

    int nLastEnd = 0;
    QString strText;
    foreach(IMG_POS imgPos, lstImgPos)
    {
  //      QImage image(imgPos.strUrl);

 //       QByteArray byteArray;
 //       QBuffer buffer(&byteArray);
 //       image.save(&buffer, "PNG");
 //       QString url = QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/>";

        strText += strHTML.mid(nLastEnd, imgPos.nStart - nLastEnd);
        strText += imgPos.strImageLink;

        nLastEnd = imgPos.nEnd + 1;
    }

    strText += strHTML.mid(nLastEnd);

    strText.replace(QRegExp("<!--StartFragment-->"), "");
    strText.replace(QRegExp("<!--StartFragment-->"), "");
    return strText;
}

////////////////////////////////////////////////////////////////////////////////////////
// class CImageTextEdit
CImageTextEdit::CImageTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

CImageTextEdit::~CImageTextEdit()
{
    clearHttpImages();
}

void CImageTextEdit::clearHttpImages()
{
    foreach(HTTP_IMAGE* pHttpImage, m_lstHttpImages)
    {
        delete pHttpImage, pHttpImage = NULL;
    }

    m_lstHttpImages.clear();
}

bool CImageTextEdit::canInsertFromMimeData(const QMimeData* source) const
{
    if(source->hasImage() || QTextEdit::canInsertFromMimeData(source) || source->hasUrls())
        return true;

    return false;
}

void CImageTextEdit::insertFromMimeData(const QMimeData* source)
{
    if (source->hasImage())
    {
        dropImage(qvariant_cast<QImage>(source->imageData()));
    }
    else if(source->hasHtml())
    {
        /*
        QList<IMG_POS> lstImgPos;
        QString strHTML = source->html();
        G_parseImage(strHTML, lstImgPos);
        strHTML = G_replaceImage(strHTML, lstImgPos);
        textCursor().insertHtml(strHTML);

        foreach(IMG_POS imgPos, lstImgPos)
        {
            requestHttp(QUrl(imgPos.strUrl), imgPos.strImageLink);
        }
        */
        textCursor().insertHtml(source->html());
    }
    else if(canInsertFromMimeData(source))
    {
        QTextEdit::insertFromMimeData(source);
    }
    else if (source->hasUrls())
    {
        foreach (QUrl url, source->urls())
        {
            textCursor().insertHtml(QString("<a href='%1'>%1</a>").arg(url.path()));
        }
    }
}

void CImageTextEdit::dropImage(const QImage& image)
{
    if (!image.isNull())
    {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        image.save(&buffer, "PNG");
        QString url = QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\" />";
        textCursor().insertHtml(url);
    }
}

void CImageTextEdit::dropTextFile(const QUrl& url)
{
    QFile file(url.toLocalFile());
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        textCursor().insertText(file.readAll());
}

void CImageTextEdit::requestHttp(const QUrl& url, const QString& strImgLink)
{
    // new http image
    HTTP_IMAGE* pHttpImage = new HTTP_IMAGE();
    pHttpImage->strImgLink = strImgLink;

    pHttpImage->pBuffer = new QBuffer(&pHttpImage->arrBytes, this);
    pHttpImage->pBuffer->open(QIODevice::WriteOnly);

    pHttpImage->pHttp = new QHttp(this);
    pHttpImage->pHttp->setHost(url.host());
    connect(pHttpImage->pHttp, SIGNAL(requestFinished(int, bool)),this, SLOT(onHttpFinished(int, bool)));

    m_lstHttpImages.push_back(pHttpImage);

    // get
    pHttpImage->nRequestId = pHttpImage->pHttp->get(url.path(), pHttpImage->pBuffer);
}

void CImageTextEdit::onHttpFinished(int nRequestId, bool)
{
    foreach(HTTP_IMAGE* pHttpImage, m_lstHttpImages)
    {
        if(pHttpImage->nRequestId == nRequestId)
        {
            // load
            QImage image;
            image.loadFromData(pHttpImage->arrBytes);

            // replace
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            image.save(&buffer, "PNG");
            QString url = QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/>";

            QTextCursor currentTextCursor = textCursor();

            setHtml(toHtml().replace(pHttpImage->strImgLink,url));

            setTextCursor(currentTextCursor);

            // remove
            m_lstHttpImages.removeOne(pHttpImage);
            delete pHttpImage, pHttpImage = NULL;
            break;
        }
    }
}
