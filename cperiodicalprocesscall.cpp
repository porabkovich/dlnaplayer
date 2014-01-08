#include "cperiodicalprocesscall.h"
#include <iostream>
#include <iomanip>
#include <QProcess>
#include <QTimer>

#ifndef NDEBUG
#define NDEBUG
#endif
#ifndef QT_TEST
#include <debugmacros.h>
#include <tools.h>
#endif

CPeriodicalProcessCall::CPeriodicalProcessCall(QObject *parent) :
	QObject(parent), period_(60000), started_(false), timer_(0)
{
	timer_ = new QTimer(this);
	timer_->setInterval(period_);
	timer_->setSingleShot(false);
	timer_->stop();
	connect(timer_, SIGNAL(timeout()), this, SLOT(timeout()));
	QTimer::singleShot(1, this, SLOT(timeout())); // first call
}

void CPeriodicalProcessCall::setPeriod(int p)
{
	period_ = p;
	if(timer_->isActive())
		start();
}

void CPeriodicalProcessCall::timeout()
{
	call(processName_, args_);
}

void CPeriodicalProcessCall::start()
{
	timer_->setInterval(period_);
	timer_->start();
}

void CPeriodicalProcessCall::stop()
{
	timer_->stop();
}

void CPeriodicalProcessCall::call(const QString &pName, const QStringList &pArgs)
{
	bool result = QProcess::startDetached(pName, pArgs);
	if(result)
		return;
#ifndef QT_TEST
	ERROR_INFO_TWO("Can not run process:", pName);
#endif
}
