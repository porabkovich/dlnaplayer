#include "cnativesyscall.h"
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <unistd.h>

int CNativeSysCall::mount(const char *source, const char *target, const char *filesystemtype, unsigned long mountflags, const void *data) {
	return ::mount(source, target, filesystemtype, mountflags, data);
}

int CNativeSysCall::umount(const char *target)	{
	return ::umount(target);
}

int CNativeSysCall::system(const char *command)	{
	return ::system(command);
}

int CNativeSysCall::mkdir(const char *pathname, int mode)	{
	return ::mkdir(pathname, (mode_t) mode);
}

int CNativeSysCall::rmdir(const char *pathname)	{
	return ::rmdir(pathname);
}
