
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

#include "qxtsshchannel_p.h"
#include "qxtsshprocess.h"
#include "qxtsshtcpsocket.h"
#include <QTimer>

/*!
    \class QxtSshClient
    \inmodule QxtNetwork
    \brief The QxtSshClient class implements a Secure Shell client

    QxtSshClient allows connecting to any standard SSH server.

    It provides facilities for password authentication or public key authentication, and
    provides methods to open a shell or a TCP socket on the remote host.

    The passphrase can be set before connecting or it can be provided in response to the
    authenticationRequired() signal. This allows the password to be stored in advance or
    prompted for only when needed.

    QxtSshClient is based on the third-party library libssh2 (http://www.libssh2.org/)
    provided under the following license:
   \code
    * Copyright (c) 2004-2007 Sara Golemon <sarag@libssh2.org>
    * Copyright (c) 2005,2006 Mikhail Gusarov <dottedmag@dottedmag.net>
    * Copyright (c) 2006-2007 The Written Word, Inc.
    * Copyright (c) 2007 Eli Fant <elifantu@mail.ru>
    * Copyright (c) 2009 Daniel Stenberg
    * Copyright (C) 2008, 2009 Simon Josefsson
    * All rights reserved.
    *
    * Redistribution and use in source and binary forms,
    * with or without modification, are permitted provided
    * that the following conditions are met:
    *
    *   Redistributions of source code must retain the above
    *   copyright notice, this list of conditions and the
    *   following disclaimer.
    *
    *   Redistributions in binary form must reproduce the above
    *   copyright notice, this list of conditions and the following
    *   disclaimer in the documentation and/or other materials
    *   provided with the distribution.
    *
    *   Neither the name of the copyright holder nor the names
    *   of any other contributors may be used to endorse or
    *   promote products derived from this software without
    *   specific prior written permission.
    *
    * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
    * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
    * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
    * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
    * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
    * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
    * OF SUCH DAMAGE.
    \endcode
*/

/*! \enum QxtSshClient::AuthenticationMethod
 *
 * \value PasswordAuthentication    Authenticate using a password
 * \value PublicKeyAuthentication   Authenticate using a public key
 */

/*! \enum QxtSshClient::Error 
 
   \value AuthenticationError       An error occurred while authenticating.
   \value HostKeyUnknownError       The host key is unknown.
   \value HostKeyInvalidError       The host key is invalid.
   \value HostKeyMismatchError      The host key does not match the key recorded in the known hosts.
   \value ConnectionRefusedError    The connection was refused.
   \value UnexpectedShutdownError   The connection was unexpectedly shut down.
   \value HostNotFoundError         The host could not be found.
   \value SocketError               An error occurred on the underlying socket.
   \value UnknownError              An unknown error occurred.
 */

/*! \enum QxtSshClient::KnownHostsFormat
 *
 * \value OpenSslFormat             Use the .ssh/known_hosts file format.
 */

/*! 
 * \fn QxtSshClient::connected()
 *
 * This signal is emitted when a connection to the SSH server has been successfully established.
 */

/*!
 * \fn QxtSshClient::disconnected()
 *
 * This signal is emitted when the connection to the SSH server has been closed.
 */

/*!
 * \fn QxtSshClient::error ( QxtSshClient::Error error ) 
 *
 * This signal is emitted after an error occurs. The error parameter describes the type of error that occurred.
 */

/*!
 * Constructs a new QxtSshClient with the specified parent.
 */
QxtSshClient::QxtSshClient(QObject * parent)
    :QObject(parent)
    ,d(new QxtSshClientPrivate){
    d->p=this;
}

/*!
 * Destroys the QxtSshClient object.
 */
QxtSshClient::~QxtSshClient(){
    delete d;
}
/*!
 * Attempts to make a connection to host on the given port as the specified user.
 *
 * QxtSshClient will attempt to authenticate first using the public key, if
 * provided, then using the password, if provided. If neither of these methods
 * succeed in authenticating, the authenticationRequired() signal will be emitted.
 * The public key and/or password can be changed in a slot directly connected to
 * this signal. After the signal has resolved, the public key and password will be
 * tried again.
 *
 * The connected() signal will be emitted when the connection is complete. At any
 * point, QxtSshClient can emit error() to signal that an error occurred.
 *
 * The host parameter can be an IP address or a host name. Host names will be
 * resolved by the server.
 */
