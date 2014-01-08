#ifndef CFILECOPIERCLIENT_H
#define CFILECOPIERCLIENT_H

#include <boost/shared_ptr.hpp>
#include <QObject>

class CFileCopierClient : public QObject
{
	Q_OBJECT

	struct Impl;
	boost::shared_ptr<CFileCopierClient::Impl> impl_;

public:
	explicit CFileCopierClient(QObject *parent = 0);
	bool copy(const QString &destination, const QString &source){
		setSource(source);
		setDestination(destination);
		return start();
	}
	void shutDown();

	enum EState	{
		NotConnected = 0,
		Working,
		Waiting
	};
	EState state();

private slots:
	void tryToConnectToServer();
//	void connected();
	void responseReceived();
	void socketStateChanged();
	void showProgressInStart();
void checkState();
signals:
	void progress(int value);
	void done();
	void currentCopyingFName(QString);
	void process();

public slots:
	void setDestination(QString);
	void setSource(QString);
	bool start();
	void stop();

};

#endif // CFILECOPIERCLIENT_H
