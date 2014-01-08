#ifndef CSYSCALL_H
#define CSYSCALL_H

class CSysCall
{
public:
	virtual int mount(const char *source, const char *target,
					  const char *filesystemtype, unsigned long mountflags,
					  const void *data) = 0;
	virtual int umount(const char *target) = 0;
	virtual int system(const char *command) = 0;
	virtual int mkdir(const char *pathname, int mode) = 0;
	virtual int rmdir(const char *pathname) = 0;

	virtual ~CSysCall() {}
};

#endif // CSYSCALL_H
