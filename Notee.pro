#****************************************************************************
#
# Copyright (C) 2012 Coode Software
# Contact: http://www.coodesoft.com/
#
# This file is part of Notee.
#
# GNU Lesser General Public License Usage
# Alternatively, this file may be used under the terms of the GNU Lesser
# General Public License version 2.1 as published by the Free Software
# Foundation and appearing in the file LICENSE.LGPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU Lesser General Public License version 2.1 requirements
# will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl.html.
#
#
#**************************************************************************/

TEMPLATE = app
TARGET = Notee

QT += core gui sql network xml

#DEFINES += QT_LARGEFILE_SUPPORT QT_SQL_LIB QT_NETWORK_LIB
#DEFINES += NOTEE_DEBUG

CONFIG(release) {
    DEFINES += NOTEE_RELEASE
} else {
    DEFINES += NOTEE_DEBUG
}

HEADERS += AttachmentItem.h \
    CommonFunctions.h \
    NoteItem.h \
    predefine.h \
    stdafx.h \
    MainFrame.h \
    DatabaseManager.h \
    NetworkManager.h \
    JsonParser.h \
    DlgLogin.h \
    DlgRefreshList.h \
    DlgRegister.h \
    Settings.h \
    GraphicsViewLayoutScene.h \
    CalendarScene.h \
    EntityItem.h \
    GraphicsHsItem.h \
    GraphicsNoteItem.h \
    GraphicsDayItem.h \
    GraphicsWeekHeaderItem.h \
    GraphicsTitleItem.h \
    HsWidget.h \
    NoteItemWidget.h \
    DayWidget.h \
    DlgEditNote.h \
    CalendarView.h \
    AttachmentListWidget.h \
    ConfigPages.h \
    ConfigDialog.h \
    DlgFind.h \
    SingleApplication.h \
    ImageTextEdit.h \
    UnicodeConvertString.h \
    DCTableWidget.h \
    ArrowWidget.h \
    HtmlHighlighter.h \
    SyncThread.h \
    SenderThreader.h \
    TabWidget.h \
    MyCleanlooksStyle.h \
    DlgProgress.h \
    FileUpload.h \
    FileDownload.h \
    TableProgress.h \
    TextContent.h \
    TextOutput.h \
    DlgShortNewNote.h \
    IconDockWidget.h \
    DlgForgotPwd.h \
    DlgBallonTip.h
SOURCES += AttachmentItem.cpp \
    CalendarScene.cpp \
    CommonFunctions.cpp \
    DatabaseManager.cpp \
    DlgLogin.cpp \
    DlgRefreshList.cpp \
    DlgRegister.cpp \
    main.cpp \
    NoteItem.cpp \
    stdafx.cpp \
    MainFrame.cpp \
    NetworkManager.cpp \
    JsonParser.cpp \
    Settings.cpp \
    GraphicsViewLayoutScene.cpp \
    EntityItem.cpp \
    GraphicsHsItem.cpp \
    GraphicsNoteItem.cpp \
    GraphicsDayItem.cpp \
    GraphicsWeekHeaderItem.cpp \
    GraphicsTitleItem.cpp \
    HsWidget.cpp \
    DayWidget.cpp \
    NoteItemWidget.cpp \
    DlgEditNote.cpp \
    CalendarView.cpp \
    AttachmentListWidget.cpp \
    ConfigPages.cpp \
    ConfigDialog.cpp \
    DlgFind.cpp \
    DCTableWidget.cpp \
    SingleApplication.cpp \
    ImageTextEdit.cpp \
    UnicodeConvertString.cpp \
    ArrowWidget.cpp \
    HtmlHighlighter.cpp \
    SyncThread.cpp \
    SenderThreader.cpp \
    TabWidget.cpp \
    MyCleanlooksStyle.cpp \
    DlgProgress.cpp \
    FileUpload.cpp \
    FileDownload.cpp \
    TableProgress.cpp \
    TextContent.cpp \
    TextOutput.cpp \
    MainFrame1.cpp \
    DlgShortNewNote.cpp \
    IconDockWidget.cpp \
    DlgForgotPwd.cpp \
    DlgBallonTip.cpp

RESOURCES += ./notee.qrc

win32:RC_FILE = notee.rc
mac:ICON = notee.icns

FORMS += \
    DlgForgotPwd.ui \
    DlgBallonTip.ui