void QxtSshClient::connectToHost(const QString & user, const QString & host, int port, bool useSystemClient){
    d->d_hostName=host;
    d->d_userName=user;
    d->d_port=port;
    d->d_state=0;
    d->d_useSystemClient=useSystemClient;
    d->connectToHost(host,port);
}


void QxtSshClient::resetState()
{
    d->d_state=0;
    d->abort();
}

/*!
 * Disconnects the current SSH connection.
 *
 * Any open channels will be closed. When the connection has been closed, the
 * disconnected() signal will be emitted.
 */
void QxtSshClient::disconnectFromHost() {
    d->d_reset();
}

/*!
 * Sets the password for the user. This password is also used for the passphrase of the private key.
 */
void QxtSshClient::setPassphrase(const QString & pass){
    d->d_failedMethods.removeAll(QxtSshClient::PasswordAuthentication);
    d->d_failedMethods.removeAll(QxtSshClient::PublicKeyAuthentication);
    d->d_passphrase=pass;
    if(d->d_state>1){
        QTimer::singleShot(0,d,SLOT(d_readyRead()));
    }
}

/*!
 * Sets a public and private key pair to use to authenticate to the SSH server.
 *
 * If the private key is secured with a passphrase, the passphrase set with setPassphrase() will be used.
 */
void QxtSshClient::setKeyFiles(const QString & publicKey,const QString & privateKey){
    d->d_failedMethods.removeAll(QxtSshClient::PublicKeyAuthentication);
    d->d_publicKey=publicKey;
    d->d_privateKey=privateKey;
    if(d->d_state>1){
        QTimer::singleShot(0,d,SLOT(d_readyRead()));
    }
}
/*!
 * Loads a list of known host signatures from a file.
 *
 * This list is used for host key verification during connection.
 */
bool QxtSshClient::loadKnownHosts(const QString & file,KnownHostsFormat c){
    Q_UNUSED(c);
    return (libssh2_knownhost_readfile(d->d_knownHosts, qPrintable(file),
                                      LIBSSH2_KNOWNHOST_FILE_OPENSSH)==0);
}

/*!
 * Saves the current list of known host signatures to a file.
 *
 * \sa loadKnownHosts 
 */
bool QxtSshClient::saveKnownHosts(const QString & file,KnownHostsFormat c) const{
    Q_UNUSED(c);
    return (libssh2_knownhost_writefile(d->d_knownHosts, qPrintable(file),
                                LIBSSH2_KNOWNHOST_FILE_OPENSSH)==0);
}

/*!
 * Adds a host key to the list of known host signatures.
 *
 * This list is used for host key verification during connection.
 *
 * \sa loadKnownHosts
 * \sa saveKnownHosts
 */
bool QxtSshClient::addKnownHost(const QString & hostname,const QxtSshKey & key){
    int typemask=LIBSSH2_KNOWNHOST_TYPE_PLAIN | LIBSSH2_KNOWNHOST_KEYENC_RAW;
    switch (key.type){
        case QxtSshKey::Dss:
            typemask|=LIBSSH2_KNOWNHOST_KEY_SSHDSS;
            break;
        case QxtSshKey::Rsa:
            typemask|=LIBSSH2_KNOWNHOST_KEY_SSHRSA;
            break;
        case QxtSshKey::UnknownType:
            return false;
    };


    return(libssh2_knownhost_add(d->d_knownHosts, qPrintable(hostname),
                                 NULL, key.key.data(), key.key.size(),
                                 typemask,  NULL));

}

/*!
 * Returns the host key of the currently connected server.
 */
QxtSshKey QxtSshClient::hostKey() const{
    return d->d_hostKey;
}

/*!
 * Returns the hostname of the currently connected server.
 */
QString   QxtSshClient::hostName() const{
    return d->d_hostName;
}

/*!
 * Opens a new SSH channel that can invoke a process or SSH subsystem on the SSH server.
 * The process's stdin, stdout, and stderr are piped through the channel.
 *
 * Returns NULL if an error occurs while opening the channel, such as not being connected to an SSH server.
 *
 * \sa QxtSshProcess
 */
