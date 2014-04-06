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

#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QMainWindow>
#include <QProgressBar>
#include <QDateTime>
#include <QTimer>
#include <QMutex>
#include <QSplitter>
#include "DatabaseManager.h"
#include "NetworkManager.h"
#include "Settings.h"
#include "DlgRegister.h"
#include "DlgLogin.h"
#include "DlgEditNote.h"
#include "DlgProgress.h"
#include "SyncThread.h"
#include "TabWidget.h"
#include "TextOutput.h"
#include "TextContent.h"
#include "DlgShortNewNote.h"
#include "IconDockWidget.h"
#include "DlgForgotPwd.h"
#include "DlgBallonTip.h"

enum ActionType
    {WORKING_TYPE_IDLE = 0,
     WORKING_TYPE_LOGIN,
     WORKING_TYPE_REGISTER,
     WORKING_TYPE_SYNC_NOTES,
     WORKING_TYPE_PROCESSING_NOTE_ITEMS};

class CCalendarView;
class CDlgFind;
class QHBoxLayout;
class CMainFrame : public QMainWindow
{
    Q_OBJECT

public:
    CMainFrame(QWidget *parent = 0, Qt::WFlags flags = Qt::Dialog | Qt::WindowMinMaxButtonsHint);
    ~CMainFrame();

    void createActions();
    void createMenu();
    void createToolbar();
    void createStatusbar();

    void addOutputString(QString str, bool bWithTime = true);
    void showBalloonTip(QString strContent, QString strTitle = "");

    bool checkLogin();
    bool isLogin() const;
    bool isLoginOnLocal() const;
    bool isLoginOnServer() const;

    int getUserId();
    int getUserLocalId();

    QDateTime getMaxTime();
    QDateTime getMinTime();

    CDatabaseManager* getLocalDbManager()   {return &m_localDbManager;}

    QAction* getActionSync() const          {return m_pActionSync;}

    void checkDataUpdateAtString();
    void updateLocalDataUpdateAtString();
    void checkAllLocalNoteStatus();

    void beforeSync();
    void afterSync();
    void syncLocalAndServerDb();
//    void syncLocalAndServerDbMonth();
    void syncLocalAndServerDbOffset();

    void stopSync();

    void afterRegister();
    void afterRegisterCancel();
    void afterForgotPwd();
    void afterForgotPwdCancel();
    void afterLogin();
    void afterLoginCancel();
    void afterEditNote();
    void afterEditNoteCancel();

    void addProcessingNoteItem(CNoteItem* pNoteItem);
    void processingNoteItems();
    void removeProcessingNoteItem(CNoteItem* pNoteItem);
    bool isInProcessingNoteList(CNoteItem* pNoteItem);

    void clearProcessAction();
    void addProcessAction(ActionType nAction);
    void doProcessAction();

    void addNoteItem(CNoteItem* pNoteItem);
    void updateNoteItem(CNoteItem* pNoteItemOrg, CNoteItem* pNoteItem);
    void deleteNoteItem(CNoteItem* pNoteItem);
    void selectNoteItem(CNoteItem* pNoteItem, bool bOnView = true, bool bOnFind = true);
    void afterAddNoteItem();
    void showCalendarView();

    QList<CNoteItem*> getNotesByString(const QString& strText);
    QList<CNoteItem*> getCurrentNotes();

    void refreshDayWidget(const QDate& dt);

    void setProgressBarRange(int nMin, int nMax);
    void setProgressBarPos(int nPos);
    void showProgressBar(bool bShow);

    bool isDlgNoteEditing() const;

    // multi-thread
    void insertNoteItemM(CNoteItem* pNoteItem);
    void updateNoteItemM(CNoteItem* pNoteItem, bool bOnlyState = false, bool bByServerId = false, bool bWithoutConten = true);
    void deleteNoteItemM(CNoteItem* pNoteItem);
    void getNoteContentM(const QString& strNoteLocalId, const QByteArray& arrContent);

    void checkClassifyStringServer(const QString& strClassifyServer);
    void updateClassifyString(const QString& str);
    void refreshTabClassifyStrings();
    void addNetworkReply(QNetworkReply* pNetworkReply, const QString& strDesc);
    void addFileDownload(CFileDownload* pFileDownload, const QString& strDesc);
    bool hasNetReplyWorking();

Q_SIGNALS:
    void emitRefreshDayWidget(const QDate& dt);
    void emitInsertNoteItem(CNoteItem* pNoteItem);
    void emitRemoveNoteItem(CNoteItem* pNoteItem);

public Q_SLOTS:
    void onRegister();
    void onLogin();
    void onLogout();
    void onClearLocalUserData();
    void onDlgFindShow(bool);
    void onTextContentShow(bool);
    void onVisibilityShortNewNote(bool);
    void onDlgProgessShow(bool);
    void onNew();
    void onNew(const QDate& date);
    void onEdit();
    void onEdit(CNoteItem* pNoteItem);
    void onDelete();
    void onDelete(CNoteItem* pNoteItem);
    void onOption();
    void onAbout();
    void onForgotPwd();
    void onExit();
    void onClickShortNewNote();
    void onFind();
    void onContentText();
    void onDlgProgress();
    void onSync();
    void onToday();
    void onPrevMonth();
    void onNextMonth();
    void onItemSelectionChanged();

