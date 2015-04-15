
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

#ifndef QXT_SSH_CLIENT_H
#define QXT_SSH_CLIENT_H

#include <QObject>
#include <QList>

class QxtSshKey{
 public:
    enum Type{
        UnknownType,
        Rsa,
        Dss
    };
    QByteArray hash;
    QByteArray key;
    Type type;
};


class QxtSshProcess;
class QxtSshTcpSocket;
class QxtSshClientPrivate;
class QxtSshClient : public QObject{
    Q_OBJECT
public:
    enum AuthenticationMethod{
        PasswordAuthentication,
        PublicKeyAuthentication
    };
    enum KnownHostsFormat{
        OpenSslFormat
    };
    enum Error{
        AuthenticationError,
        HostKeyUnknownError,
        HostKeyInvalidError,
        HostKeyMismatchError,
        ConnectionRefusedError,
        UnexpectedShutdownError,
        HostNotFoundError,
        SocketError,
        UnknownError
    };

    QxtSshClient(QObject * parent=0);
    ~QxtSshClient();

    void connectToHost(const QString & username,const QString & hostname,
                       int port = 22, bool useSystemClient = false);
    void disconnectFromHost ();
    void resetState();
    void setPassphrase(const QString & pass);
    void setKeyFiles(const QString & publicKey,const QString & privateKey);

    bool loadKnownHosts(const QString & file,KnownHostsFormat c = OpenSslFormat);
    bool saveKnownHosts(const QString & file,KnownHostsFormat c = OpenSslFormat) const;
    bool addKnownHost(const QString & hostname,const QxtSshKey & key);

    QxtSshKey hostKey() const;
    QString   hostName() const;

    QxtSshProcess * openProcessChannel();
    QxtSshTcpSocket * openTcpSocket(const QString & hostName,quint16 port);
signals:
    void connected();
    void disconnected();
    void error (QxtSshClient::Error error);
    void authenticationRequired(QList<QxtSshClient::AuthenticationMethod> availableMethods);
private:
    QxtSshClientPrivate * d;
    friend class QxtSshClientPrivate;
    friend class QxtSshChannelPrivate;
};

#include <QDebug>
inline QDebug operator<< (QDebug d, const QxtSshClient::AuthenticationMethod  & m){
    switch(m){
        case QxtSshClient::PasswordAuthentication:
            d<<"QxtSshClient::PasswordAuthentication";
            break;
        case QxtSshClient::PublicKeyAuthentication:
            d<<"QxtSshClient::PublicKeyAuthentication";
            break;
    }
    return d;
}
inline QDebug operator<< (QDebug d, const QxtSshKey::Type  & m){
    switch(m){
        case QxtSshKey::Dss:
            d<<"QxtSshKey::Dss";
            break;
        case QxtSshKey::Rsa:
            d<<"QxtSshKey::Rsa";
            break;
        case QxtSshKey::UnknownType:
            d<<"QxtSshKey::UnknownType";
            break;
    }
    return d;
}



#endif
