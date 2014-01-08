#include "sharemigration.h"

#include <sstream>
#include <iostream>
#include <iomanip>

#include <QString>
#include <QHash>
#include <QDomElement>
#include <QFile>
#include <QVector>
#include <QTextStream>

#include "mediaplayer.h"
#include "csharemounterinterface.h"
#include "cshareslistdialog.h"

#define MODULE "mediaplayer"
#ifndef NDEBUG
//#define NDEBUG
#endif
#include <debugmacros.h>
#include <tools.h>

namespace {
typedef QHash<QString, QHash<QString, QString> > Data_t;

void parse(QIODevice & dev, Data_t & result)	{ // thows std::runtime_error
	QString errorStr;
	int errorLine = 0, errorColumn = 0;
	QDomDocument doc;
	if (!doc.setContent(&dev, false, &errorStr, &errorLine, &errorColumn)) {
		std::ostringstream out;
		out << "Parse error at \nline:" << errorLine
			<< "\ncolumn:" << errorColumn
			<< "\nreason: " << errorStr.toUtf8().constData() << std::endl;
		ERROR_( << out.str() );
		throw std::runtime_error(out.str());
	}
	QDomElement sett = doc.documentElement();
	if(sett.isNull()) return;
	QDomElement block = sett.firstChild().toElement();
	while(!block.isNull())		{
		QDomElement param = block.firstChild().toElement();
		while(!param.isNull())		{
			result[block.tagName()][param.tagName()] = param.text();
			param = param.nextSibling().toElement();
		}
		block = block.nextSibling().toElement();
	}
}

void save(QIODevice & dev, Data_t & data){
	QTextStream out(&dev);
	out.setCodec("UTF-8");
	out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	out << "<settings version=\"0.1\">\n";
	QHash<QString,QHash<QString,QString> >::const_iterator it=data.begin();
	QHash<QString,QString>::const_iterator it1;
	for(;it!=data.end();++it)
	{
		out << "\t<" << it.key() << ">" << endl;
		for(it1=it.value().begin();it1!=it.value().end();++it1)
		{
			const QString &par = it1.key();
			const QString &val = it1.value();
			out	<< "\t\t<" << par << ">"
				<< "<![CDATA[" << val << "]]>"
				<< "</"	<< par << ">" << endl;
		}
		out << "\t</" << it.key() << ">" << endl;
	}
	out << "</settings>\n" << endl;
}

void saveToFile(const QString &filename, Data_t & data)	{
	QFile file(filename);
	if( !file.open(QFile::WriteOnly | QFile::Text) ) 	{
		ERROR_( << "Error: Can not open file " << filename << ": " << file.errorString() )
				  << std::endl;
		file.close();
		return;
	}
	save(file, data);
}

const QString filename = "/etc/settings.xml";
const QString markFile = "/home/user/temp_files/.shares_migrated";

}

void ShareMigration::migrate()
{
	DEBUG_INFO();
	if(QFile::exists(markFile))	{
		WARNING_INFO_ONE("Shares have already been migrated.");
		return;
	}
	DEBUG_INFO_ONE("Parsing /etc/settings.xml");
	QFile file(filename);
	Data_t data;
	if (!file.open(QFile::ReadOnly)) {
		ERROR_INFO_THREE("Can't open file:", filename, file.errorString());
		return;
	}
	try	{
		parse(file, data);
	}
	catch(std::exception &e)	{
		ERROR_INFO_TWO("Exception", e.what());
		return;
	}
	QVector<QSharedPointer<SShare> > sharesToMigrate;
	bool changed = false;
	foreach(const QString &key, data.keys())	{
		if(!key.startsWith("share"))
			continue;
		QSharedPointer<SShare> p(new SShare);
		p->name = data[key]["name"];
		p->ip   = data[key]["host"];
		p->path = data[key]["folder"];
		p->username = data[key]["login"];
		p->password = data[key]["pass"];
		p->type = data[key]["sharetype"] == "1" ? SShare::Samba : SShare::Nfs_Udp ;
		sharesToMigrate.push_back(p);
		data.remove(key);
		changed = true;
	}
	WARNING_INFO_TWO(sharesToMigrate.size(), "shares were found.");

	QSettings sett(CMediaPlayer::settingsFilename, QSettings::NativeFormat);
	QVector<QSharedPointer<SShare> > shares = CSharesListDialog::loadSettings(sett);
	WARNING_INFO_TWO(shares.size(), "new shares were found.");
	shares += sharesToMigrate;
	CSharesListDialog::saveSettings(sett, shares);
	WARNING_INFO_ONE("All shares were saved to new location");
	if(changed)	{
		DEBUG_INFO_ONE("Saving modified settings to /etc/settings.xml ");
		saveToFile(filename, data);
	}

	QFile mark(markFile);
	if(!mark.open(QFile::WriteOnly))	{
		return;
	}
	mark.write(" ", 1);
	mark.close();
}
