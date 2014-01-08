#ifndef CSHARESPARSER_H
#define CSHARESPARSER_H

#include <QVector>
#include <QSharedPointer>
#include <QIODevice>
#include "csharemounterinterface.h"

class CSharesParser
{
public:
	static QVector<QSharedPointer<SShare> > parse(QIODevice *dev);
};

#endif // CSHARESPARSER_H
