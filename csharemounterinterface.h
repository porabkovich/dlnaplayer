#ifndef CSHAREMOUNTERINTERFACE_H
#define CSHAREMOUNTERINTERFACE_H

#include <stdexcept>
#include <QString>
#include <QSharedPointer>

struct SShare	{
	enum Type{ Nfs_Udp, Samba, Ftp, Nfs_Tcp };

	QString name;
	QString ip;
	QString path;
	QString username;
	QString password;
	Type type;
};

struct SharePtrComparator	{
	bool operator()(const QSharedPointer<SShare> &lhs, const QSharedPointer<SShare> &rhs) const 	{
		using namespace std;
		return lhs->name + lhs->ip < rhs->name + rhs->ip;
	}
};

class CShareMounterInterface
{
public:
	class CMountError : public std::runtime_error	{
		int errno_;
	public:
		CMountError(int e, const std::string &what) : std::runtime_error(what), errno_(e) {}
		int err() const { return errno_; }
	};
	virtual void mount(const SShare &s) = 0;
	virtual void umount(const SShare &s) = 0;
	virtual void mkdir(const SShare &s) = 0;
	virtual void rmdir(const SShare &s) = 0;
	virtual QString mountPoint() const = 0;
	virtual ~CShareMounterInterface() {}
};


#endif // CSHAREMOUNTERINTERFACE_H
