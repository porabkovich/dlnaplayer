#include "csharemanager.h"

#include <QMutexLocker>

#include "csharemounterinterface.h"
#include <algorithm>
#include <errno.h>

#ifndef NDEBUG
//#define NDEBUG
#endif
#ifndef QT_TEST
#include <debugmacros.h>
#include <tools.h>
#endif

CShareManager::CShareManager(CShareMounterInterface *m, QObject *parent):
	QThread(parent), umountAll_(false), mounter_(m)
{
}

CShareManager::~CShareManager()
{
	delete mounter_;
	wait();
}

void CShareManager::run()
{
	while(true)	{
		{
			QMutexLocker l(&taskListmutex_);
			while( !umountAll_ && taskList_.isEmpty() )
				cond_.wait(&taskListmutex_);
		}
		if(umountAll_)	{
			umountAllImpl();
			return;
		}
		started_ = false;
		while(!taskList_.empty())	{
			if(!started_)	{
				emit working();
				started_ = true;
			}
			QPair<ETask, QSharedPointer<SShare> > task;
			{
				QMutexLocker l(&taskListmutex_);
				task = taskList_.front();
				taskList_.pop_front();
			}
			switch(task.first)	{
			case Mount:				mount(task.second);				break;
			case MountAutoShare:		mountAutoShare(task.second);	break;
			case Umount:				umount(task.second);			break;
			case MakePath:			makePath(task.second);			break;
			case DestroyPath:		destroyPath(task.second);		break;
			default:	break;
			}
		}
		if(started_)	{
			started_ = false;
			emit done();
		}
	}
}

QVector<QPair<QSharedPointer<SShare>, QPair<int, QString> > > CShareManager::errors()
{
	QMutexLocker l(&taskListmutex_);
	QVector<QPair<QSharedPointer<SShare>, QPair<int, QString> > > retv = errors_;
	errors_.clear();
	return retv;
}

void CShareManager::addShare(const QSharedPointer<SShare> & s)
{
#ifndef QT_TEST
	DEBUG_INFO_ONE(s->name);
#endif
	QSharedPointer<SShare> p = QSharedPointer<SShare>(new SShare(*s));
	QMutexLocker l(&taskListmutex_);
	taskList_.push_back(qMakePair(Mount, p));
	cond_.wakeOne();
}

void CShareManager::addAutoShare(const QSharedPointer<SShare> & s)
{
#ifndef QT_TEST
	DEBUG_INFO_ONE(s->name);
#endif
	QSharedPointer<SShare> p = QSharedPointer<SShare>(new SShare(*s));
	QMutexLocker l(&taskListmutex_);
	taskList_.push_back(qMakePair(MakePath, p));
	cond_.wakeOne();
}

struct find_by_name : public std::binary_function<QString, QSharedPointer<SShare>, bool> 	{
	bool operator() (const QString &s, const QSharedPointer<SShare> &p) const	{
		return s == p->name;
	}
};

void CShareManager::tryMountAutoShare(const QString &path)
{
#ifndef QT_TEST
	DEBUG_INFO_ONE(path);
#endif
	if(!path.startsWith(mounter_->mountPoint()))
		return;
	QString name = path.mid(mounter_->mountPoint().size());
	QMutexLocker l(&taskListmutex_);
	CShareSet::iterator it = std::find_if(prepared_.begin(), prepared_.end(), std::bind1st(find_by_name(), name));
	if(it == prepared_.end()) return;
	taskList_.push_back(qMakePair(MountAutoShare, *it));
	cond_.wakeOne();
}

void CShareManager::tryMountAutoShare(const QSharedPointer<SShare> &s)
{
#ifndef QT_TEST
	DEBUG_INFO_ONE(s->name);
#endif
	QSharedPointer<SShare> p = QSharedPointer<SShare>(new SShare(*s));
	QMutexLocker l(&taskListmutex_);
	taskList_.push_back(qMakePair(MountAutoShare, p));
	cond_.wakeOne();
}

void CShareManager::removeAutoShare(const QSharedPointer<SShare> &s)
{
#ifndef QT_TEST
	DEBUG_INFO_ONE(s->name);
#endif
	QSharedPointer<SShare> p = QSharedPointer<SShare>(new SShare(*s));
	QMutexLocker l(&taskListmutex_);
	taskList_.push_back(qMakePair(DestroyPath, p));
	cond_.wakeOne();
}

void CShareManager::removeShare(const QSharedPointer<SShare> & s)
{
#ifndef QT_TEST
	DEBUG_INFO_ONE(s->name);
#endif
	QSharedPointer<SShare> p = QSharedPointer<SShare>(new SShare(*s));
	QMutexLocker l(&taskListmutex_);
	taskList_.push_back(qMakePair(Umount, p));
	cond_.wakeOne();
}

