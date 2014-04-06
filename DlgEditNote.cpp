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
#include <QSpinBox>
#include "NoteItem.h"
#include "AttachmentListWidget.h"
#include "ImageTextEdit.h"
#include "DlgEditNote.h"
#include "MainFrame.h"
#include "HtmlHighlighter.h"


CDlgEditNote::CDlgEditNote(QWidget *parent)
    : QWidget(parent)
    , m_bModified(false)
{
    m_bCursorPosChanging = false;
    m_pNoteItem = NULL;
    createControls();
}

CDlgEditNote::CDlgEditNote(CNoteItem* pNoteItem, const QDateTime& dateTime, QWidget *parent)
        : QWidget(parent)
	, m_bModified(false)
{
    m_pNoteItem = NULL;
    m_dtInit = dateTime;
    createControls();
    setNoteItem(pNoteItem);
}

CDlgEditNote::~CDlgEditNote()
{
    endDialog();
}

void CDlgEditNote::endDialog()
{
    m_pNoteItemOrg = NULL;
    m_bModified = false;
    if(m_pNoteItem)
        delete m_pNoteItem, m_pNoteItem = NULL;
}

void CDlgEditNote::convertToNewNote()
{
    m_pNoteItemOrg = NULL;
    m_bModified = true;
    m_pNoteItem->setState(STATE_NEW);
    setWindowTitle(tr("New Note"));
}

void CDlgEditNote::setNoteItem(CNoteItem* pNoteItem)
{
    m_pNoteItemOrg = pNoteItem;

    m_pNoteItem = new CNoteItem;

    if(m_pNoteItemOrg)
    {
        *m_pNoteItem = *m_pNoteItemOrg;
        m_pNoteItem->setContent(m_pNoteItemOrg->getContent());
        m_pNoteItem->setState(STATE_MODIFY);
        setWindowTitle(tr("Edit Note"));
    } else {
        m_pNoteItem->setState(STATE_NEW);
        setWindowTitle(tr("New Note"));
    }

    resize(QSize(600, 400));
    initControls();
}

