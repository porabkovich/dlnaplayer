#include <iostream>
#include <iomanip>
#include <cstdlib>

#include "csyscommandthread.h"
#include "tools.h"

#ifndef GLOBAL_DEBUG_ENABLE
#define NDEBUG
#endif
#include "debugmacros.h"

void CSysCommandThread::run()
{
	while(cmdq.size())	{
		DEBUG_INFO_ONE(cmdq.front());
		system(cmdq.front().toUtf8().data());
		cmdq.pop();
	}
	DEBUG_INFO_ONE("done!!!");
}