void CShareManager::umountAll()
{
	QMutexLocker l(&taskListmutex_);
	umountAll_ = true;
	cond_.wakeOne();
}


// private members, they are called in background thread
void CShareManager::mount(const QSharedPointer<SShare> &p)
{
	CShareSet::iterator it = mounted_.find(p);
	if(it != mounted_.end())	{
		errors_.push_back( qMakePair(p, qMakePair(0, QString("Share already exists") ) ) );
		emit hasErrors();
		return;
	}
	bool needToRemoveDir = false;
	try	{
		mounter_->mkdir(*p);
		needToRemoveDir = true;
	}
	catch(CShareMounterInterface::CMountError &e)	{
		if(e.err() != EEXIST)	{
			errors_.push_back( qMakePair(p, qMakePair(e.err(), QString(e.what()) ) ) );
			emit hasErrors();
		}
	}
	try	{
		mounter_->mount(*p);
		emit mounted(mounter_->mountPoint() + p->name);
		mounted_.insert(p);
	}
	catch(CShareMounterInterface::CMountError &e)	{
		errors_.push_back( qMakePair(p, qMakePair(e.err(), QString(e.what()) ) ) );
		emit hasErrors();
	}
	if(needToRemoveDir)	{
		try	{	mounter_->rmdir(*p);		}
		catch(CShareMounterInterface::CMountError &e)	{}
	}
}

void CShareManager::mountAutoShare(const QSharedPointer<SShare> &p)
{
	CShareSet::iterator it = prepared_.find(p);
	if(it == prepared_.end())
		return;
	try	{
		mounter_->mount(*(*it));
		emit mounted(mounter_->mountPoint() + (*it)->name);
		mounted_.insert(*it);
		prepared_.erase(it);
	}
	catch(CShareMounterInterface::CMountError &e)	{
		errors_.push_back( qMakePair(p, qMakePair(e.err(), QString(e.what()) ) ) );
		emit hasErrors();
	}
}

void CShareManager::umount(const QSharedPointer<SShare> &p)
{
	CShareSet::iterator it = mounted_.find(p);
	if(it == mounted_.end())	{
#ifndef QT_TEST
		ERROR_("No such share to umount: name: " << p->name << " ip:" << p->ip);
#endif
		return;
	}
	QSharedPointer<SShare> share = *it;
	mounted_.erase(it);
	try	{
		mounter_->umount(*share);
		mounter_->rmdir(*share);
	}
	catch(CShareMounterInterface::CMountError &e)	{
		errors_.push_back( qMakePair(share, qMakePair(e.err(), QString(e.what()) ) ) );
		emit hasErrors();
	}
}

void CShareManager::makePath(const QSharedPointer<SShare> &p)
{
	try	{
		CShareSet::iterator it = prepared_.find(p);
		if(it != prepared_.end())	{
#ifndef QT_TEST
			ERROR_("Share: name: " << p->name << " ip: " << p->ip << " already prepeared.");
#endif
			return;
		}
		it = mounted_.find(p);
		if( it != mounted_.end() )	{
#ifndef QT_TEST
			ERROR_("Share: name: " << p->name << " ip: " << p->ip << " already mounted.");
#endif
			return;
		}
		mounter_->mkdir(*p);
		prepared_.insert(p);
	}
	catch(CShareMounterInterface::CMountError &e)	{
		errors_.push_back( qMakePair(p, qMakePair(e.err(), QString(e.what()) ) ) );
		emit hasErrors();
	}
}

void CShareManager::destroyPath(const QSharedPointer<SShare> &p)
{
	CShareSet::iterator it = prepared_.find(p);
	if(it == prepared_.end())	{
#ifndef QT_TEST
		ERROR_("No such share to remove: name: " << p->name << " ip:" << p->ip);
#endif
		return;
	}
	QSharedPointer<SShare> share = *it;
	prepared_.erase(it);
	try	{
		mounter_->rmdir(*share);
	}
	catch(CShareMounterInterface::CMountError &e)	{
		errors_.push_back( qMakePair(share, qMakePair(e.err(), QString(e.what()) ) ) );
		emit hasErrors();
	}
}

void CShareManager::umountAllImpl()
{
	while(!mounted_.empty())	{
		QSharedPointer<SShare> p = *mounted_.begin();
		umount(p);
	}
	while(!prepared_.empty())	{
		QSharedPointer<SShare> p = *prepared_.begin();
		destroyPath(p);
	}
}
