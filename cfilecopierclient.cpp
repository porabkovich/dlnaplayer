#include "cfilecopierclient.h"

#include <QLocalSocket>
#include <QTimer>
#include <QProcess>
#include "copiertypes.h"
#include "tools.h"

#define MODULE "mediaplayer"
#ifndef NDEBUG
#define NDEBUG
#endif
#include "debugmacros.h"

struct CFileCopierClient::Impl	{
	CFileCopierClient::EState state_;
	QLocalSocket *socket_;
	QString source;
	QString destination;
	CP::SServerResponse lastServerResponse;
	Impl():state_(CFileCopierClient::NotConnected)
	  ,socket_(0)	{}
};

CFileCopierClient::CFileCopierClient(QObject *parent) :
	QObject(parent), impl_(new Impl)
{
	DEBUG_INFO();
	impl_->socket_ = new QLocalSocket(this);
	connect(impl_->socket_,SIGNAL(readyRead()),this,SLOT(responseReceived()));
	connect(impl_->socket_,SIGNAL(readyRead()),this,SLOT(showProgressInStart()));
	connect(impl_->socket_,SIGNAL(stateChanged(QLocalSocket::LocalSocketState)),this,SLOT(socketStateChanged()));
	QTimer::singleShot(10,this,SLOT(tryToConnectToServer()));
	impl_->lastServerResponse.status = CP::Error;
}
void CFileCopierClient::showProgressInStart()
{
	disconnect(impl_->socket_,SIGNAL(readyRead()),this,SLOT(showProgressInStart()));
	emit process();
}

void CFileCopierClient::socketStateChanged()
{
	DEBUG_INFO_ONE(int(impl_->socket_->state()));
}

void CFileCopierClient::tryToConnectToServer()
{
	QLocalSocket *sock = impl_->socket_;
	DEBUG_INFO_TWO("before ",int(sock->state()));
	sock->connectToServer(CP::socketName);
	DEBUG_INFO_TWO("after ",int(sock->state()));
	if(sock->state() == QLocalSocket::UnconnectedState)	{
		ERROR_INFO_ONE("Can not connect to server socket.");
//		shutDown();
//		system(impl_->serverBinaryName.toUtf8().constData());
//		QProcess proc;
//		proc.start(impl_->serverBinaryName);
//		proc.waitForFinished(-1);
		QTimer::singleShot(1000,this,SLOT(tryToConnectToServer()));
	}
}

void CFileCopierClient::responseReceived()
{
	DEBUG_INFO();
	if(!impl_->socket_->isValid() || !impl_->socket_->bytesAvailable())	{
		DEBUG_INFO_TWO("socket_->isValid() =",impl_->socket_->isValid());
		DEBUG_INFO_TWO("socket_->bytesAvailable() =",impl_->socket_->bytesAvailable());
		return;
	}
	QDataStream stream(impl_->socket_);
	stream.setVersion(QDataStream::Qt_4_7);
	while(impl_->socket_->bytesAvailable()){
		stream >> impl_->lastServerResponse;
		DEBUG_INFO_THREE(int(impl_->lastServerResponse.status),int(impl_->lastServerResponse.percent),impl_->lastServerResponse.filename);
		emit currentCopyingFName(impl_->lastServerResponse.filename);
		if(impl_->lastServerResponse.percent )	{
			emit progress(impl_->lastServerResponse.percent);
		}
		if(impl_->lastServerResponse.status == CP::Done)
			emit done();
	}
}

void CFileCopierClient::setDestination(QString d)
{
	DEBUG_INFO_ONE(d);
	impl_->destination = d;
}

void CFileCopierClient::setSource(QString s)
{
	DEBUG_INFO_ONE(s);
	impl_->source = s;
}

bool CFileCopierClient::start()
{
	DEBUG_INFO();
	if(!impl_->socket_ || !impl_->socket_->isValid())	{
		DEBUG_INFO_TWO("socket_ =",int(impl_->socket_));
		DEBUG_INFO_TWO("socket_->isValid() =",impl_->socket_->isValid());
		return false;
	}
	//TODO check source and destination
	CP::SClientRequest request;
	request.cmd = CP::Copy;
	request.source = impl_->source;
	request.destination = impl_->destination;
	QDataStream stream(impl_->socket_);
	stream.setVersion(QDataStream::Qt_4_7);
	stream << request;
	QTimer::singleShot(500, this, SLOT(checkState()));
	return true;
}
void CFileCopierClient::checkState()
{
	if(state() == 0){
		emit progress(impl_->lastServerResponse.percent);
		emit currentCopyingFName(impl_->lastServerResponse.filename);
		emit done();
		tryToConnectToServer();
	}
}

void CFileCopierClient::shutDown()
{
	DEBUG_INFO();
	system(("killall " + CP::serverName).toUtf8().constData());
}

CFileCopierClient::EState CFileCopierClient::state()
{
	DEBUG_INFO();
	if(impl_->socket_->state() == QLocalSocket::ConnectedState)
	{
		switch(impl_->lastServerResponse.status)	{
		case CP::Process:
			return Working;

		case CP::Done:
		case CP::Waiting:
			return Waiting;

		default:
			return NotConnected;
		}
	}
	return NotConnected;
}

void CFileCopierClient::stop()
{
	DEBUG_INFO();
	CP::SClientRequest request;
	request.cmd = CP::Terminate;
	QDataStream stream(impl_->socket_);
	stream.setVersion(QDataStream::Qt_4_7);
	stream << request;
}







