#ifndef CNATIVESYSCALL_H
#define CNATIVESYSCALL_H

#include "csyscall.h"

struct CNativeSysCall : public CSysCall
{
	virtual int mount(const char *source, const char *target,
					  const char *filesystemtype, unsigned long mountflags,
					  const void *data);
	virtual int umount(const char *target);
	virtual int system(const char *command);
	virtual int mkdir(const char *pathname, int mode);
	virtual int rmdir(const char *pathname);

};

#endif // CNATIVESYSCALL_H
