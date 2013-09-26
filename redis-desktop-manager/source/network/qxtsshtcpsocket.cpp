
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

#include "qxtsshtcpsocket.h"
#include "qxtsshchannel_p.h"


/*!
    \class QxtSshTcpSocket
    \inmodule QxtNetwork
    \brief The QxtSshTcpSocket class provides a TCP socket tunneled over an SSH connection

    QxtSshTcpSocket enables an SSH client to establish a TCP connection between the SSH server and
    a remote host. Traffic over this TCP connection is tunneled through the channel.

    Note that traffic between the SSH server and the remote host is unencrypted. Only
    communication between QxtSshClient and the SSH server is encrypted.

    QxtSshTcpSocket objects are created using the QxtSshClient::openTcpSocket() method.
*/

/*!
 *
 */
QxtSshTcpSocket::QxtSshTcpSocket(QxtSshClient * parent)
    :QxtSshChannel(parent)
{
}