void CDlgEditNote::createControls()
{
    QVBoxLayout* pMainLayout = new QVBoxLayout(this);
//    setLayout(pMainLayout);

    pMainLayout->setContentsMargins(5, 0, 5, 5);

    // toolbar
    m_pMainToolbar = new QToolBar(this);
    pMainLayout->addWidget(m_pMainToolbar);

    // undo
    m_pActionUndo = new QAction(this);
    m_pMainToolbar->addAction(m_pActionUndo);
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/res/undo.png"));
    m_pActionUndo->setIcon(icon1);
    m_pActionUndo->setToolTip(tr("Undo"));
    m_pActionUndo->setStatusTip(tr("Undo"));
    m_pActionUndo->setEnabled(false);
    connect(m_pActionUndo, SIGNAL(triggered()), this, SLOT(onUndo()));

    // redo
    m_pActionRedo = new QAction(this);
    m_pMainToolbar->addAction(m_pActionRedo);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/res/redo.png"));
    m_pActionRedo->setIcon(icon2);
    m_pActionRedo->setToolTip(tr("Redo"));
    m_pActionRedo->setStatusTip(tr("Redo"));
    m_pActionRedo->setEnabled(false);
    connect(m_pActionRedo, SIGNAL(triggered()), this, SLOT(onRedo()));

    m_pMainToolbar->addSeparator();

    // cut
    m_pActionCut = new QAction(this);
    m_pMainToolbar->addAction(m_pActionCut);
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/res/cut.png"));
    m_pActionCut->setIcon(icon3);
    m_pActionCut->setToolTip(tr("Cut"));
    m_pActionCut->setStatusTip(tr("Cut"));
    connect(m_pActionCut, SIGNAL(triggered()), this, SLOT(onCut()));

    // copy
    m_pActionCopy = new QAction(this);
    m_pMainToolbar->addAction(m_pActionCopy);
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/res/copy.png"));
    m_pActionCopy->setIcon(icon4);
    m_pActionCopy->setToolTip(tr("Copy"));
    m_pActionCopy->setStatusTip(tr("Copy"));
    connect(m_pActionCopy, SIGNAL(triggered()), this, SLOT(onCopy()));

    // paste
    m_pActionPaste = new QAction(this);
    m_pMainToolbar->addAction(m_pActionPaste);
    QIcon icon5;
    icon5.addFile(QString::fromUtf8(":/res/paste.png"));
    m_pActionPaste->setIcon(icon5);
    m_pActionPaste->setToolTip(tr("Paste"));
    m_pActionPaste->setStatusTip(tr("Paste"));
    connect(m_pActionPaste, SIGNAL(triggered()), this, SLOT(onPaste()));

    m_pMainToolbar->addSeparator();

    // add image
    m_pActionAddImg = new QAction(this);
    m_pMainToolbar->addAction(m_pActionAddImg);
    icon5 = QIcon();
    icon5.addFile(QString::fromUtf8(":/res/add_img.png"));
    m_pActionAddImg->setIcon(icon5);
    m_pActionAddImg->setToolTip(tr("Add image"));
    m_pActionAddImg->setStatusTip(tr("Add image"));
    connect(m_pActionAddImg, SIGNAL(triggered()), this, SLOT(onAddImage()));

    // add attachment
    m_pActionAddAttachment = new QAction(this);
    m_pMainToolbar->addAction(m_pActionAddAttachment);
    QIcon icon6;
    icon6.addFile(QString::fromUtf8(":/res/attachment.png"));
    m_pActionAddAttachment->setIcon(icon6);
    m_pActionAddAttachment->setToolTip(tr("Add Attachment"));
    m_pActionAddAttachment->setStatusTip(tr("Add Attachment"));
    connect(m_pActionAddAttachment, SIGNAL(triggered()), this, SLOT(onAddAttachment()));

    // del attachment
    m_pActionDelAttachment = new QAction(this);
    m_pMainToolbar->addAction(m_pActionDelAttachment);
    QIcon icon7;
    icon7.addFile(QString::fromUtf8(":/res/attachment_del.png"));
    m_pActionDelAttachment->setIcon(icon7);
    m_pActionDelAttachment->setToolTip(tr("Delete Attachment"));
    m_pActionDelAttachment->setStatusTip(tr("Delete Attachment"));
    m_pActionDelAttachment->setEnabled(false);
    connect(m_pActionDelAttachment, SIGNAL(triggered()), this, SLOT(onDelAttachment()));

    // save attachment
    m_pActionSaveAttachment = new QAction(this);
    m_pMainToolbar->addAction(m_pActionSaveAttachment);
    QIcon icon30;
    icon30.addFile(QString::fromUtf8(":/res/attachment_save.png"));
    m_pActionSaveAttachment->setIcon(icon30);
    m_pActionSaveAttachment->setToolTip(tr("Delete Attachment"));
    m_pActionSaveAttachment->setStatusTip(tr("Delete Attachment"));
    m_pActionSaveAttachment->setEnabled(false);
    connect(m_pActionSaveAttachment, SIGNAL(triggered()), this, SLOT(onSaveAttachment()));

    // text format
    m_pMainToolbar->addSeparator();
    m_pActionTextColor = new QAction(this);
    m_pMainToolbar->addAction(m_pActionTextColor);
    QIcon icon8;
    icon8.addFile(QString::fromUtf8(":/res/colors.png"));
    m_pActionTextColor->setIcon(icon8);
    m_pActionTextColor->setToolTip(tr("Text color"));
    m_pActionTextColor->setStatusTip(tr("Text color"));
    connect(m_pActionTextColor, SIGNAL(triggered()), this, SLOT(onTextColor()));

    m_pActionTextFont = new QAction(this);
    m_pMainToolbar->addAction(m_pActionTextFont);
    QIcon icon9;
    icon9.addFile(QString::fromUtf8(":/res/font.png"));
    m_pActionTextFont->setIcon(icon9);
    m_pActionTextFont->setToolTip(tr("Text font"));
    m_pActionTextFont->setStatusTip(tr("Text font"));
    connect(m_pActionTextFont, SIGNAL(triggered()), this, SLOT(onTextFont()));

    m_pSpinBoxTextSize = new QSpinBox(this);
    m_pMainToolbar->addWidget(m_pSpinBoxTextSize);
    m_pSpinBoxTextSize->setToolTip(tr("Text size"));
    m_pSpinBoxTextSize->setStatusTip(tr("Text size"));
    m_pSpinBoxTextSize->setRange(5,120);
    m_pSpinBoxTextSize->setValue(12);
    connect(m_pSpinBoxTextSize, SIGNAL(valueChanged(int)), this, SLOT(onTextSize(int)));

    // html or plain text
    m_pMainToolbar->addSeparator();
    m_pActionHtml = new QAction(this);
    m_pMainToolbar->addAction(m_pActionHtml);
    QIcon icon10;
    icon10.addFile(QString::fromUtf8(":/res/html.png"));
    m_pActionHtml->setIcon(icon10);
    m_pActionHtml->setToolTip(tr("Plain or HTML text"));
    m_pActionHtml->setStatusTip(tr("Plain or HTML text"));
    m_pActionHtml->setCheckable(true);
    m_pActionHtml->setChecked(!G_Settings()->isEditNoteHtml());
    connect(m_pActionHtml, SIGNAL(triggered()), this, SLOT(onHtml()));

    // import and export
    m_pMainToolbar->addSeparator();
    m_pActionImport = new QAction(this);
    m_pMainToolbar->addAction(m_pActionImport);
    QIcon icon11;
    icon11.addFile(QString::fromUtf8(":/res/import.png"));
    m_pActionImport->setIcon(icon11);
    m_pActionImport->setToolTip(tr("Import note content"));
    m_pActionImport->setStatusTip(tr("Import note content"));
    connect(m_pActionImport, SIGNAL(triggered()), this, SLOT(onNoteImport()));

    m_pActionExport = new QAction(this);
    m_pMainToolbar->addAction(m_pActionExport);
    QIcon icon12;
    icon12.addFile(QString::fromUtf8(":/res/export.png"));
    m_pActionExport->setIcon(icon12);
    m_pActionExport->setToolTip(tr("Export note content"));
    m_pActionExport->setStatusTip(tr("Export note content"));
    connect(m_pActionExport, SIGNAL(triggered()), this, SLOT(onNoteExport()));

    // top layout
    QGridLayout* pTopLayout = new QGridLayout(this);
    pMainLayout->addLayout(pTopLayout);
    pTopLayout->setColumnStretch(2, 1);

    // title
    QLabel* pLabelTitle = new QLabel(this);
    pLabelTitle->setText(tr("Title:"));
    m_pEditTitle = new QLineEdit;
    pLabelTitle->setBuddy(m_pEditTitle);
    pTopLayout->addWidget(pLabelTitle, 0, 0);
    pTopLayout->addWidget(m_pEditTitle, 0, 1, 1, 2);
    connect(m_pEditTitle, SIGNAL(returnPressed()), this, SLOT(onOk()));
    connect(m_pEditTitle, SIGNAL(textChanged(QString)), SLOT(onDataChanged()));

    // share type
    QLabel* pLabelShareType = new QLabel(tr("Share type:"), this);
    m_pRadioPrivate = new QRadioButton(tr("Private"));
    m_pRadioPrivate->setToolTip(tr("private note"));
    m_pRadioPrivate->setStatusTip(m_pRadioPrivate->toolTip());
    m_pRadioPublic = new QRadioButton(tr("Public"));
    m_pRadioPublic->setToolTip(tr("share note to public on \"Note Space\""));
    m_pRadioPublic->setStatusTip(m_pRadioPublic->toolTip());
    m_pRadioPrivate->setChecked(true);
    QButtonGroup* pBtnGroup1 = new QButtonGroup(this);
    pBtnGroup1->addButton(m_pRadioPrivate);
    pBtnGroup1->addButton(m_pRadioPublic);
    pTopLayout->addWidget(pLabelShareType, 1, 0);
    pTopLayout->addWidget(m_pRadioPrivate, 1, 1);
    pTopLayout->addWidget(m_pRadioPublic, 1, 2);
    connect(m_pRadioPrivate, SIGNAL(clicked()), this, SLOT(onClickPrivate()));
    connect(m_pRadioPublic, SIGNAL(clicked()), this, SLOT(onClickPublic()));

    // date and time
    QLabel* pLabelDateTime = new QLabel(this);
    pLabelDateTime->setText(tr("Time:"));
    m_pEditDate = new QDateEdit;
    m_pEditDate->setDisplayFormat("yyyy-M-d");
    m_pEditDate->setDate(m_dtInit.date());
    m_pEditTime = new QTimeEdit;
    m_pEditTime->setDisplayFormat("h:m");
    m_pEditTime->setTime(m_dtInit.time());
    pLabelDateTime->setBuddy(m_pEditDate);
    pTopLayout->addWidget(pLabelDateTime, 2, 0);
    pTopLayout->addWidget(m_pEditDate, 2, 1);
    pTopLayout->addWidget(m_pEditTime, 2, 2);
    connect(m_pEditDate, SIGNAL(dateChanged(QDate)), this, SLOT(onDataChanged()));
    connect(m_pEditTime, SIGNAL(timeChanged(QTime)), this, SLOT(onDataChanged()));

    // type
    QLabel* pLabelType = new QLabel(this);
    pLabelType->setText(tr("Type:"));
    m_pRadioNote = new QRadioButton;
    m_pRadioNote->setText(tr("Note"));
    m_pRadioNote->setChecked(true);
    m_pRadioSchedule = new QRadioButton;
    m_pRadioSchedule->setText(tr("Schedule"));
    QButtonGroup* pBtnGroup2 = new QButtonGroup(this);
    pBtnGroup2->addButton(m_pRadioNote);
    pBtnGroup2->addButton(m_pRadioSchedule);
    pTopLayout->addWidget(pLabelType, 3, 0);
    pTopLayout->addWidget(m_pRadioNote, 3, 1);
    pTopLayout->addWidget(m_pRadioSchedule, 3, 2);
    connect(m_pRadioNote, SIGNAL(clicked()), this, SLOT(onClickNote()));
    connect(m_pRadioSchedule, SIGNAL(clicked()), this, SLOT(onClickSchedule()));

    // ok and cancel
    m_pBtnOk = new QPushButton;
    m_pBtnOk->setIconSize(QSize(48, 48));
    QIcon icon20;
    icon20.addFile(QString::fromUtf8(":/res/save.png"));
    m_pBtnOk->setIcon(icon20);
    m_pBtnOk->setToolTip(tr("Save"));
    m_pBtnOk->setStatusTip(tr("Save"));
    connect(m_pBtnOk, SIGNAL(clicked()), this, SLOT(onOk()));
    m_pBtnCancel = new QPushButton;
    m_pBtnCancel->setIconSize(QSize(48, 16));
    QIcon icon21;
    icon21.addFile(QString::fromUtf8(":/res/cancel.png"));
    m_pBtnCancel->setIcon(icon21);
    m_pBtnCancel->setToolTip(tr("Cancel"));
    m_pBtnCancel->setStatusTip(tr("Cancel"));
    pTopLayout->addWidget(m_pBtnOk,     0, 3, 3, 1);
    pTopLayout->addWidget(m_pBtnCancel, 3, 3, 1, 1);
    connect(m_pBtnCancel, SIGNAL(clicked()), this, SLOT(onCancel()));

    // splitter
    QSplitter* pSplitter = new QSplitter(this);
    pSplitter->setOrientation(Qt::Vertical);
    pMainLayout->addWidget(pSplitter);

    // attachment list
    m_pListAttachments = new CAttachmentListWidget(this);
    pSplitter->addWidget(m_pListAttachments);
    m_pListAttachments->setVisible(false);
    connect(m_pListAttachments, SIGNAL(emitAdd()), this, SLOT(onAddAttachment()));
    connect(m_pListAttachments, SIGNAL(emitDelete()), this, SLOT(onDelAttachment()));
    connect(m_pListAttachments, SIGNAL(emitSaveAs()), this, SLOT(onSaveAttachment()));
    connect(m_pListAttachments, SIGNAL(addAttachment(QString)), this, SLOT(onAddAttachment(QString)));

    // text edit
    m_pEditContent = new CImageTextEdit;
    m_pEditContent->setUndoRedoEnabled(false);
    pSplitter->addWidget(m_pEditContent);
    m_pEditContent->setFontPointSize(G_Settings()->getEditNoteFontSize());

    QList<int> lstHeights;
    lstHeights.push_back(100);
    lstHeights.push_back(500);
    pSplitter->setSizes(lstHeights);

    m_pHtmlHighlighter = new CHtmlHighlighter(m_pEditContent->document());

    connect(m_pEditContent, SIGNAL(textChanged()), this, SLOT(onContentChanged()));
    connect(m_pEditContent, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
    connect(m_pEditContent, SIGNAL(undoAvailable(bool)), this, SLOT(onContentUndoAvailable(bool)));
    connect(m_pEditContent, SIGNAL(redoAvailable(bool)), this, SLOT(onContentRedoAvailable(bool)));

    QList<int> lstSizes;
    lstSizes.push_back(50);
    lstSizes.push_back(1000);
    pSplitter->setSizes(lstSizes);

    // connect
    connect(m_pListAttachments, SIGNAL(dataChanged()), this, SLOT(onDataChanged()));
    connect(m_pListAttachments, SIGNAL(itemSelectionChanged()), this, SLOT(onAttachmentListSelectionChanged()));
}

void CDlgEditNote::initControls()
{
    m_pEditContent->setUndoRedoEnabled(false);

    // init controls
    if(m_pNoteItemOrg)
    {
        m_pRadioPrivate->setChecked(m_pNoteItemOrg->getShareType() != SHARE_TYPE_PUBLIC);
        m_pRadioPublic->setChecked(m_pNoteItemOrg->getShareType() == SHARE_TYPE_PUBLIC);

        m_pEditTitle->setText(m_pNoteItemOrg->getTitle());
        m_pEditDate->setDate(m_pNoteItemOrg->getSetAt().date());

        if(m_pNoteItemOrg->isSchedule())
        {
            m_pRadioSchedule->setChecked(true);
            m_pEditTime->setTime(m_pNoteItemOrg->getSetAt().time());
        }
        else
            m_pRadioNote->setChecked(true);
    }
    else
    {
        m_pRadioPrivate->setChecked(true);
        m_pRadioPublic->setChecked(false);
        m_pRadioNote->setChecked(true);
        m_pRadioSchedule->setChecked(false);
        m_pEditTitle->setText("");
    }

    m_pEditContent->setHtml(m_pNoteItem->getContent());

    if(!G_Settings()->isEditNoteHtml())
        m_pEditContent->setPlainText(m_pEditContent->document()->toHtml());

    m_pActionTextColor->setEnabled(G_Settings()->isEditNoteHtml());
    m_pActionTextFont->setEnabled(G_Settings()->isEditNoteHtml());
    m_pSpinBoxTextSize->setEnabled(G_Settings()->isEditNoteHtml());

//    m_pEditContent->setPlainText(m_pEditContent->document()->toHtml());
    m_pEditContent->setUndoRedoEnabled(true);

    // attachments
    m_pListAttachments->clear();

    QList<CAttachmentItem*> lstAttachments = m_pNoteItem->getAttachmentList();
    if(lstAttachments.length())
    {
        foreach(CAttachmentItem* pItem, lstAttachments)
        {
            if(pItem->getState() == STATE_DELETE)
                continue;

            m_pListAttachments->addAttachmentItem(pItem);
        }
    }

    m_pListAttachments->setVisible(m_pListAttachments->count() > 0);
    refreshControlStatus();
}

void CDlgEditNote::setDate(const QDate& date)
{
    m_pEditDate->setDate(date);
}

CNoteItem* CDlgEditNote::getNoteItemOrg() const
{
    return m_pNoteItemOrg;
}

CNoteItem* CDlgEditNote::getNoteItem() const
{
    return m_pNoteItem;
}

CNoteItem* CDlgEditNote::detachNoteItem()
{
    CNoteItem* pItem = m_pNoteItem;
    m_pNoteItem = NULL;
    return pItem;
}

bool CDlgEditNote::isModified()
{
	return m_bModified;
}

void CDlgEditNote::setModified(bool bModified/* = true*/)
{
    if(m_bModified != bModified)
    {
        m_bModified = bModified;
    }
}

void CDlgEditNote::onClickPrivate()
{
    if(m_pNoteItem->getShareType() == SHARE_TYPE_PRIVATE)
        return;

    setModified(true);
}

void CDlgEditNote::onClickPublic()
{
    if(m_pNoteItem->getShareType() == SHARE_TYPE_PUBLIC)
        return;

    setModified(true);
}

void CDlgEditNote::onDataChanged()
{
    setModified(true);
}

void CDlgEditNote::onAttachmentListSelectionChanged()
{
    m_pActionDelAttachment->setEnabled(m_pListAttachments->selectedItems().length() > 0);
    m_pActionSaveAttachment->setEnabled(m_pListAttachments->selectedItems().length() > 0);
}

void CDlgEditNote::onTitleChanged()
{
    setModified(true);
}

void CDlgEditNote::onContentChanged()
{
    setModified(true);
}

void CDlgEditNote::onCursorPositionChanged()
{
    m_bCursorPosChanging = true;
    QTextCharFormat textCharFmt = m_pEditContent->textCursor().charFormat();
    m_pSpinBoxTextSize->setValue(textCharFmt.fontPointSize());
    m_bCursorPosChanging = false;
}

void CDlgEditNote::onContentUndoAvailable(bool b)
{
    m_pActionUndo->setEnabled(b);
}

void CDlgEditNote::onContentRedoAvailable(bool b)
{
    m_pActionRedo->setEnabled(b);
}

void CDlgEditNote::onClickNote()
{
    if(!m_pNoteItem->isNote())
    {
        setModified(true);
    }

    refreshControlStatus();
}

void CDlgEditNote::onClickSchedule()
{
    if(!m_pNoteItem->isSchedule())
    {
        setModified(true);
    }

    refreshControlStatus();
}

void CDlgEditNote::onUndo()
{
    m_pEditContent->undo();
}

void CDlgEditNote::onRedo()
{
    m_pEditContent->redo();
}

void CDlgEditNote::onCut()
{
    m_pEditContent->cut();
}

void CDlgEditNote::onCopy()
{
    m_pEditContent->copy();
}

void CDlgEditNote::onPaste()
{
    m_pEditContent->paste();
}

void CDlgEditNote::onAddImage()
{
    QString strExts;
    QList<QByteArray> lstFormats = QImageReader::supportedImageFormats();
    foreach(QByteArray arrBytes, lstFormats)
    {
        if(!strExts.isEmpty())
            strExts += " ";

        strExts += "*." + QString::fromAscii(arrBytes);
    }

    QString strImgFileFilter = QString("Images (%1);;All files (*.*)").arg(strExts);
    QString strPathFile = QFileDialog::getOpenFileName(this, tr("Add an image"), QString(), strImgFileFilter);
    if(strPathFile.isEmpty())
        return;

    QFileInfo fileInfo(strPathFile);
    if(fileInfo.size() > MAX_NOTE_IMAGE_SIZE) {
        QMessageBox::warning(this, tr("File size error"), tr("File size should be less than 100MB."));
        return;
    }

    if(m_pEditTitle->text().trimmed().length() == 0)
    {
        m_pEditTitle->setText(fileInfo.fileName());
    }

    QImage image(strPathFile);
    if (!image.isNull())
    {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        image.save(&buffer, "PNG");
        QString url = QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\" />";
        m_pEditContent->textCursor().insertHtml(url);
    }
    else
    {
        QMessageBox::warning(this, tr("Image Error"), tr("No image data or the format isn't supported."));
    }
}

void CDlgEditNote::onAddAttachment()
{
    QStringList lstFileNames = QFileDialog::getOpenFileNames(this, tr("Add attachments"));
    if(lstFileNames.length())
    {
        for(int i = 0; i < lstFileNames.length(); i++)
        {
            onAddAttachment(lstFileNames.at(i));
        }
    }
}

void CDlgEditNote::onAddAttachment(const QString& strPathFile)
{
    if(strPathFile.isEmpty())
        return;

    QFileInfo fileInfo(strPathFile);
    if(fileInfo.size() > MAX_ATTACHMENT_SIZE)
    {
        QMessageBox::warning(this, tr("File size error"), tr("File size should be less than 500MB."));
        return;
    }

    if(m_pEditTitle->text().trimmed().length() == 0)
    {
        m_pEditTitle->setText(fileInfo.fileName());
    }

    CAttachmentItem* pAttachmentItem = new CAttachmentItem();
    pAttachmentItem->setFileName(strPathFile);
    pAttachmentItem->setState(STATE_NEW);
    pAttachmentItem->setContentSize(fileInfo.size());
    pAttachmentItem->setLoadFromFile();

    if(!m_pListAttachments->addAttachmentItem(pAttachmentItem))
    {
        delete pAttachmentItem, pAttachmentItem = NULL;
        return;
    }

    m_pNoteItem->addAttachmentItem(pAttachmentItem);

    if(!m_pListAttachments->isVisible())
        m_pListAttachments->setVisible(true);

    setModified();
}

void CDlgEditNote::onDelAttachment()
{
    m_pListAttachments->delAttachmentItem();
    setModified();

    if(m_pListAttachments->count() == 0)
        m_pListAttachments->setVisible(false);
}

void CDlgEditNote::onSaveAttachment()
{
    QList<CAttachmentItem*> lstAttachmentItems = m_pListAttachments->getSelectedAttachments();

    QDir dirFile;
    QString strFileName;
    foreach(CAttachmentItem* pItem, lstAttachmentItems)
    {
        if(!pItem)
            continue;

        if(strFileName.isEmpty() || dirFile.exists(pItem->getFileNameDisplay()))
        {
            if(strFileName.isEmpty())
            {
                strFileName = QFileDialog::getSaveFileName(this, tr("Save attachments"), pItem->getFileName());
            }
            else
            {
                strFileName = QFileDialog::getSaveFileName(this, tr("Save attachments"), dirFile.filePath(pItem->getFileNameDisplay()));
            }

            if(strFileName.isEmpty())
                return;

            QFileInfo fileInfo(strFileName);
            dirFile.setPath(fileInfo.path());
        }
        else
        {
            strFileName = dirFile.filePath(pItem->getFileNameDisplay());
        }

        QFile file(strFileName);
        if(!file.open(QFile::WriteOnly))
        {
            QMessageBox::critical(this, tr("Fail to save"), tr("Fail to save file \"") + strFileName + ("\"."));
            return;
        }

        // load from local file
        if(pItem->isLoadFromFile())
        {
            if(!pItem->loadContent(this))
                return;
        }
        // load from local DB
        else
        {
            G_MainFrame()->getLocalDbManager()->loadAttachmentContent(pItem);
        }

        file.write(pItem->getContent());
        pItem->clearContent();
    }
}

void CDlgEditNote::onHtml()
{
    if(!m_pActionHtml->isChecked())
    {
        m_pEditContent->setHtml(m_pEditContent->document()->toPlainText());
    }
    else
    {
        m_pEditContent->setTextColor(QColor(0,0,0));
        m_pEditContent->setFontPointSize(G_Settings()->getEditNoteFontSize());
        m_pEditContent->setPlainText(m_pEditContent->document()->toHtml());
    }

    G_Settings()->setEditNoteHtml(!m_pActionHtml->isChecked());

    m_pActionTextColor->setEnabled(G_Settings()->isEditNoteHtml());
    m_pActionTextFont->setEnabled(G_Settings()->isEditNoteHtml());
    m_pSpinBoxTextSize->setEnabled(G_Settings()->isEditNoteHtml());
}

void CDlgEditNote::onNoteImport()
{
    QString strPathFile = QFileDialog::getOpenFileName(this, tr("Import note content"));
    if(!strPathFile.isEmpty())
    {
        QFile inFile(strPathFile);
        if(inFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            m_pEditContent->setHtml(inFile.readAll());

            if(!G_Settings()->isEditNoteHtml())
                m_pEditContent->setPlainText(m_pEditContent->document()->toHtml());

            // extract title
            QString strTitle = m_pEditTitle->text().trimmed();
            if(strTitle.isEmpty())
            {
                m_pEditTitle->setText(m_pEditContent->toPlainText().left(15));
            }
        }
        else
        {
            QMessageBox::warning(this, tr("Error"), tr("Fail to open file '") + strPathFile + "'");
        }
    }
}

void CDlgEditNote::onNoteExport()
{
    QString strPathFile = QFileDialog::getSaveFileName(this, tr("Export note content"));
    if(!strPathFile.isEmpty())
    {
        QFile outFile(strPathFile);
        if(outFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        {
            outFile.write(m_pEditContent->toHtml().toUtf8());
        }
        else
        {
            QMessageBox::warning(this, tr("Error"), tr("Fail to open file '") + strPathFile + "'");
        }
    }
}

void CDlgEditNote::onOk()
{
    QString strTitle = m_pEditTitle->text().trimmed();
    if(strTitle.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Title is empty."));
        return;
    }
    m_pNoteItem->setTitle(strTitle);
    QDateTime dtSetAt = m_pEditDate->dateTime();

    if(m_pRadioPrivate->isChecked())
    {
        m_pNoteItem->setShareType(SHARE_TYPE_PRIVATE);
    }
    else
    {
        m_pNoteItem->setShareType(SHARE_TYPE_PUBLIC);
    }

    if(m_pRadioNote->isChecked())
    {
        m_pNoteItem->setType(NOTE_TYPE_NOTE);
    }
    else
    {
        m_pNoteItem->setType(NOTE_TYPE_SCHEDULE);
        dtSetAt.setTime(m_pEditTime->dateTime().time());
    }

    m_pNoteItem->setSetAt(dtSetAt);

    if(m_pEditContent->document()->toPlainText().length())
    {
        if(G_Settings()->isEditNoteHtml())
        {
            m_pNoteItem->setContent(m_pEditContent->document()->toHtml().toUtf8());
        }
        else
        {
            m_pNoteItem->setContent(m_pEditContent->document()->toPlainText().toUtf8());
        }

 //       m_pNoteItem->setContent(m_pEditContent->document()->toHtml().toAscii());
    }
    else
        m_pNoteItem->setContent(QByteArray());

    if(m_pNoteItem->getContentSize() > MAX_NOTE_CONTENT_SIZE)
    {
        QMessageBox::warning(this, tr("File size error"), tr("Note content size should be less than 500MB."));
        return;
    }

    if(!m_pNoteItem->loadAttachmentContents(this))
        return;

    // attachment list
//    QDialog::accept();
    if(m_pNoteItemOrg && m_pNoteItemOrg->equalTo(m_pNoteItem))
    {
        G_MainFrame()->afterEditNoteCancel();
    }
    else
    {
        G_MainFrame()->afterEditNote();
    }

    endDialog();
}

void CDlgEditNote::onCancel()
{
    /*
    if(isModified())
    {
        if(QMessageBox::Cancel == QMessageBox::question(this, tr("Cancel"), tr("Do you want to cancel the modified item?"), QMessageBox::Yes | QMessageBox::Cancel))
            return;
    }
    */


//    QDialog::reject();
    G_MainFrame()->afterEditNoteCancel();

    endDialog();
}

void CDlgEditNote::OnSelectionChanged()
{
    UpdateFontAndSize();
}

void CDlgEditNote::OnCurrentFontChanged(const QFont &)
{
    ChangeCurrentFont();
}

void CDlgEditNote::OnCurrentIndexChangedFontSize(const QString &)
{
    ChangeCurrentFont();
}

void CDlgEditNote::onTextColor()
{
    QTextCharFormat textCharFmt = m_pEditContent->textCursor().charFormat();

    QColorDialog dlg(this);
    dlg.setCurrentColor(textCharFmt.foreground().color());
    if(QDialog::Accepted == dlg.exec())
    {
        QTextCharFormat textCharFmt2;
        textCharFmt2.setForeground(dlg.selectedColor());
        m_pEditContent->textCursor().mergeCharFormat(textCharFmt2);
    }
}

void CDlgEditNote::onTextFont()
{
    QTextCharFormat textCharFmt = m_pEditContent->textCursor().charFormat();

    QFont ft(textCharFmt.fontFamily(), textCharFmt.fontPointSize());
    QFontDialog dlg(ft, this);
    if(QDialog::Accepted == dlg.exec())
    {
        QTextCharFormat textCharFmt2;
        m_bCursorPosChanging = true;
        textCharFmt2.setFontFamily(dlg.selectedFont().family());
        textCharFmt2.setFontPointSize(dlg.selectedFont().pointSize());
        m_pEditContent->textCursor().mergeCharFormat(textCharFmt2);
        m_pSpinBoxTextSize->setValue(dlg.selectedFont().pointSize());
        m_bCursorPosChanging = false;
    }
}

void CDlgEditNote::onTextSize(int nVal)
{
    if(m_bCursorPosChanging)
        return;

    QTextCharFormat textCharFmt;
    textCharFmt.setFontPointSize(nVal);
    m_pEditContent->textCursor().mergeCharFormat(textCharFmt);
}

void CDlgEditNote::showEvent(QShowEvent * )
{
    m_pEditTitle->setFocus();
    setModified(false);
    m_pEditContent->setUndoRedoEnabled(false);
    m_pEditContent->setUndoRedoEnabled(true);
    refreshControlStatus();
}

void CDlgEditNote::refreshControlStatus()
{
    m_pEditTime->setVisible(!m_pRadioNote->isChecked());
    m_pActionDelAttachment->setEnabled(m_pListAttachments->isVisible() && m_pListAttachments->currentIndex().isValid());
}

bool CDlgEditNote::CheckNote()
{
    /*
	if(ui.m_pEditTitle->text().trimmed().isEmpty())
	{
		QMessageBox::critical(this, tr("Error"), tr("Title cannot be empty."));
		return false;
	}
        */

	return true;
}

void CDlgEditNote::UpdateFontAndSize()
{
    /*
	QTextCharFormat charFormat = ui.m_pEditNote->currentCharFormat();
	int iSel = ui.m_pComboBoxFont->findText(charFormat.fontFamily());
	if(iSel >= 0)
		ui.m_pComboBoxFont->setCurrentIndex(iSel);

	int iFontSize = charFormat.fontPointSize();
	iSel = ui.m_pComboBoxFontSize->findText(QString("%1").arg(iFontSize));
	if(iSel >= 0)
		ui.m_pComboBoxFontSize->setCurrentIndex(iSel);
        */
}

void CDlgEditNote::ChangeCurrentFont()
{
    /*
	ui.m_pEditNote->setFontFamily(ui.m_pComboBoxFont->currentText());
	ui.m_pEditNote->setFontPointSize(ui.m_pComboBoxFontSize->currentText().toInt());
        */
}