QxtSshProcess * QxtSshClient::openProcessChannel(){
    if(d->d_state!=6){
        qWarning("cannot open channel before connected()");
        return NULL;
    }
    QxtSshProcess * s=new QxtSshProcess(this);
    d->d_channels.append(s);
    connect(s,SIGNAL(destroyed()),d,SLOT(d_channelDestroyed()));
    return s;
}

/*!
 * Opens a new SSH channel and attempts to establish a TCP connection from the SSH server
 * to a remote host. Traffic on this TCP connection is tunneled through the channel.
 *
 * Note that traffic between the SSH server and the remote host is unencrypted. Only
 * communication between QxtSshClient and the SSH server is encrypted.
 *
 * Returns NULL if an error occurs while opening the channel, such as not being connected to an SSH server.
 *
 * \sa QxtSshTcpSocket
 */
QxtSshTcpSocket * QxtSshClient::openTcpSocket(const QString & hostName,quint16 port){
    if(d->d_state!=6){
        qWarning("cannot open channel before connected()");
        return NULL;
    }
    QxtSshTcpSocket * s=new QxtSshTcpSocket(this);
    d->d_channels.append(s);
    connect(s,SIGNAL(destroyed()),d,SLOT(d_channelDestroyed()));
    s->d->openTcpSocket(hostName,port);
    return s;
}


static ssize_t qxt_p_libssh_recv(int socket,void *buffer, size_t length,int flags, void **abstract){
    Q_UNUSED(socket);
    Q_UNUSED(flags);
    QTcpSocket* c=reinterpret_cast<QTcpSocket*>(*abstract);
    int r=c->read(reinterpret_cast<char*>(buffer),length);
    if(r==0)
        return -EAGAIN;
    return r;
}

static ssize_t qxt_p_libssh_send(int socket,const void *buffer, size_t length,int flags, void **abstract){
    Q_UNUSED(socket);
    Q_UNUSED(flags);
    QTcpSocket* c=reinterpret_cast<QTcpSocket*>(*abstract);
    int r=c->write(reinterpret_cast<const char*>(buffer),length);
    if(r==0)
        return -EAGAIN;
    return r;
}





QxtSshClientPrivate::QxtSshClientPrivate()
    :d_session(0)
    ,d_knownHosts(0)
    ,d_state(0)
    ,d_errorCode(0)
    ,d_useSystemClient(false)

{
    connect(this,SIGNAL(connected()),this,SLOT(d_connected()));
    connect(this,SIGNAL(disconnected()),this,SLOT(d_disconnected()));
    connect(this,SIGNAL(readyRead()),this,SLOT(d_readyRead()));

    Q_ASSERT(libssh2_init (0)==0);

    d_reset();

}

QxtSshClientPrivate::~QxtSshClientPrivate(){
    d_reset();
    if(d_session){
        libssh2_knownhost_free(d_knownHosts);
        libssh2_session_free(d_session);
    }
}

void QxtSshClientPrivate::d_connected(){
    d_state=2;
    d_readyRead();
}

