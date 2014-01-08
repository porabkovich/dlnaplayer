#include <iostream>
#include <iomanip>

#include <mntent.h>

#include "mntres.h"

#define NDEBUG

CMntRes::CMntRes()
{
	f = setmntent("/etc/mtab", "ro");
}

CMntRes::~CMntRes()
{
    endmntent(f);
}

QStringList CMntRes::resources()
{
    QStringList result;
    mntent *ptr = 0;
    while(ptr = getmntent(f))    {
		QString str = QString::fromUtf8(ptr->mnt_dir);
        if(str.startsWith(QLatin1String("/mnt/stb")))
           result << str;
    }
#ifndef NDEBUG
    QStringList::const_iterator it = result.constBegin();
    while(it != result.constEnd())  {
        std::cout << it->toUtf8().constData() << std::endl;
        ++it;
    }
#endif
    return result;
}

bool isResourceMounted(const QString &path)
{
    QStringList all = CMntRes().resources();
	foreach(const QString &mounted,all)	{
		if(path.indexOf(mounted) == 0)
			return true;
	}
	return false;
}


