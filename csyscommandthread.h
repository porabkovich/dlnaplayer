#ifndef CSYSCOMMANDTHREAD_H
#define CSYSCOMMANDTHREAD_H
#include <queue>

#include <QThread>
#include <QString>

class CSysCommandThread:public QThread
{
	Q_OBJECT

	std::queue<QString> cmdq;

protected:
	void run();

public:
	explicit CSysCommandThread(QObject *parent,QString s):QThread(parent){cmdq.push(s);}
	explicit CSysCommandThread(QObject *parent):QThread(parent){}
	bool addCommand (const QString &str){
		if(isRunning())
			return false;
		cmdq.push(str);
		return true;
	}
};


#endif // CSYSCOMMANDTHREAD_H
