#ifndef CAUTOSHARESWATCHER_H
#define CAUTOSHARESWATCHER_H

#include <QObject>
#include <QSharedPointer>
#include <QVector>
#include "csharemounterinterface.h"

class CPeriodicalProcessCall;
class QFileSystemWatcher;

class CAutoSharesWatcher : public QObject
{
	Q_OBJECT

	CPeriodicalProcessCall *runner_;
	QFileSystemWatcher *fsWatcher_;
	QVector<QSharedPointer<SShare> > currentShares_;

public:
	static const QString smbtreeOut;
	explicit CAutoSharesWatcher(QObject *parent = 0);
	
signals:
	void appeared(QSharedPointer<SShare>);
	void disappeared(QSharedPointer<SShare>);

public slots:

	void fileChanged(const QString &);
	
};

#endif // CAUTOSHARESWATCHER_H
