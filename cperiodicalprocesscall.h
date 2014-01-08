#ifndef CPERIODICALPROCESSCALL_H
#define CPERIODICALPROCESSCALL_H

#include <QObject>
#include <QString>
#include <QStringList>

class QTimer;

class CPeriodicalProcessCall : public QObject
{
	Q_OBJECT
	int period_;
	bool started_;
	QString processName_;
	QStringList args_;
	QTimer *timer_;

protected:
	virtual void call(const QString &pName, const QStringList &pArgs);

public:
	explicit CPeriodicalProcessCall(QObject *parent = 0);

	void setPeriod(int p);
	int period() const		{ return period_; }
	bool started() const	{ return started_; }
	QString processName() const { return processName_; }
	void setProcessName(const QString &pn) { processName_ = pn;}
	QStringList processArgs() const { return args_; }
	void setProcessArgs(const QStringList &args) { args_ = args; }

	
signals:
	
public slots:
	void timeout();
	void start();
	void stop();
	
};

#endif // CPERIODICALPROCESSCALL_H
