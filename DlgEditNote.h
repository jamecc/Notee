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

#ifndef EDITWIDGET_H
#define EDITWIDGET_H

#include <QWidget>
#include <QDateTime>
#include <QShowEvent>

class CNoteItem;
class QTextEdit;
class QDateEdit;
class QTimeEdit;
class QLineEdit;
class CAttachmentListWidget;
class QRadioButton;
class QToolBar;
class CImageTextEdit;
class CHtmlHighlighter;
class QSpinBox;
class QPushButton;
class CDlgEditNote : public QWidget
{
	Q_OBJECT
public:
    CDlgEditNote(QWidget *parent = 0);
    CDlgEditNote(CNoteItem* pNoteItem, const QDateTime& dateTime, QWidget *parent = 0);
    ~CDlgEditNote();

    void createControls();
    void initControls();

    void setNoteItem(CNoteItem* pNoteItem);
    void setDate(const QDate& date);
    void endDialog();
    void convertToNewNote();

    CNoteItem* getNoteItemOrg() const;
    CNoteItem* getNoteItem() const;
    CNoteItem* detachNoteItem();

    bool isModified();
    void setModified(bool bModified = true);

	bool CheckNote();
	void UpdateFontAndSize();
	void ChangeCurrentFont();

public Q_SLOTS:
    void onClickPrivate();
    void onClickPublic();
    void onDataChanged();
    void onAttachmentListSelectionChanged();
    void onTitleChanged();
    void onContentChanged();
    void onCursorPositionChanged();
    void onContentUndoAvailable(bool b);
    void onContentRedoAvailable(bool b);
    void onClickNote();
    void onClickSchedule();
    void onUndo();
    void onRedo();
    void onCut();
    void onCopy();
    void onPaste();
    void onAddImage();
    void onAddAttachment();
    void onAddAttachment(const QString& strPathFile);
    void onDelAttachment();
    void onSaveAttachment();
    void onHtml();
    void onNoteImport();
    void onNoteExport();
    void onOk();
    void onCancel();
	void OnSelectionChanged();
	void OnCurrentFontChanged(const QFont &);
	void OnCurrentIndexChangedFontSize(const QString &);
    void onTextColor();
    void onTextFont();
    void onTextSize(int);

protected:
    virtual void showEvent(QShowEvent *);

    void refreshControlStatus();

private:
    bool m_bModified;
    bool m_bCursorPosChanging;
    QDateTime m_dtInit;
    CNoteItem* m_pNoteItemOrg;
    CNoteItem* m_pNoteItem;
    QLineEdit* m_pEditTitle;
    QRadioButton* m_pRadioPrivate;
    QRadioButton* m_pRadioPublic;
    QDateEdit* m_pEditDate;
    QTimeEdit* m_pEditTime;
    QRadioButton* m_pRadioNote;
    QRadioButton* m_pRadioSchedule;
    QPushButton* m_pBtnOk;
    QPushButton* m_pBtnCancel;
    CAttachmentListWidget* m_pListAttachments;
    CImageTextEdit* m_pEditContent;
    CHtmlHighlighter* m_pHtmlHighlighter;

    QToolBar* m_pMainToolbar;
    QAction* m_pActionUndo;
    QAction* m_pActionRedo;
    QAction* m_pActionCut;
    QAction* m_pActionCopy;
    QAction* m_pActionPaste;
    QAction* m_pActionAddImg;
    QAction* m_pActionAddAttachment;
    QAction* m_pActionDelAttachment;
    QAction* m_pActionSaveAttachment;
    QAction* m_pActionHtml;
    QAction* m_pActionTextColor;
    QAction* m_pActionTextFont;
    QSpinBox* m_pSpinBoxTextSize;
    QAction* m_pActionImport;
    QAction* m_pActionExport;
};

#endif // EDITWIDGET_H
