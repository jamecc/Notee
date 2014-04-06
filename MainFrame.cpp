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
#include <QtGui/QMdiArea>
#include <QSignalMapper>
#include <QToolBar>
#include <QSqlError>
#include <QDesktopServices>
#include "DlgEditNote.h"
#include "DlgRegister.h"
#include "DlgLogin.h"
#include "DlgFind.h"
#include "ConfigDialog.h"
#include "CalendarScene.h"
#include "NoteItem.h"
#include "GraphicsViewLayoutScene.h"
#include "CommonFunctions.h"
#include "CalendarView.h"
#include "AttachmentItem.h"
#include "MainFrame.h"
#include "FileDownload.h"

#define URL_DOWNLOAD_MAC  "http://cmpp.coodesoft.com/main/widgets#notee"


CMainFrame::CMainFrame(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
    m_bSender = false;
    m_nWorkingType = WORKING_TYPE_IDLE;
    m_pProcessingNoteItem = NULL;
    m_pProcessingAttachment = NULL;
    m_nAmountOfNotes = 0;
    m_nAmountOfItems = 0;
    m_nNumberPerTimes = 10;
    m_nCurrentOffsetOfNotes = 0;
    m_pSelectedNoteItem = NULL;
    m_nLoginUserIdServer = 0;
    m_bGetingUserPulse = false;

    QIcon icon;
    icon.addFile(QString::fromUtf8(":/res/calendar.png"), QSize(), QIcon::Normal, QIcon::Off);
    setWindowIcon(icon);

    // open database
    if(!m_localDbManager.OpenDB())
    {
        QSqlError errSql = m_localDbManager.GetLastError();
        QMessageBox::critical(this, "Error", errSql.text());
    }

    // menubar toolbar statusbar
    createActions();
    createMenu();
    createToolbar();
    createStatusbar();

    QWidget* pMainWidget = new QWidget(this);
    setCentralWidget(pMainWidget);
    m_pMainLayout = new QHBoxLayout();
    pMainWidget->setLayout(m_pMainLayout);
    m_pMainLayout->setSpacing(0);
    m_pMainLayout->setMargin(0);

    m_pCalendarView = new CCalendarView(&m_localDbManager, this);
    m_pCalendarView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pCalendarView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pMainLayout->addWidget(m_pCalendarView);

    connect(m_pCalendarView, SIGNAL(newNote(QDate)), this, SLOT(onNew(QDate)));
    connect(m_pCalendarView, SIGNAL(editNote(CNoteItem*)), this, SLOT(onEdit(CNoteItem*)));
    connect(m_pCalendarView, SIGNAL(newNote()), this, SLOT(onNew()));
    connect(m_pCalendarView, SIGNAL(editNote()), this, SLOT(onEdit()));
    connect(m_pCalendarView, SIGNAL(delNote()), this, SLOT(onDelete()));

    CCalendarScene* pCalendarScene = qobject_cast<CCalendarScene*>(m_pCalendarView->scene());
    if(pCalendarScene)
    {
        connect(pCalendarScene, SIGNAL(clickPrevious()), this, SLOT(onPrevMonth()));
        connect(pCalendarScene, SIGNAL(clickNext()), this, SLOT(onNextMonth()));
    }

    // new note widget
    m_pDockWidgetShortNewNote = new CIconDockWidget(QIcon(":/res/new.png"), tr("New Note"), this);
    m_pDockWidgetShortNewNote->setObjectName("DockWidgetNewNote");
    m_pDockWidgetShortNewNote->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_pDlgShortNewNote = new CDlgShortNewNote(m_pDockWidgetShortNewNote);
    m_pDockWidgetShortNewNote->setWidget(m_pDlgShortNewNote);
    addDockWidget(Qt::RightDockWidgetArea, m_pDockWidgetShortNewNote);
    connect(m_pDlgShortNewNote,SIGNAL(emitReturnString(QString)),this,SLOT(onShortNewNote(QString)));
    connect(m_pDockWidgetShortNewNote, SIGNAL(visibilityChanged(bool)), this, SLOT(onVisibilityShortNewNote(bool)));

    // find widget
    m_pDockWidgetFind = new CIconDockWidget(QIcon(":/res/find.png"), tr("Find"),this);
    m_pDockWidgetFind->setObjectName("DockWidgetFind");
    m_pDockWidgetFind->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_pDlgFind = new CDlgFind(m_pDockWidgetFind);
    m_pDockWidgetFind->setWidget(m_pDlgFind);
    connect(m_pDockWidgetFind, SIGNAL(visibilityChanged(bool)), this, SLOT(onDlgFindShow(bool)));
    CDCTableWidget* pTableWidgetFind = m_pDlgFind->getTableWidget();
    connect(pTableWidgetFind, SIGNAL(itemSelectionChanged()), this, SLOT(onItemSelectionChanged()));
    addDockWidget(Qt::RightDockWidgetArea, m_pDockWidgetFind);
 //   m_pDockWidgetFind->hide();

    // content widget
    m_pDockWidgetContent = new CIconDockWidget(QIcon(":/res/html.png"), tr("Content(Read only)"),this);
    m_pDockWidgetContent->setObjectName("DockWidgetContent");
    m_pDockWidgetContent->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_pTextContent = new CTextContent(m_pDockWidgetContent);
    m_pDockWidgetContent->setWidget(m_pTextContent);
    connect(m_pDockWidgetContent, SIGNAL(visibilityChanged(bool)), this, SLOT(onTextContentShow(bool)));
    addDockWidget(Qt::RightDockWidgetArea, m_pDockWidgetContent);

    // progress
    m_pDockWidgetProgress = new CIconDockWidget(QIcon(":/res/ui-progress-bar.png"), tr("Progress"),this);
    m_pDockWidgetProgress->setObjectName("DockWidgetProgress");
    m_pDockWidgetProgress->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_pDlgProgress = new CDlgProgress(m_pDockWidgetProgress);
    m_pDockWidgetProgress->setWidget(m_pDlgProgress);
    connect(m_pDockWidgetProgress, SIGNAL(visibilityChanged(bool)), this, SLOT(onDlgProgessShow(bool)));
    addDockWidget(Qt::RightDockWidgetArea, m_pDockWidgetProgress);

    if(!G_Settings()->isShowViewFind())
        m_pDockWidgetFind->hide();

    if(!G_Settings()->isShowViewContent())
        m_pDockWidgetContent->hide();

    if(!G_Settings()->isShowViewProgress())
        m_pDockWidgetProgress->hide();

    // output
    m_pDockWidgetOutput = new QDockWidget(tr("Output"), this);
    m_pDockWidgetOutput->setObjectName("DockWidgetOutput");
    m_pDockWidgetOutput->setAllowedAreas(Qt::AllDockWidgetAreas);
    m_pTextEditOutput = new CTextOutput();
    m_pDockWidgetOutput->setWidget(m_pTextEditOutput);
    m_pDockWidgetOutput->setAllowedAreas(Qt::AllDockWidgetAreas);
    addDockWidget(Qt::BottomDockWidgetArea, m_pDockWidgetOutput);

    connect(&m_localDbManager, SIGNAL(noteItemSelectionChanged()), this, SLOT(onItemSelectionChanged()));

    connect(&m_serverDbManager, SIGNAL(readyReadyfinished()), this, SLOT(onServerDbManagerReadyReadyfinished()));
 //   connect(&m_serverDbManager, SIGNAL(replyError(QNetworkReply::NetworkError)), this, SLOT(onNetworkReplyError(QNetworkReply::NetworkError)));
 //   connect(&m_serverDbManager, SIGNAL(finished()), this, SLOT(onServerDbManagerFinished()));
    connect(&m_serverDbManager, SIGNAL(finishedDownloadAttachmentItem(CAttachmentItem*,QString)), this, SLOT(onFinishedDownloadAttachmentItem(CAttachmentItem*,QString)));
 //   connect(&m_serverDbManager, SIGNAL(errorFinishedDownloadAttachmentItem(CAttachmentItem*,QString)), this, SLOT(onErrorFinishedDownloadAttachmentItem(CAttachmentItem*,QString)));
 //   connect(&m_serverDbManager, SIGNAL(finishedDownloadAttachment(QString)), this, SLOT(onFinishedDownloadAttachment(QString)));

    // in 2 minutes
    m_timerUserPulse.setInterval(2*60*1000);
    connect(&m_timerUserPulse, SIGNAL(timeout()), this, SLOT(onTimeOutUserPulse()));

    if(G_Settings()->getCheckPadRegular())
    {
        m_serverDbManager.getPadFile();
    }

    restoreState(G_Settings()->getMainFrameState());

#ifdef NOTEE_DEBUG
    m_pDockWidgetOutput->show();
#else
    m_pDockWidgetOutput->hide();
#endif

    m_pDlgRegister = NULL;
    m_pDlgLogin = NULL;
    m_pDlgForgotPwd = NULL;
    m_pDlgEditNote = NULL;
    m_pDlgBallonTip = NULL;

    connect(this, SIGNAL(emitRefreshDayWidget(QDate)), this, SLOT(onEmitRefreshDayWidget(QDate)));
    connect(this, SIGNAL(emitInsertNoteItem(CNoteItem*)), this, SLOT(onEmitInsertNoteItem(CNoteItem*)));

    // thread
    m_pSyncThread = new CSyncThread(this);
    connect(m_pSyncThread, SIGNAL(syncProgressBegin()), this, SLOT(onSyncProgressBegin()));
    connect(m_pSyncThread, SIGNAL(syncProgressFinished()), this, SLOT(onSyncProgressFinished()));
    connect(m_pSyncThread, SIGNAL(signalSyncProgressMax(int)), this, SLOT(onSyncProgressMax(int)));
    connect(m_pSyncThread, SIGNAL(signalSyncProgressPos(int)), this, SLOT(onSyncProgressPos(int)));
    connect(m_pSyncThread, SIGNAL(emitError(QString)), this, SLOT(onSyncError(QString)));
    connect(m_pSyncThread, SIGNAL(networkReplyFinished()), this, SLOT(onSynThreadNetworkReplyFinished()));

    connect(&m_localDbManager,SIGNAL(emitMsg(QString)),this,SLOT(onOutputString(QString)));
    connect(&m_serverDbManager,SIGNAL(emitMsg(QString)),this,SLOT(onOutputString(QString)));
}

