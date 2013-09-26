
/****************************************************************************
** Copyright (c) 2006 - 2011, the LibQxt project.
** See the Qxt AUTHORS file for a list of authors and copyright holders.
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the LibQxt project nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** <http://libqxt.org>  <foundation@libqxt.org>
*****************************************************************************/

#include "qxtsshprocess.h"
#include "qxtsshchannel_p.h"


/*!
    \class QxtSshProcess
    \inmodule QxtNetwork
    \brief The QxtSshProcess class allows communication with a process invoked on an SSH server

    QxtSshProcess is an I/O device analogous to QProcess. It can invoke a command or SSH subsystem on
    the SSH server; the stdin, stdout, and stderr channels of the process are redirected through the
    channel.

    QxtSshProcess objects are created using the QxtSshClient::openProcessChannel() method.
*/

/*!
 * \enum QxtSshProcess::TerminalType 
 * \value VanillaTerminal   A terminal with no special features
 * \value Vt102Terminal     A terminal that understands VT102 command codes
 * \value AnsiTerminal      A terminal that understands ANSI command codes
 */

/*! 
 * \fn QxtSshProcess::finished ( int exitCode )
 *
 * This signal is emitted when the process on the SSH server finishes. The parameter is the exit code of
 * the process. After the process has finished, the read buffer QxtSshProcess will still contain any
 * data the process may have written before terminating.
 *
 * Another process or subsystem may be invoked on the same channel after the first process terminates.
 */

/*!
 * \fn QxtSshProcess::started ()
 *
 * This signal is emitted by QxtSshProcess when the remote process has started.
 */

QxtSshProcess::QxtSshProcess(QxtSshClient * parent)
    :QxtSshChannel(parent)
{
    d->openSession();
    connect(this,SIGNAL(connected()),this,SIGNAL(started()));
}
/*!
 * Starts a login shell on the SSH server.
 *
 * If there is already a process running on this channel, the behavior is undefined.
 */
void QxtSshProcess::startShell(){
    d->startShell();
}
/*!
 * Invokes a shell command on the SSH server.
 *
 * If there is already a process running on this channel, the behavior is undefined.
 */
void QxtSshProcess::start(const QString & cmd){
    d->start(cmd);
}
/*!
 * Requests that a PTY be allocated for this channel on the remote host.
 * 
 * This function must be invoked before starting the process that requires it.
 */
void QxtSshProcess::requestPty(TerminalType term){
    switch (term){
        case VanillaTerminal:
            d->requestPty("vanilla");
            break;;
        case Vt102Terminal:
            d->requestPty("vt102");
            break;;
        case AnsiTerminal:
            d->requestPty("ansi");
            break;;
    }
}