    void onServerDbManagerReadyReadyfinished();
 //   void onNetworkReplyError(QNetworkReply::NetworkError);
 //   void onServerDbManagerFinished();
    void onFinishedDownloadAttachmentItem(CAttachmentItem* pAttachmentItem, QString str);
 //   void onErrorFinishedDownloadAttachmentItem(CAttachmentItem* pAttachmentItem, QString strErr);
  //  void onFinishedDownloadAttachment(const QString& strErr);

    void onSyncProgressBegin();
    void onSyncProgressFinished();
    void onSyncProgressMax(int nPosMax);
    void onSyncProgressPos(int nPos);
    void onSyncError(const QString& str);
    void onSynThreadNetworkReplyFinished();
    void onOutputString(QString str);

protected Q_SLOTS:
    void onTimeOutUserPulse();
    void onEmitRefreshDayWidget(const QDate& dt);
    void onEmitInsertNoteItem(CNoteItem* pNoteItem);
    void onShortNewNote(QString str);

protected:
    virtual void resizeEvent(QResizeEvent * pEvent);
    virtual void showEvent(QShowEvent *);

    void autoLogin();
    void updateControlState();

private:
    bool m_bSender;
    CIconDockWidget* m_pDockWidgetShortNewNote;
    CIconDockWidget* m_pDockWidgetFind;
    CIconDockWidget* m_pDockWidgetContent;
    CIconDockWidget* m_pDockWidgetProgress;
    QDockWidget* m_pDockWidgetOutput;
    CDlgShortNewNote* m_pDlgShortNewNote;
    CDlgFind* m_pDlgFind;
    CTextContent* m_pTextContent;
    CDlgProgress* m_pDlgProgress;
    QToolBar* m_pMainToolbar;
    QStatusBar* m_pStatusBar;
    QProgressBar* m_pProgressBar;
    CTextOutput* m_pTextEditOutput;
    QAction* m_pActionRegister;
    QAction* m_pActionLogin;
    QAction* m_pActionClearUserLocalData;
    QAction* m_pActionLogout;
    QAction* m_pActionNew;
    QAction* m_pActionEdit;
    QAction* m_pActionDelete;
    QAction* m_pActionOption;
    QAction* m_pActionAbout;
    QAction* m_pActionSync;
    QAction* m_pActionShortNewNote;
    QAction* m_pActionFind;
    QAction* m_pActionContentText;
    QAction* m_pActionDlgProgress;
    QAction* m_pActionPrevMonth;
    QAction* m_pActionNextMonth;
    QAction* m_pActionToday;
    QAction* m_pActionForgotPwd;
    QAction* m_pActionExit;

    CCalendarView* m_pCalendarView;
    CDatabaseManager m_localDbManager;
    CNetworkManager m_serverDbManager;
    QHBoxLayout* m_pMainLayout;

    CDlgRegister* m_pDlgRegister;
    CDlgLogin* m_pDlgLogin;
    CDlgForgotPwd* m_pDlgForgotPwd;
    CDlgEditNote* m_pDlgEditNote;
    CDlgBallonTip* m_pDlgBallonTip;

    QList<QDateTime> m_lstDateNetGetNotee;
    QList<CNoteItem*> m_lstNoteItemsFromNet;

    int m_nWorkingType;

    QString m_strLoginUserNameTmp;
    QString m_strLoginPwdTmp;
    int m_nLoginUserIdServer;

    QDate m_dtCurrentProcessMonth;
    int m_nAmountOfNotes;
    int m_nAmountOfItems;
    int m_nNumberPerTimes;
    int m_nCurrentOffsetOfNotes;
    int m_nCurrentPosOfNotes;

    CNoteItem* m_pSelectedNoteItem;
    CNoteItem* m_pProcessingNoteItem;
    QList<CNoteItem*> m_lstProcessingNoteItems;
    CAttachmentItem* m_pProcessingAttachment;
    QList<CAttachmentItem*> m_lstProcessingAttachments;

    QList<ActionType> m_lstActions;

    bool m_bGetingUserPulse;
    QTimer m_timerUserPulse;
    QString m_strDataUpdatedAt;
    bool m_bAllItemsNormalStatus;

    CSyncThread* m_pSyncThread;
    QMutex m_mutexModifyNote;
    QByteArray m_arrBufferSync;
};

extern CSettings* G_Settings();
extern CMainFrame* G_MainFrame();

#endif // MAINFRAME_H
