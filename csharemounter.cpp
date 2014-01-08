#include "csharemounter.h"
#include <errno.h>
#include <string.h>
#include <sys/mount.h>

#include <QStack>
#include <QStringList>

#include "csyscall.h"

CShareMounter::CShareMounter(CSysCall *s) : syscall_(s), mountPoint_(QString::fromUtf8("/"))
{
}

CShareMounter::~CShareMounter()
{
	delete syscall_;
}

int CShareMounter::mountNfsShare(const SShare &s)
{
	QString target = mountPoint_ + s.name + "/";
	QString cmd = QString("mount -t nfs -o nolock%1 %2:%3 %4")
			.arg(SShare::Nfs_Udp == s.type ? "": ",tcp")
			.arg(s.ip)
			.arg(s.path)
			.arg(target.replace(" ","\\ "));
	return syscall_->system(cmd.toUtf8().constData());
}

int CShareMounter::mountSambaShare(const SShare &s)
{
	QString target = mountPoint_ + s.name + "/";
	QString source = "//" + s.ip + "/" + s.path;
	QString type = QString::fromUtf8("cifs");
	unsigned long mountFlags = 0;
	QString data = QString::fromUtf8("iocharset=utf8,soft,nolock,noserverino,username=");
	if(s.username.isEmpty())	{
		data+=QString::fromUtf8("anonymous,guest");
	}
	else if(s.password.isEmpty())	{
		data+=s.username + QString::fromUtf8(",guest");
	}
	else	{
		data+=s.username + ",password=" + s.password;
	}
	return syscall_->mount(source.toUtf8().constData(),
						   target.toUtf8().constData(),
						   type.toUtf8().constData(),
						   mountFlags,
						   (void *) data.toUtf8().constData());
}

int CShareMounter::mountFtphare(const SShare &s)
{
	QString target = mountPoint_ + s.name + "/";
	QString cmd;

	if(s.username.isEmpty())	{
		cmd = QString("/sdk/tools/bin/curlftpfs -o connect_timeout=5 ftp://%1%2 %3")
				.arg(s.ip)
				.arg(s.path)
				.arg(target.replace(" ","\\ "));
	}
	else	{
		if(s.password.isEmpty())	{
			cmd = QString("/sdk/tools/bin/curlftpfs -o connect_timeout=5 ftp://%1@%2%3 %4")
					.arg(s.username)
					.arg(s.ip)
					.arg(s.path)
					.arg(target.replace(" ","\\ "));
		}
		else	{
			cmd = QString("/sdk/tools/bin/curlftpfs -o connect_timeout=5 ftp://%1:%2@%3%4 %5")
					.arg(s.username)
					.arg(s.password)
					.arg(s.ip)
					.arg(s.path)
					.arg(target.replace(" ","\\ "));
		}
	}
	return syscall_->system(cmd.toUtf8().constData());
}

void CShareMounter::mount(const SShare &s)
{
	if(SShare::Ftp != s.type)	{
		int res = syscall_->system(QString("ping -c 2 -qw 2 %1 > /dev/null").arg(s.ip).toUtf8().constData());
		if(res)
			throw CMountError(0, "Destination host unreachable");
	}
	int res = 0;
	if(SShare::Nfs_Tcp == s.type || SShare::Nfs_Udp == s.type)	{
		res = mountNfsShare(s);
	}
	else if(SShare::Samba == s.type)		{
		res = mountSambaShare(s);
	}
	else if(SShare::Ftp == s.type)	{
		res = mountFtphare(s);
	}
	else	{
		throw CMountError(0, "Unknown share type");
	}
	if(res != 0)	{
		int e = errno;
		throw CMountError(e, strerror(e));
	}
}

void CShareMounter::umount(const SShare &s)
{
	QString target = mountPoint_ + s.name + "/";
	int res = syscall_->umount(target.toUtf8().constData());
	if(res == -1)	{
		int e = errno;
		throw CMountError(e, strerror(e));
	}
}

void CShareMounter::mkdir(const SShare &s)
{
	QStringList dirs = s.name.split('/', QString::SkipEmptyParts);
	QString mp = mountPoint_;
	while(dirs.size())	{
		mp += dirs.front() + "/";
		dirs.pop_front();
		int res = syscall_->mkdir(mp.toUtf8().constData(), 0755);
		if(res == -1)	{
			int e = errno;
			if(e == EEXIST && dirs.size())	// thow if only it is the last folder
				continue;
			throw CMountError(e, strerror(e));
		}
	}
}

void CShareMounter::rmdir(const SShare &s)
{
	QStack<QString> removeList;
	QStringList dirs = s.name.split('/', QString::SkipEmptyParts);
	removeList.push( mountPoint_ + dirs.front() + "/");
	dirs.pop_front();
	while(dirs.size())	{
		removeList.push(removeList.top() + dirs.front() + "/");
		dirs.pop_front();
	}
	bool first = true;
	while(removeList.size())	{
		int res = syscall_->rmdir(removeList.pop().toUtf8().constData());
		if(res == -1)	{
			int e = errno;
			if( e == ENOTEMPTY && !first )	// thow if only it is the last folder
				continue;
			throw CMountError(e, strerror(e));
		}
		first = false;
	}
}

void CShareMounter::setMountPoint(const QString &mp)
{
	 mountPoint_ = mp;
	 if(!mountPoint_.endsWith('/'))	{
		 mountPoint_ += '/';
	 }
}
