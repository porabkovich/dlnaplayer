#ifndef CSHAREMANAGER_H
#define CSHAREMANAGER_H

#include <QThread>
#include <QSharedPointer>
#include <QMutex>
#include <QWaitCondition>
#include <QPair>
#include <QVector>
#include <set>
#include "csharemounterinterface.h"

class CShareMounterInterface;

class CShareManager : public QThread
{
	Q_OBJECT

	enum ETask	{
		Mount = 1,
		MountAutoShare,
		Umount,
		MakePath,
		DestroyPath
	};


	volatile bool umountAll_;
	bool started_;
	CShareMounterInterface * mounter_;
	QMutex taskListmutex_;
	QWaitCondition cond_;
	QList<QPair<ETask, QSharedPointer<SShare> > > taskList_;
	typedef std::set<QSharedPointer<SShare>, SharePtrComparator> CShareSet;
	CShareSet prepared_; // contains autoshares that have oly path
	CShareSet mounted_;  // contains mounted shares that have oly path
	QVector< QPair<QSharedPointer<SShare>, QPair<int, QString> > > errors_;

	void mount(const QSharedPointer<SShare> &);
	void mountAutoShare(const QSharedPointer<SShare> &);
	void umount(const QSharedPointer<SShare> &);
	void makePath(const QSharedPointer<SShare> &);
	void destroyPath(const QSharedPointer<SShare> &);
	void umountAllImpl();

public:
	explicit CShareManager(CShareMounterInterface *, QObject *parent = 0);
	~CShareManager();

	void run();

	QVector< QPair<QSharedPointer<SShare>, QPair<int, QString> > > errors();
	
signals:

	void mounted(QString);
	void hasErrors();
	void working();
	void done();
	
public slots:

//	share must have unique name
	void addShare(const QSharedPointer<SShare> &);
	void addAutoShare(const QSharedPointer<SShare> &);
	void tryMountAutoShare(const QString &);
	void tryMountAutoShare(const QSharedPointer<SShare> &);
	void removeAutoShare(const QSharedPointer<SShare> &);
	void removeShare(const QSharedPointer<SShare> &);
	void umountAll();
	
};

#endif // CSHAREMANAGER_H
