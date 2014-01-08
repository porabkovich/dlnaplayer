#include "cautoshareswatcher.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

#include <QFile>
#include <QFileSystemWatcher>

#include "cperiodicalprocesscall.h"
#include "csharesparser.h"
#include "bidirectional_subtract.h"

#ifndef NDEBUG
#define NDEBUG
#endif
#ifndef QT_TEST
#include <debugmacros.h>
#endif

const QString CAutoSharesWatcher::smbtreeOut = "/tmp/shares.xml";

CAutoSharesWatcher::CAutoSharesWatcher(QObject *parent) :
	QObject(parent), runner_(0), fsWatcher_(0)
{
#ifndef QT_TEST
	runner_ = new CPeriodicalProcessCall(this);
	runner_->setProcessName("smbtree");
	runner_->setProcessArgs(QStringList() << "-Nb");
	runner_->start();
#endif
	{
		QFile f(smbtreeOut);
		f.open(QFile::WriteOnly | QFile::Truncate);
		f.write(" ",1);
		f.close();
	}
	fsWatcher_ = new QFileSystemWatcher(this);
	fsWatcher_->addPath(smbtreeOut);
	connect(fsWatcher_, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)));
}

void CAutoSharesWatcher::fileChanged(const QString &f)
{
	if(f != smbtreeOut)
		return ;
	QFile input(smbtreeOut);
	if(!input.open(QFile::ReadOnly))
		return;
	try	{

		QVector<QSharedPointer<SShare> > found = CSharesParser::parse(&input);
		QVector<QSharedPointer<SShare> > newShares;
		QVector<QSharedPointer<SShare> > invalidShares;
		std::sort(found.begin(), found.end(), SharePtrComparator());
		bidirectional_subtract(currentShares_.begin(), currentShares_.end(),
							   found.begin(), found.end(),
							   std::back_inserter(invalidShares),
							   std::back_inserter(newShares),
							   SharePtrComparator());
		for(int i = 0; i < invalidShares.size(); ++i)	{
			emit disappeared(invalidShares[i]);
		}
		for(int i = 0; i < newShares.size(); ++i)	{
			emit appeared(newShares[i]);
		}
		currentShares_ = found;
	}
	catch (const std::runtime_error &e)	{
#ifndef QT_TEST
		ERROR_INFO_TWO("std::runtime_error", e.what());
#endif
		return;
	}
}
