#ifndef CSHAREMOUNTER_H
#define CSHAREMOUNTER_H

#include "csharemounterinterface.h"

class CSysCall;
#include "csyscall.h"


class CShareMounter : public CShareMounterInterface
{
	CSysCall * syscall_;
	QString mountPoint_;

	CShareMounter(const CShareMounter &);
	const CShareMounter & operator=(const CShareMounter &);

	int mountNfsShare(const SShare &s);
	int mountSambaShare(const SShare &s);
	int mountFtphare(const SShare &s);

public:

	CShareMounter(CSysCall *s);
	~CShareMounter();

	void mount(const SShare &s);
	void umount(const SShare &s);
	void mkdir(const SShare &s);
	void rmdir(const SShare &s);
	QString mountPoint() const {return mountPoint_;}
	void setMountPoint(const QString &mp);

};

#endif // CSHAREMOUNTER_H