void QxtSshClientPrivate::d_readyRead(){
    if(d_state==2){
        int sock=socketDescriptor();
        int ret=0;

        //1) initalise ssh session. exchange banner and stuff.

        if((ret = libssh2_session_startup(d_session, sock)) ==LIBSSH2_ERROR_EAGAIN){
            return;
        }
        if (ret) {
            qWarning("Failure establishing SSH session: %d", ret);
            emit p->error(QxtSshClient::UnexpectedShutdownError);
            d_reset();
            return;
        }

        //2) make sure remote is safe.
        size_t len;
        int type;
        const char * fingerprint = libssh2_session_hostkey(d_session, &len, &type);
        d_hostKey.key=QByteArray(fingerprint,len);
        d_hostKey.hash=QByteArray(libssh2_hostkey_hash(d_session,LIBSSH2_HOSTKEY_HASH_MD5),16);
        switch (type){
            case LIBSSH2_HOSTKEY_TYPE_RSA:
                d_hostKey.type=QxtSshKey::Rsa;
                break;
            case LIBSSH2_HOSTKEY_TYPE_DSS:
                d_hostKey.type=QxtSshKey::Dss;
                break;
            default:
                d_hostKey.type=QxtSshKey::UnknownType;
        }
        if(fingerprint) {
            struct libssh2_knownhost *host;
            int check = libssh2_knownhost_check(d_knownHosts, qPrintable(d_hostName),
                                                (char *)fingerprint, len,
                                                LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                                LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                                &host);

            switch(check){
                case LIBSSH2_KNOWNHOST_CHECK_MATCH:
                    d_state=3;
                    d_readyRead();
                    return;
                case LIBSSH2_KNOWNHOST_CHECK_FAILURE:
                    d_delaydError=QxtSshClient::HostKeyInvalidError;
                    break;
                case LIBSSH2_KNOWNHOST_CHECK_MISMATCH:
                    d_delaydError=QxtSshClient::HostKeyMismatchError;
                    break;
                case LIBSSH2_KNOWNHOST_CHECK_NOTFOUND:
                    d_delaydError=QxtSshClient::HostKeyUnknownError;
                    break;
            }
        }else{
            d_delaydError=QxtSshClient::HostKeyInvalidError;
        }
        d_getLastError();
        d_reset();
        disconnectFromHost ();
        QTimer::singleShot(0,this,SLOT(d_delaydErrorEmit()));
        return;

    }else if(d_state==3){
        //3) try auth type "none" and get a list of other methods
        //   in the likely case that the server doesnt like "none"

        QByteArray username=d_userName.toLocal8Bit();
        char * alist=libssh2_userauth_list(d_session, username.data(),username.length());
        if(alist==NULL){
            if(libssh2_userauth_authenticated(d_session)){
                //null auth ok
                emit p->connected();
                d_state=5;
                return;
            }else if(libssh2_session_last_error(d_session,NULL,NULL,0)==LIBSSH2_ERROR_EAGAIN) {
                return;
            }else{
                d_getLastError();
                emit p->error(QxtSshClient::UnexpectedShutdownError);
                d_reset();
                emit p->disconnected();
                return;
            }
        }

        foreach(QByteArray m,QByteArray(alist).split(',')){
            if(m=="publickey"){
                d_availableMethods<<QxtSshClient::PublicKeyAuthentication;
            }
            else if(m=="password"){
                d_availableMethods<<QxtSshClient::PasswordAuthentication;
            }
        }
        d_state=4;
        d_readyRead();
    }else if(d_state==4){
#ifdef QXT_DEBUG_SSH
        qDebug("looking for auth option");
#endif
        if(d_availableMethods.contains(QxtSshClient::PublicKeyAuthentication) &&
           !d_privateKey.isNull() &&
           !d_failedMethods.contains(QxtSshClient::PublicKeyAuthentication)){

            d_currentAuthTry=QxtSshClient::PublicKeyAuthentication;
            d_state=5;
            d_readyRead();
            return;
        }
        if(d_availableMethods.contains(QxtSshClient::PasswordAuthentication) &&
           !d_passphrase.isNull() &&
           !d_failedMethods.contains(QxtSshClient::PasswordAuthentication)){

            d_currentAuthTry=QxtSshClient::PasswordAuthentication;
            d_state=5;
            d_readyRead();
            return;
        }
        emit p->authenticationRequired(d_availableMethods);
    }else if(d_state==5){
        int ret(0);
#ifdef QXT_DEBUG_SSH
        qDebug()<<"trying"<<d_currentAuthTry;
#endif
        if (d_useSystemClient) {            
            ret=authWithSystemClient();
            qDebug() << "SSH auth with system client: " << ret;
        } else if(d_currentAuthTry==QxtSshClient::PasswordAuthentication){
            ret=libssh2_userauth_password(d_session, qPrintable(d_userName),
                                          qPrintable(d_passphrase));

        }else if(d_currentAuthTry==QxtSshClient::PublicKeyAuthentication){
            ret=libssh2_userauth_publickey_fromfile(d_session,
                                                       qPrintable(d_userName),
                                                       NULL, //qPrintable(d_publicKey),
                                                       qPrintable(d_privateKey),
                                                       qPrintable(d_passphrase));
        }
        if(ret==LIBSSH2_ERROR_EAGAIN){
            return;
        }else if(ret==0){
            d_state=6;
            emit p->connected();
        }else{
            d_getLastError();
            emit p->error(QxtSshClient::AuthenticationError);
            d_failedMethods.append(d_currentAuthTry);
            d_state=4;
            d_readyRead();
        }
    }else if(d_state==6){
        QList<QxtSshChannel*>::const_iterator i;
        for (i = d_channels.constBegin(); i != d_channels.constEnd(); ++i){
            bool ret=(*i)->d->activate();
            if(!ret){
                d_getLastError();
            }
        }
    }else{
#ifdef QXT_DEBUG_SSH
        qDebug("did not expect to receive data in this state");
#endif
    }
}

