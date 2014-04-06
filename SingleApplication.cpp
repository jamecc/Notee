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

#include <QTimer>
#include <QByteArray>
#include "SingleApplication.h"


CSingleApplication::CSingleApplication(int &argc, char *argv[], const QString& uniqueKey) :
    QApplication(argc, argv)
{
    m_sharedMemory.setKey(uniqueKey);
    if (m_sharedMemory.attach())
        m_isRunning = true;
    else
    {
        m_isRunning = false;
        // attach data to shared memory.
        QByteArray byteArray("0"); // default value to note that no message is available.
        if (!m_sharedMemory.create(byteArray.size()))
        {
            qDebug("Unable to create single instance.");
            return;
        }
        m_sharedMemory.lock();
        char *to = (char*)m_sharedMemory.data();
        const char *from = byteArray.data();
        memcpy(to, from, qMin(m_sharedMemory.size(), byteArray.size()));
        m_sharedMemory.unlock();
        // start checking for messages of other instances.
        QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(onCheckForMessage()));
        timer->start(1000);
    }
}

void CSingleApplication::onCheckForMessage()
{
    m_sharedMemory.lock();
    QByteArray byteArray = QByteArray((char*)m_sharedMemory.constData(), m_sharedMemory.size());
    m_sharedMemory.unlock();
    if (byteArray.left(1) == "0")
        return;
    byteArray.remove(0, 1);
    QString message = QString::fromUtf8(byteArray.constData());
    emit messageAvailable(message);
    // remove message from shared memory.
    byteArray = "0";
    m_sharedMemory.lock();
    char *to = (char*)m_sharedMemory.data();
    const char *from = byteArray.data();
    memcpy(to, from, qMin(m_sharedMemory.size(), byteArray.size()));
    m_sharedMemory.unlock();
}

bool CSingleApplication::isRunning()
{
    return m_isRunning;
}

bool CSingleApplication::sendMessage(const QString &message)
{
    if (!m_isRunning)
        return false;
    QByteArray byteArray("1");
    byteArray.append(message.toUtf8());
    byteArray.append('\0'); // < should be as char here, not a string!
    m_sharedMemory.lock();
    char *to = (char*)m_sharedMemory.data();
    const char *from = byteArray.data();
    memcpy(to, from, qMin(m_sharedMemory.size(), byteArray.size()));
    m_sharedMemory.unlock();
    return true;
}