CMainFrame::~CMainFrame()
{
    G_Settings()->setMainFrameState(saveState());
    G_Settings()->setShowViewFind(!m_pDockWidgetFind->isHidden());
    G_Settings()->setShowViewContent(!m_pDockWidgetContent->isHidden());
    G_Settings()->setShowCalendarTable(!m_pCalendarView->isHidden());
    G_Settings()->setShowViewProgress(!m_pDockWidgetProgress->isHidden());

    m_localDbManager.clearNoteItems();
    m_serverDbManager.clearNoteItems();
}

void CMainFrame::createActions()
{
    m_pActionRegister = new QAction(tr("Register..."),this);
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/res/register.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionRegister->setIcon(icon1);
    m_pActionRegister->setToolTip(tr("Register..."));
    m_pActionRegister->setStatusTip(tr("Register..."));
    connect(m_pActionRegister, SIGNAL(triggered()), this, SLOT(onRegister()));

    m_pActionLogin = new QAction(tr("Login..."),this);
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/res/login.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionLogin->setIcon(icon2);
    m_pActionLogin->setToolTip(tr("Login..."));
    m_pActionLogin->setStatusTip(tr("Login..."));
    connect(m_pActionLogin, SIGNAL(triggered()), this, SLOT(onLogin()));

    m_pActionClearUserLocalData = new QAction(tr("Clear local data"),this);
    m_pActionClearUserLocalData->setToolTip(tr("Clear user local data"));
    m_pActionClearUserLocalData->setStatusTip(tr("Clear user local data"));
    connect(m_pActionClearUserLocalData, SIGNAL(triggered()), this, SLOT(onClearLocalUserData()));

    m_pActionLogout = new QAction(tr("Logout"), this);
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/res/logout.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionLogout->setIcon(icon3);
    m_pActionLogout->setToolTip(tr("Logout"));
    m_pActionLogout->setStatusTip(tr("Logout"));
    connect(m_pActionLogout, SIGNAL(triggered()), this, SLOT(onLogout()));

    m_pActionNew = new QAction(tr("New..."), this);
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/res/new.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionNew->setIcon(icon4);
    m_pActionNew->setToolTip(tr("New..."));
    m_pActionNew->setStatusTip(tr("New..."));
    connect(m_pActionNew, SIGNAL(triggered()), this, SLOT(onNew()));

    m_pActionEdit = new QAction(tr("Edit..."), this);
    QIcon icon5;
    icon5.addFile(QString::fromUtf8(":/res/edit.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionEdit->setIcon(icon5);
    m_pActionEdit->setToolTip(tr("Edit..."));
    m_pActionEdit->setStatusTip(tr("Edit..."));
    connect(m_pActionEdit, SIGNAL(triggered()), this, SLOT(onEdit()));

    m_pActionDelete = new QAction(tr("Delete"), this);
    QIcon icon6;
    icon6.addFile(QString::fromUtf8(":/res/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionDelete->setIcon(icon6);
    m_pActionDelete->setToolTip(tr("Delete"));
    m_pActionDelete->setStatusTip(tr("Delete"));
    connect(m_pActionDelete, SIGNAL(triggered()), this, SLOT(onDelete()));

    m_pActionOption = new QAction(tr("Option..."),this);
    QIcon icon61;
    icon61.addFile(QString::fromUtf8(":/res/config.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionOption->setIcon(icon61);
    m_pActionOption->setToolTip(tr("Option..."));
    m_pActionOption->setStatusTip(tr("Option..."));
    connect(m_pActionOption, SIGNAL(triggered()), this, SLOT(onOption()));

    m_pActionAbout = new QAction(tr("About..."),this);
    QIcon icon7;
    icon7.addFile(QString::fromUtf8(":/res/question.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionAbout->setIcon(icon7);
    m_pActionAbout->setToolTip(tr("About..."));
    m_pActionAbout->setStatusTip(tr("About..."));
    connect(m_pActionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

    m_pActionForgotPwd = new QAction(tr("Forgot password..."),this);
    m_pActionForgotPwd->setToolTip(tr("Forgot password..."));
    m_pActionForgotPwd->setStatusTip(tr("Forgot password..."));
    connect(m_pActionForgotPwd, SIGNAL(triggered()), this, SLOT(onForgotPwd()));

    m_pActionExit = new QAction(tr("Exit"),this);
    QIcon icon8;
    icon8.addFile(QString::fromUtf8(":/res/exit.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionExit->setIcon(icon8);
    m_pActionExit->setToolTip(tr("Exit"));
    m_pActionExit->setStatusTip(tr("Exit"));
    connect(m_pActionExit, SIGNAL(triggered()), this, SLOT(onExit()));

    m_pActionSync = new QAction(tr("Synchronize"),this);
    QIcon icon9;
    icon9.addFile(QString::fromUtf8(":/res/sync.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionSync->setIcon(icon9);
    m_pActionSync->setToolTip(tr("Synchronize with Server"));
    m_pActionSync->setStatusTip(tr("Synchronize with Server"));
    connect(m_pActionSync, SIGNAL(triggered()), this, SLOT(onSync()));

    m_pActionShortNewNote = new QAction(tr("New note panel"), this);
    m_pActionShortNewNote->setToolTip(tr("New note panel"));
    m_pActionShortNewNote->setStatusTip(tr("New note panel"));
    m_pActionShortNewNote->setCheckable(true);
    connect(m_pActionShortNewNote, SIGNAL(triggered()), this, SLOT(onClickShortNewNote()));

    m_pActionFind = new QAction(tr("Find panel"),this);
    QIcon icon91;
    icon91.addFile(QString::fromUtf8(":/res/find.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionFind->setIcon(icon91);
    m_pActionFind->setToolTip(tr("Find"));
    m_pActionFind->setStatusTip(tr("Find"));
    m_pActionFind->setCheckable(true);
    connect(m_pActionFind, SIGNAL(triggered()), this, SLOT(onFind()));

    m_pActionContentText = new QAction(tr("Content panel"),this);
    m_pActionContentText->setToolTip(tr("Content panel"));
    m_pActionContentText->setStatusTip(tr("Content panel"));
    m_pActionContentText->setCheckable(true);
    connect(m_pActionContentText, SIGNAL(triggered()), this, SLOT(onContentText()));

    m_pActionDlgProgress = new QAction(tr("Progress panel"),this);
    m_pActionDlgProgress->setToolTip(tr("Progress panel"));
    m_pActionDlgProgress->setStatusTip(tr("Progress panel"));
    m_pActionDlgProgress->setCheckable(true);
    connect(m_pActionDlgProgress, SIGNAL(triggered()), this, SLOT(onDlgProgress()));

    m_pActionToday = new QAction(tr("Today"), this);
    QIcon icon92;
    icon92.addFile(QString::fromUtf8(":/res/calendar_today.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionToday->setIcon(icon92);
    m_pActionToday->setToolTip(tr("Today"));
    m_pActionToday->setStatusTip(tr("Today"));
    m_pActionToday->setEnabled(false);
    connect(m_pActionToday, SIGNAL(triggered()), this, SLOT(onToday()));

    m_pActionPrevMonth = new QAction(tr("Prev month"),this);
    QIcon icon93;
    icon93.addFile(QString::fromUtf8(":/res/arrow_prev.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionPrevMonth->setIcon(icon93);
    m_pActionPrevMonth->setToolTip(tr("Previous month"));
    m_pActionPrevMonth->setStatusTip(tr("Previous month"));
    connect(m_pActionPrevMonth, SIGNAL(triggered()), this, SLOT(onPrevMonth()));

    m_pActionNextMonth = new QAction(tr("Next month"),this);
    QIcon icon94;
    icon94.addFile(QString::fromUtf8(":/res/arrow_next.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_pActionNextMonth->setIcon(icon94);
    m_pActionNextMonth->setToolTip(tr("Next month"));
    m_pActionNextMonth->setStatusTip(tr("Next month"));
    connect(m_pActionNextMonth, SIGNAL(triggered()), this, SLOT(onNextMonth()));
}

void CMainFrame::createMenu()
{
    QMenu* pMenuBegin = menuBar()->addMenu(tr("&Begin"));
    QMenu* pMenuEdit = menuBar()->addMenu(tr("&Edit"));
    QMenu* pMenuTools = menuBar()->addMenu(tr("&Tools"));
    QMenu* pMenuHelp = menuBar()->addMenu(tr("&Help"));

    // begin
    pMenuBegin->addAction(m_pActionRegister);
    pMenuBegin->addAction(m_pActionLogin);
    pMenuBegin->addAction(m_pActionClearUserLocalData);
    pMenuBegin->addAction(m_pActionLogout);
    pMenuBegin->addSeparator();
    pMenuBegin->addAction(m_pActionForgotPwd);
    pMenuBegin->addSeparator();
    pMenuBegin->addAction(m_pActionExit);

    // edit
    pMenuEdit->addAction(m_pActionNew);
    pMenuEdit->addAction(m_pActionEdit);
    pMenuEdit->addAction(m_pActionDelete);
    pMenuEdit->addSeparator();
    pMenuEdit->addAction(m_pActionToday);
    pMenuEdit->addAction(m_pActionPrevMonth);
    pMenuEdit->addAction(m_pActionNextMonth);
    pMenuEdit->addSeparator();
    QMenu* pViewsMenu = pMenuEdit->addMenu(tr("Panels"));
    pViewsMenu->addAction(m_pActionShortNewNote);
    pViewsMenu->addAction(m_pActionFind);
    pViewsMenu->addAction(m_pActionContentText);
    pViewsMenu->addAction(m_pActionDlgProgress);
    pMenuEdit->addAction(m_pActionSync);

    // tools
    pMenuTools->addAction(m_pActionOption);

    // help
    pMenuHelp->addAction(m_pActionAbout);
}

void CMainFrame::createToolbar()
{
    m_pMainToolbar = new QToolBar(tr("Main"), this);
    m_pMainToolbar->setIconSize(QSize(48,48));
    m_pMainToolbar->setObjectName(QString::fromUtf8("main_toolbar"));
    m_pMainToolbar->setMinimumSize(QSize(32, 32));
    m_pMainToolbar->setSizeIncrement(QSize(16, 16));
    m_pMainToolbar->setBaseSize(QSize(16, 16));

//    m_pMainToolbar->addAction(m_pActionRegister);
    m_pMainToolbar->addAction(m_pActionLogin);
    m_pMainToolbar->addAction(m_pActionLogout);
    m_pMainToolbar->addSeparator();
    m_pMainToolbar->addAction(m_pActionNew);
    m_pMainToolbar->addAction(m_pActionEdit);
    m_pMainToolbar->addAction(m_pActionDelete);
    m_pMainToolbar->addSeparator();
    m_pMainToolbar->addAction(m_pActionToday);
//    m_pMainToolbar->addAction(m_pActionPrevMonth);
//    m_pMainToolbar->addAction(m_pActionNextMonth);
//    m_pMainToolbar->addSeparator();
//    m_pMainToolbar->addAction(m_pActionFind);
//    m_pMainToolbar->addAction(m_pActionSync);

    addToolBar(Qt::TopToolBarArea, m_pMainToolbar);
}

void CMainFrame::createStatusbar()
{
    m_pStatusBar = new QStatusBar(this);
    m_pStatusBar->setObjectName(QString::fromUtf8("statusBar"));
    setStatusBar(m_pStatusBar);

    // progress bar
    m_pProgressBar = new QProgressBar(NULL);
    m_pStatusBar->addPermanentWidget(m_pProgressBar);
    m_pProgressBar->setRange(0,100);
    m_pProgressBar->setTextVisible(true);
    m_pProgressBar->setMinimumWidth(5);
    m_pProgressBar->setMaximumHeight(15);
    m_pProgressBar->hide();
}

void CMainFrame::addOutputString(QString str, bool bWithTime)
{
#ifndef NOTEE_DEBUG
    return;
#endif
    QString strText;
    if(bWithTime)
    {
        strText = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ");
    }

    strText += str + "\r\n";
    m_pTextEditOutput->setPlainText(strText + m_pTextEditOutput->toPlainText());
}

void CMainFrame::showBalloonTip(QString strContent, QString strTitle)
{
    if(!m_pDlgBallonTip)
    {
        m_pDlgBallonTip = new CDlgBallonTip(this);
    }

    m_pDlgBallonTip->showBalloon(strContent, strTitle);
}

bool CMainFrame::checkLogin()
{
    if(!isLoginOnLocal() && !isLoginOnServer())
    {
        int nRet = QMessageBox::information(this, tr("Info"),
                                 tr("Please register, login first."),
                                 tr("Login"), tr("Register"), tr("Cancel"));

        switch(nRet)
        {
        case 0:
            onLogin();
            break;
        case 1:
            onRegister();
            break;
        case 2:
        default:
            break;
        }

        return false;
    }

    return true;
}

bool CMainFrame::isLogin() const
{
    return isLoginOnLocal() || isLoginOnServer();
}

bool CMainFrame::isLoginOnLocal() const
{
    return m_localDbManager.getUserLocalId() > 0;
}

bool CMainFrame::isLoginOnServer() const
{
    return !m_serverDbManager.getLoginToken().isEmpty() &&
            m_serverDbManager.getUserId() > 0;
}

int CMainFrame::getUserId()
{
    return m_serverDbManager.getUserId();
}

int CMainFrame::getUserLocalId()
{
    return m_localDbManager.getUserLocalId();
}

void CMainFrame::resizeEvent(QResizeEvent * pEvent)
{
    QMainWindow::resizeEvent(pEvent);
}

void CMainFrame::showEvent(QShowEvent *pEvent)
{
    QMainWindow::showEvent(pEvent);

    if(!isLogin())
        autoLogin();
}

void CMainFrame::onRegister()
{
    m_pCalendarView->hide();

    if(!m_pDlgRegister)
    {
        m_pDlgRegister = new CDlgRegister(this);
        m_pMainLayout->addWidget(m_pDlgRegister);
    }

    m_pDlgRegister->initControls();
    m_pDlgRegister->show();
    updateControlState();
}

void CMainFrame::afterRegister()
{
    m_strLoginUserNameTmp = m_pDlgRegister->getUserName();
    m_strLoginPwdTmp = m_pDlgRegister->getPassword();

    if(G_Settings()->isAlwaysLogin())
    {
        G_Settings()->setLoginName(m_strLoginUserNameTmp);
        G_Settings()->setLoginPassword(m_strLoginPwdTmp);
    }

    m_serverDbManager.register_user(m_strLoginUserNameTmp, m_strLoginPwdTmp, m_pDlgRegister->getEmail());
    m_nWorkingType = WORKING_TYPE_REGISTER;

    showCalendarView();
    updateControlState();
}

void CMainFrame::afterRegisterCancel()
{
    showCalendarView();
    updateControlState();
}

void CMainFrame::onForgotPwd()
{
    m_pCalendarView->hide();

    if(!m_pDlgForgotPwd)
    {
        m_pDlgForgotPwd = new CDlgForgotPwd(this);
        m_pMainLayout->addWidget(m_pDlgForgotPwd);
    }

    m_pDlgForgotPwd->initControls();
    m_pDlgForgotPwd->show();
    updateControlState();
}

void CMainFrame::afterForgotPwd()
{
    QString str = m_pDlgForgotPwd->getEmail();

    if(!str.isEmpty())
        m_serverDbManager.forgotPwd(str);

    m_nWorkingType = WORKING_TYPE_IDLE;

    showCalendarView();
    updateControlState();
}

void CMainFrame::afterForgotPwdCancel()
{
    m_nWorkingType = WORKING_TYPE_IDLE;
    showCalendarView();
    updateControlState();
}

void CMainFrame::onLogin()
{
    if(!m_pDlgLogin)
    {
        m_pDlgLogin = new CDlgLogin(this);
        m_pMainLayout->addWidget(m_pDlgLogin);
    }

    m_pDlgLogin->initControls();

    if(G_Settings()->isAlwaysLogin())
    {
        m_pDlgLogin->setAlwaysLogin(G_Settings()->isAlwaysLogin());
        m_pDlgLogin->setUserName(G_Settings()->getLoginName());
        m_pDlgLogin->setPassword(G_Settings()->getLoginPassword());
    }

    m_pDlgLogin->show();
    m_pCalendarView->hide();
    updateControlState();
}

void CMainFrame::afterLogin()
{
    showCalendarView();

    if(m_pDlgLogin->getAlwaysLogin())
    {
        G_Settings()->setLoginName(m_pDlgLogin->getUserName());
        G_Settings()->setLoginPassword(m_pDlgLogin->getPassword());
    }
    else
    {
        G_Settings()->setLoginName("");
        G_Settings()->setLoginPassword("");
    }
    G_Settings()->setAlwaysLogin(m_pDlgLogin->getAlwaysLogin());
    G_Settings()->save();

    m_strLoginUserNameTmp = m_pDlgLogin->getUserName();
    m_strLoginPwdTmp = m_pDlgLogin->getPassword();
    if(m_localDbManager.login(m_strLoginUserNameTmp, m_strLoginPwdTmp))
    {
        m_pCalendarView->refreshAllDayWidgets();

        refreshTabClassifyStrings();
    }

    m_serverDbManager.login(m_pDlgLogin->getUserName(), m_pDlgLogin->getPassword());
    m_pStatusBar->showMessage(tr("Login on server..."));
    m_pDlgFind->afterAddNoteItem();
    updateControlState();
}

void CMainFrame::afterLoginCancel()
{
    showCalendarView();
    updateControlState();
}

void CMainFrame::afterEditNote()
{
    CNoteItem* pNoteItemOrg = m_pDlgEditNote->getNoteItemOrg();
    CNoteItem* pNoteItemNew = m_pDlgEditNote->detachNoteItem();
    if(pNoteItemOrg)
    {
        updateNoteItem(pNoteItemOrg, pNoteItemNew);
        selectNoteItem(pNoteItemOrg);
        delete pNoteItemNew, pNoteItemNew = NULL;
    }
    else
    {
        addNoteItem(pNoteItemNew);
        afterAddNoteItem();
        selectNoteItem(pNoteItemNew);
    }

    updateLocalDataUpdateAtString();
    showCalendarView();
    updateControlState();
    doProcessAction();
}

void CMainFrame::afterEditNoteCancel()
{
    showCalendarView();
    updateControlState();
}

void CMainFrame::onLogout()
{
    m_nWorkingType = WORKING_TYPE_IDLE;
    m_pCalendarView->clearAllNoteWidgets();
    m_localDbManager.clearLoginInfo();
    m_serverDbManager.clearLoginInfo();
    m_pDlgFind->clearContent();
    m_pDlgProgress->clearContent();
    updateControlState();
    showProgressBar(false);
    m_timerUserPulse.stop();
    m_bGetingUserPulse = false;

    G_Settings()->setLoginName("");
    G_Settings()->setLoginPassword("");
    G_Settings()->setAlwaysLogin(false);
    G_Settings()->save();
}

void CMainFrame::onClearLocalUserData()
{
//    int nUserLocalId = m_localDbManager.getUserLocalId();
//    m_localDbManager.deleteUser(nUserLocalId);
    stopSync();
    showProgressBar(false);
    m_localDbManager.clearLoginInfo();
    m_localDbManager.DeleteDB();
    m_localDbManager.OpenDB();
    m_localDbManager.addUser(m_strLoginUserNameTmp, m_strLoginPwdTmp, m_nLoginUserIdServer);
    m_serverDbManager.clearNoteItems();

    m_pCalendarView->refreshAllDayWidgets();
    m_pDlgFind->clearNoteItems();
    m_pDlgFind->clearContent();
    m_pDlgProgress->clearContent();

    m_nWorkingType = WORKING_TYPE_IDLE;
    updateControlState();
}

void CMainFrame::onNew()
{
    if(!checkLogin())
    {
        return;
    }

    QDate dtCurrent = m_pCalendarView->getSelectedDate();
    if(!dtCurrent.isValid())
        dtCurrent = QDate::currentDate();
    onNew(dtCurrent);
}

void CMainFrame::onNew(const QDate& date)
{
    if(!checkLogin())
    {
        return;
    }

    if(!m_pDlgEditNote)
    {
        m_pDlgEditNote = new CDlgEditNote(this);
        m_pMainLayout->addWidget(m_pDlgEditNote);
    }

    m_pDlgEditNote->setNoteItem(NULL);
    m_pDlgEditNote->setDate(date);
    m_pDlgEditNote->initControls();
    m_pDlgEditNote->show();
    m_pCalendarView->hide();

    updateControlState();

    /*
    CDlgEditNote dlg(NULL, QDateTime(date), this);
    if(QDialog::Accepted == dlg.exec())
    {
        CNoteItem* pNoteItem = dlg.detachNoteItem();
        addNoteItem(pNoteItem);
        m_pDlgFind->afterAddNoteItem();
        selectNoteItem(pNoteItem);
    }
    */
}

void CMainFrame::onEdit()
{
    onEdit(m_localDbManager.getSelectedNoteItem());
}

void CMainFrame::onEdit(CNoteItem* pNoteItem)
{
    if(isDlgNoteEditing())
        return;

    if(!checkLogin())
    {
        return;
    }

    if(!m_pDlgEditNote)
    {
        m_pDlgEditNote = new CDlgEditNote(this);
        m_pMainLayout->addWidget(m_pDlgEditNote);
    }

 //   CDlgEditNote dlg(pNoteItem, QDateTime(), this);

    m_pDlgEditNote->setNoteItem(pNoteItem);
    m_pDlgEditNote->initControls();
    m_pDlgEditNote->show();
    m_pCalendarView->hide();

    updateControlState();

    /*
    if(QDialog::Accepted == dlg.exec())
    {
        updateNoteItem(pNoteItem, dlg.detachNoteItem());
        selectNoteItem(pNoteItem);
    }
    */
}

void CMainFrame::onDelete()
{
    CNoteItem* pNoteItem = m_localDbManager.getSelectedNoteItem();
    if(pNoteItem)
    {
        onDelete(pNoteItem);
    }
}

void CMainFrame::onDelete(CNoteItem* pNoteItem)
{
    if(QMessageBox::Ok ==
            QMessageBox::question(this,
                                  tr("Delete"),
                                  tr("Do you want to delete the selected note item?"),
                                  QMessageBox::Ok, QMessageBox::Cancel))
    {
        deleteNoteItem(pNoteItem);
    }
}

void CMainFrame::onDlgFindShow(bool bShow)
{
    m_pActionFind->setChecked(bShow);
}

void CMainFrame::onTextContentShow(bool bShow)
{
    m_pActionContentText->setChecked(bShow);
}

void CMainFrame::onVisibilityShortNewNote(bool bShow)
{
    m_pActionShortNewNote->setChecked(bShow);
}

void CMainFrame::onDlgProgessShow(bool bShow)
{
    m_pActionDlgProgress->setChecked(bShow);
}

void CMainFrame::onOption()
{
    CConfigDialog dlg(this, G_Settings());
    if(QDialog::Accepted == dlg.exec())
    {
        m_pCalendarView->refreshWeekHeader();
        m_pCalendarView->refreshAllDayWidgets();
        m_pDlgFind->onFind();
    }
}

void CMainFrame::onAbout()
{
    char DateStr[]=__DATE__;
    char TimeStr[]=__TIME__;

    QString strDateTime = QString(DateStr) + QString(" ") + QString(TimeStr);
    QString strNewVersion = tr("The current version is latest.");

    QString strLatestVersion = G_Settings()->getNewVersion();
    QString strCurrentVer(STR_VERSION);

    if(!strLatestVersion.isEmpty() &&
            !G_Settings()->getDownloadUrl().isEmpty() &&
            !G_Settings()->getChangeInfo().isEmpty())
    {
        int nMajorVerLatest = 0;
        int nMajorVerCurrent = 0;
        int nMinorVerLatest = 0;
        int nMinorVerCurrent = 0;

        int nPosLatest = strLatestVersion.indexOf('.');
        int nPosCurrent = strCurrentVer.indexOf('.');
        if(nPosLatest >= 0)
        {
            nMajorVerLatest = strLatestVersion.left(nPosLatest).toInt();
            nMinorVerLatest = strLatestVersion.mid(nPosLatest + 1).toInt();
        }
        else
        {
            nMajorVerLatest = strLatestVersion.toInt();
        }

        if(nPosCurrent >= 0)
        {
            nMajorVerCurrent = strCurrentVer.left(nPosCurrent).toInt();
            nMinorVerCurrent = strCurrentVer.mid(nPosCurrent + 1).toInt();
        }
        else
        {
            nMajorVerCurrent = strCurrentVer.toInt();
        }

        if(nMajorVerLatest > nMajorVerCurrent ||
                (nMajorVerLatest == nMajorVerCurrent && nMinorVerLatest > nMinorVerCurrent))
        {
#ifdef Q_WS_MAC
            strNewVersion = QString("<a href='%1' title='%2'>The latest version is %3</a>.")
                    .arg(URL_DOWNLOAD_MAC)
                    .arg(G_Settings()->getChangeInfo())
                    .arg(strLatestVersion);
#else
            strNewVersion = QString("<a href='%1' title='%2'>The latest version is %3</a>.")
                    .arg(G_Settings()->getDownloadUrl())
                    .arg(G_Settings()->getChangeInfo())
                    .arg(strLatestVersion);
#endif
        }
    }

    QMessageBox::about(this, tr("About Notee"),
            QString(tr("<h2>Notee %1</h2>"
               "<p>Build at %2</p>"
             "<p>Please visit the following link to access web Notee.</p>"
             "<p><a href=\"http://notee.coodesoft.com/\">http://notee.coodesoft.com/</a></p>"
             "<p>%3</p>"
            "<p>&copy; 2012-2013 <a href=\"http://www.coodesoft.com/\">Coode Software</a></p>"
                       )).arg(STR_VERSION).arg(strDateTime).arg(strNewVersion));
}

void CMainFrame::onExit()
{
    QApplication::exit();
}

void CMainFrame::onItemSelectionChanged()
{
    if(m_bSender)
        return;

    m_bSender = true;

    CNoteItem* pNoteItem = m_localDbManager.getSelectedNoteItem();
    if(pNoteItem)
        selectNoteItem(pNoteItem, false, true);

    m_bSender = false;
}

void CMainFrame::onClickShortNewNote()
{
    if(m_pActionShortNewNote->isChecked())
        m_pDockWidgetShortNewNote->show();
    else
        m_pDockWidgetShortNewNote->hide();
}

void CMainFrame::onFind()
{
    if(m_pActionFind->isChecked())
        m_pDockWidgetFind->show();
    else
        m_pDockWidgetFind->hide();
}

void CMainFrame::onContentText()
{
    if(m_pActionContentText->isChecked())
        m_pDockWidgetContent->show();
    else
        m_pDockWidgetContent->hide();
}

void CMainFrame::onDlgProgress()
{
    if(m_pActionDlgProgress->isChecked())
        m_pDockWidgetProgress->show();
    else
        m_pDockWidgetProgress->hide();
}

void CMainFrame::onToday()
{
    m_pCalendarView->setMonth(QDate::currentDate());
    if(m_pDlgFind->getFindText().isEmpty())
    {
        m_pDlgFind->onCurrent();
    }
    m_pActionToday->setEnabled(false);
}

void CMainFrame::onPrevMonth()
{
    m_pCalendarView->prevMonth(isLoginOnLocal());
    if(m_pDlgFind->needRefreshAfterChangeMonth())
    {
        m_pDlgFind->onCurrent();
    }
    m_pActionToday->setEnabled(!m_pCalendarView->isCurrentMonth());
}

void CMainFrame::onNextMonth()
{
    m_pCalendarView->nextMonth(isLoginOnLocal());
    if(m_pDlgFind->needRefreshAfterChangeMonth())
    {
        m_pDlgFind->onCurrent();
    }
    m_pActionToday->setEnabled(!m_pCalendarView->isCurrentMonth());
}

void CMainFrame::checkAllLocalNoteStatus()
{
    m_localDbManager.loadAllNoteItems();
    QList<CNoteItem*>& lstNoteItems = m_localDbManager.getNoteItemList();
    foreach(CNoteItem* pNoteItem, lstNoteItems)
    {
        if(!pNoteItem->isWholeNormalState())
        {
            addProcessingNoteItem(pNoteItem);
        }
    }
}

void CMainFrame::checkClassifyStringServer(const QString& strClassifyServer)
{
    if(strClassifyServer.isEmpty())
        return;

    if(m_localDbManager.getClassifyLabelsAtServer() != strClassifyServer)
    {
        m_localDbManager.updateClassifyLabels(strClassifyServer, strClassifyServer);

        refreshTabClassifyStrings();
    }
    else if(m_localDbManager.getClassifyLabelsAtServer() != m_localDbManager.getClassifyLabels())
    {
        updateClassifyString(m_localDbManager.getClassifyLabels());
    }
}

void CMainFrame::updateClassifyString(const QString& str)
{
    m_serverDbManager.updateClassifyString(str);
}

void CMainFrame::refreshTabClassifyStrings()
{
    m_pDlgFind->updateTabLabels(m_localDbManager.getClassifyTypesStringList());
}


void CMainFrame::onSyncProgressBegin()
{
    setProgressBarPos(0);
    showProgressBar(true);
    m_pStatusBar->showMessage("Synchronize with server...");
}

void CMainFrame::onSyncProgressFinished()
{
    checkDataUpdateAtString();
    m_nWorkingType = WORKING_TYPE_IDLE;
    m_pStatusBar->showMessage("");
    showProgressBar(false);

    doProcessAction();
}

void CMainFrame::onSyncProgressMax(int nPosMax)
{
    setProgressBarRange(0, nPosMax);
}

void CMainFrame::onSyncProgressPos(int nPos)
{
    setProgressBarPos(nPos);
}

void CMainFrame::onSyncError(const QString& str)
{
    m_pStatusBar->showMessage(str);
}

void CMainFrame::onSynThreadNetworkReplyFinished()
{
    QNetworkReply* pNetworkReply = qobject_cast<QNetworkReply*>(sender());
    if(!pNetworkReply)
        return;

    m_arrBufferSync = pNetworkReply->readAll();
}

void CMainFrame::onOutputString(QString str)
{
    addOutputString(str);
}

void CMainFrame::onTimeOutUserPulse()
{
    if(hasNetReplyWorking())
        return;

    if(m_serverDbManager.getUserId() >= 0 && !m_bGetingUserPulse) {
        m_bGetingUserPulse = true;
        m_serverDbManager.userPulse();
    }
}

void CMainFrame::autoLogin()
{
    // login
    if(G_Settings()->isAlwaysLogin())
    {
        m_nWorkingType = WORKING_TYPE_LOGIN;
        m_strLoginUserNameTmp = G_Settings()->getLoginName();
        m_strLoginPwdTmp = G_Settings()->getLoginPassword();
        m_serverDbManager.login(m_strLoginUserNameTmp, m_strLoginPwdTmp);
        m_pStatusBar->showMessage(tr("Logining..."));

        if(m_localDbManager.login(G_Settings()->getLoginName(), G_Settings()->getLoginPassword()))
        {
            m_localDbManager.loadAllNoteItems();
            m_pCalendarView->refreshAllDayWidgets();
            m_pDlgFind->onCurrent();

            refreshTabClassifyStrings();
        }

        m_pStatusBar->showMessage(tr("Login on server..."));
    }

    updateControlState();
}

void CMainFrame::updateControlState()
{
    bool bCalendarViewShow = m_pCalendarView->isVisible();

    bool bServerLogin = isLoginOnServer();
    bool bLocalLogin = isLoginOnLocal();
    bool bIsIdle = m_nWorkingType == WORKING_TYPE_IDLE;
    bool bLogin = bServerLogin || bLocalLogin;

    bool bCanLoginOrRegister = !bLogin &&
            (m_nWorkingType == WORKING_TYPE_LOGIN ||
             m_nWorkingType == WORKING_TYPE_IDLE);

    m_pActionRegister->setEnabled(bCalendarViewShow && bCanLoginOrRegister);
    m_pActionLogin->setEnabled(bCalendarViewShow && bCanLoginOrRegister);
    m_pActionLogout->setEnabled(bCalendarViewShow && bLogin);
    m_pActionForgotPwd->setEnabled(bCalendarViewShow);
    m_pActionNew->setEnabled(bCalendarViewShow && bLogin);
    m_pActionEdit->setEnabled(bCalendarViewShow && bLogin && bIsIdle && m_localDbManager.getSelectedNoteItem());
    m_pActionDelete->setEnabled(bCalendarViewShow && bLogin && bIsIdle && m_localDbManager.getSelectedNoteItem());
    m_pActionSync->setEnabled(bCalendarViewShow && bLogin);

    if(m_pActionSync->isEnabled())
    {
        if(bIsIdle)
        {
            m_pActionSync->setText(tr("Synchronize"));
        }
        else
        {
            m_pActionSync->setText(tr("Cancel synchronize"));
        }
    }

    if(bServerLogin)
    {
        setWindowTitle(QString("Notee (%1)")
                       .arg(m_serverDbManager.getUserName()));
    }
    else if(bLocalLogin)
    {
        setWindowTitle(QString("Notee (%1)(%2)")
                       .arg(tr("local"))
                       .arg(m_localDbManager.getUserName()));
    }
    else
    {
        setWindowTitle(QString("Notee (%1)")
                       .arg(tr("disconnect")));
    }
}

void CMainFrame::addNoteItem(CNoteItem* pNoteItem)
{
    insertNoteItemM(pNoteItem);
    refreshDayWidget(pNoteItem->getSetAt().date());

    addProcessingNoteItem(pNoteItem);
}

void CMainFrame::updateNoteItem(CNoteItem* pNoteItemOrg, CNoteItem* pNoteItem)
{
    QDateTime dtOrg = pNoteItemOrg->getSetAt();

    // set *pNoteItemOrg = *pNoteItem
    updateNoteItemM(pNoteItem, false, false, false);

    refreshDayWidget(pNoteItem->getSetAt().date());
    if(dtOrg != pNoteItem->getSetAt())
    {
        refreshDayWidget(dtOrg.date());
    }

    addProcessingNoteItem(pNoteItemOrg);
}

void CMainFrame::deleteNoteItem(CNoteItem* pNoteItem)
{
    // need to delete item on server
    if(pNoteItem->getId() > 0)
    {
        pNoteItem->setState(STATE_DELETE);
        updateNoteItemM(pNoteItem, true);

        refreshDayWidget(pNoteItem->getSetAt().date());

        addProcessingNoteItem(pNoteItem);

        updateLocalDataUpdateAtString();
    }
    // it isn't on server
    else
    {
        deleteNoteItemM(pNoteItem);

        refreshDayWidget(pNoteItem->getSetAt().date());
    }
}

void CMainFrame::selectNoteItem(CNoteItem* pNoteItem, bool bOnView, bool bOnFind)
{
    if(m_pSelectedNoteItem == pNoteItem)
        return;

    m_bSender = true;

    m_pSelectedNoteItem = pNoteItem;

    if(bOnView)
        m_pCalendarView->selectNoteItem(pNoteItem);

    if(bOnFind)
        m_pDlgFind->selectNoteItem(pNoteItem);

    if(pNoteItem)
        m_pTextContent->setHtml(QString::fromUtf8(pNoteItem->getContent()));
 //       m_pTextContent->setHtml(QString::fromAscii(pNoteItem->getContent()));
    else
        m_pTextContent->setHtml("");

    updateControlState();

    m_bSender = false;
}

void CMainFrame::afterAddNoteItem()
{
    m_pDlgFind->afterAddNoteItem();
}

void CMainFrame::showCalendarView()
{
    m_pCalendarView->show();
    if(m_pDlgRegister && m_pDlgRegister->isVisible())
        m_pDlgRegister->hide();
    if(m_pDlgLogin && m_pDlgLogin->isVisible())
        m_pDlgLogin->hide();
    if(m_pDlgEditNote && m_pDlgEditNote->isVisible())
        m_pDlgEditNote->hide();
    if(m_pDlgForgotPwd && m_pDlgForgotPwd->isVisible())
        m_pDlgForgotPwd->hide();
}

QList<CNoteItem*> CMainFrame::getNotesByString(const QString& strText)
{
    return m_localDbManager.getNotesByString(strText);
}

QList<CNoteItem*> CMainFrame::getCurrentNotes()
{
    return m_pCalendarView->getNoteItemList();
}

void CMainFrame::refreshDayWidget(const QDate& dt)
{
    m_pCalendarView->refreshDayWidget(dt);
}

void CMainFrame::setProgressBarRange(int nMin, int nMax)
{
    m_pProgressBar->setRange(nMin, nMax);
}

void CMainFrame::setProgressBarPos(int nPos)
{
    m_pProgressBar->setValue(nPos);
    if(nPos >= m_pProgressBar->minimum() && nPos < m_pProgressBar->maximum())
        setStatusTip(tr("Synchronizing from server..."));
    else
        setStatusTip("");

    if(nPos >= m_pProgressBar->maximum())
        m_pProgressBar->hide();

    QApplication::processEvents();
}

void CMainFrame::showProgressBar(bool bShow)
{
    if(bShow)
    {
        m_pProgressBar->show();
        updateControlState();
    }
    else
    {
        m_pProgressBar->hide();
        m_pStatusBar->showMessage("");
        updateControlState();
    }
}

bool CMainFrame::isDlgNoteEditing() const
{
    return m_pDlgEditNote && m_pDlgEditNote->isVisible();
}

// multi-thread
void CMainFrame::insertNoteItemM(CNoteItem* pNoteItem)
{
    m_mutexModifyNote.lock();
    m_localDbManager.addData(pNoteItem);
    emit emitRefreshDayWidget(pNoteItem->getSetAt().date());
    emit emitInsertNoteItem(pNoteItem);
    m_mutexModifyNote.unlock();
}

void CMainFrame::updateNoteItemM(CNoteItem* pNoteItem, bool bOnlyState, bool bByServerId, bool bWithoutConten)
{
    m_mutexModifyNote.lock();
    m_localDbManager.updateData(pNoteItem, bOnlyState, bByServerId, bWithoutConten);
    if(pNoteItem->getSetAtBk().isValid() && pNoteItem->getSetAt() != pNoteItem->getSetAtBk())
        emit emitRefreshDayWidget(pNoteItem->getSetAtBk().date());
    emit emitRefreshDayWidget(pNoteItem->getSetAt().date());
    m_mutexModifyNote.unlock();
}

void CMainFrame::deleteNoteItemM(CNoteItem* pNoteItem)
{
    m_mutexModifyNote.lock();
    pNoteItem->setState(STATE_DELETE);
    QDateTime dtNoteItem = pNoteItem->getSetAt();
    m_localDbManager.deleteNoteItem(pNoteItem->getLocalId());
    emit emitRefreshDayWidget(dtNoteItem.date());
    m_mutexModifyNote.unlock();
}

void CMainFrame::onEmitRefreshDayWidget(const QDate& dt)
{
    refreshDayWidget(dt);
}

void CMainFrame::onEmitInsertNoteItem(CNoteItem* pNoteItem)
{
    m_pDlgFind->refresh();
}

void CMainFrame::onShortNewNote(QString str)
{
    QDate dtCurrent = m_pCalendarView->getSelectedDate();
    if(!dtCurrent.isValid())
        dtCurrent = QDate::currentDate();
    QDateTime dtime;
    dtime.setDate(dtCurrent);

    CNoteItem* pNoteItemNew = new CNoteItem();
    pNoteItemNew->setTitle(str);
    pNoteItemNew->setSetAt(dtime);
    pNoteItemNew->setState(STATE_NEW);

    addNoteItem(pNoteItemNew);
    afterAddNoteItem();
    selectNoteItem(pNoteItemNew);

    updateLocalDataUpdateAtString();
    updateControlState();
    doProcessAction();
}