void QxtSshClientPrivate::d_reset(){
#ifdef QXT_DEBUG_SSH
    qDebug("reset");
#endif
    //teardown
    if(d_knownHosts){
        libssh2_knownhost_free(d_knownHosts);
    }
    if(d_state>1){
        libssh2_session_disconnect(d_session,"good bye!");
    }
    if(d_session){
        libssh2_session_free(d_session);
    }

    d_state=0;
    d_errorCode=0;
    d_sleepInterval=5;
    d_errorMessage=QString();
    d_failedMethods.clear();
    d_availableMethods.clear();


    //buildup
    d_session=libssh2_session_init_ex(NULL,NULL,NULL,reinterpret_cast<void*>(this));
    libssh2_session_callback_set(d_session,LIBSSH2_CALLBACK_RECV,reinterpret_cast<void*>(&qxt_p_libssh_recv));
    libssh2_session_callback_set(d_session,LIBSSH2_CALLBACK_SEND,reinterpret_cast<void*>(&qxt_p_libssh_send));
    Q_ASSERT(d_session);

    d_knownHosts= libssh2_knownhost_init(d_session);
    Q_ASSERT(d_knownHosts);

    libssh2_session_set_blocking(d_session,0);
    libssh2_keepalive_config(d_session, 1, d_sleepInterval);

    d_keepAliveTimer = QSharedPointer<QTimer>(new QTimer());
    d_keepAliveTimer->setSingleShot(false);
    QObject::connect(d_keepAliveTimer.data(), &QTimer::timeout, this, [this]() {
        libssh2_keepalive_send(d_session, &d_sleepInterval);
        d_keepAliveTimer->start(d_sleepInterval * 1000);
        qDebug() << "SSH keepalive";
    });
    d_keepAliveTimer->start(d_sleepInterval * 1000);
}

void QxtSshClientPrivate::d_disconnected (){
    if(d_state!=0){
        qWarning("unexpected shutdown");
        d_reset();
    }
}

void QxtSshClientPrivate::d_getLastError(){
    char * msg;
    int len=0;
    d_errorCode=libssh2_session_last_error(d_session, &msg, &len,0);
    d_errorMessage=QString::fromLocal8Bit(QByteArray::fromRawData(msg,len));
}



void QxtSshClientPrivate::d_channelDestroyed(){
    QxtSshChannel* channel=  qobject_cast<QxtSshChannel*>(sender());
    d_channels.removeAll(channel);
}

void QxtSshClientPrivate::d_delaydErrorEmit(){
    emit p->error(d_delaydError);
}

int QxtSshClientPrivate::authWithSystemClient()
{
    struct libssh2_agent_publickey *identity, *prev_identity = NULL;
    d_agent = libssh2_agent_init(d_session);

    if (!d_agent)
        return 1;

    if (libssh2_agent_connect(d_agent) != 0)
        return 1;

    while (true) {
        int r = libssh2_agent_list_identities(d_agent);

        if (r == LIBSSH2_ERROR_EAGAIN)
            return r;
        else if (r != 0) {
            d_getLastError();
            qDebug() << d_errorMessage;
            return 1;
        } else
            break;
    }

    while (true) {
        if (libssh2_agent_get_identity(d_agent, &identity, prev_identity) != 0
                || libssh2_agent_userauth(d_agent, qPrintable(d_userName), identity))
            return 1;
        else
            return 0;

        prev_identity = identity;
    }
    return 1;
}
