#ifndef MNTRES_H
#define MNTRES_H

#include <QStringList>

class CMntRes
{
    FILE *f;

    CMntRes(CMntRes &);
    CMntRes& operator= (CMntRes&);

public:
    CMntRes();
    ~CMntRes();
    QStringList resources();

};

bool isResourceMounted(const QString &);

#endif // MNTRES_H
