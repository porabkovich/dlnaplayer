#include "csharesparser.h"
#include <QFile>
#include <QtXml/QDomDocument>
#include <stdexcept>

QVector<QSharedPointer<SShare> > CSharesParser::parse(QIODevice *dev)
{
	QString errMsg;
	int errLine, errCol;
	QDomDocument doc;
	if(!doc.setContent(dev, &errMsg, &errLine, &errCol))
		throw std::runtime_error("Parse error at line " + QString::number(errLine).toStdString()
								 + ", column " + QString::number(errCol).toStdString() + ": "
								 + errMsg.toStdString());

	QVector<QSharedPointer<SShare> > ret_shares;
	QDomElement shares = doc.documentElement();
	if(shares.isNull())
		return ret_shares;

	QDomElement block = shares.firstChild().toElement();
	while(!block.isNull())
	{
		if(block.nodeName() != "share")		{
			block = block.nextSibling().toElement();
			continue;
		}
		QSharedPointer<SShare> shareptr(new SShare);
		shareptr->type = SShare::Samba;
		QDomElement param = block.firstChild().toElement();
		QString wg;
		while(!param.isNull())
		{
			if(param.nodeName() == "workgroup")
				wg = param.text();
			else if(param.nodeName() == "host")
				shareptr->ip = param.text();
			else if(param.nodeName() == "name")
				shareptr->name = param.text();
			else if(param.nodeName() == "folder")
				shareptr->path = param.text();
			param = param.nextSibling().toElement();
		}
		if(wg.isEmpty())
			wg = "unknown";
		shareptr->name = QString("all/%1/%2/%3").arg(wg).arg(shareptr->name).arg(shareptr->path);
		if(shareptr->name != "" && shareptr->path != "" && shareptr->ip != "")
			ret_shares.push_back(shareptr);
		block = block.nextSibling().toElement();
	}

	return ret_shares;
}
