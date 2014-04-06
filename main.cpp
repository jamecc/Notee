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
#include "SingleApplication.h"
#include "Settings.h"
#include "MainFrame.h"

#include "MyCleanlooksStyle.h"


CSettings* g_pSettings = NULL;
CMainFrame* g_pMainFrame = NULL;

CSettings* G_Settings()
{
    return g_pSettings;
}

CMainFrame* G_MainFrame()
{
    return g_pMainFrame;
}

/////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    // app
    CSingleApplication a(argc, argv);

    if(a.isRunning())
    {
        QMessageBox::information(NULL, QCoreApplication::tr("Info"), QCoreApplication::tr("Notee has run."));
        return 0;
    }

    a.setStyle(new CMyCleanlooksStyle);
    
    // init
    Q_INIT_RESOURCE(notee);

    CSettings appSettings("Coode Software", "Notee");
    g_pSettings = &appSettings;

    QTextCodec* pTextCode = QTextCodec::codecForName("UTF-8"); // GB18030-0
    QTextCodec::setCodecForTr(pTextCode);
    QTextCodec::setCodecForLocale(pTextCode);
    QTextCodec::setCodecForCStrings(pTextCode);

    QCoreApplication::setOrganizationName( "Coode Software" );
    QCoreApplication::setOrganizationDomain( "coodesoft.com" );
    QCoreApplication::setApplicationName( "Notee" );
    QCoreApplication::setApplicationVersion(STR_VERSION);

    QApplication::addLibraryPath(QApplication::applicationDirPath());

    // main frame
    CMainFrame w(NULL, Qt::Widget | Qt::WindowMinMaxButtonsHint);
    g_pMainFrame = &w;

    // connect message queue to the main window.
//    QObject::connect(&app, SIGNAL(messageAvailable(QString)), &w, SLOT(receiveMessage(QString)));

    w.showMaximized();
    return a.exec();
}
