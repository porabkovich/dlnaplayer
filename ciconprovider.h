#ifndef TICONPROVIDER_H
#define TICONPROVIDER_H

#include <QFileIconProvider>

class CIconProvider : public QFileIconProvider
{
public:
	CIconProvider();
	QIcon	icon ( QFileIconProvider::IconType type ) const;
	QIcon	icon ( const QFileInfo & info ) const;


private:
	QIcon folder;
	QIcon folderNotMounted;
	QIcon file;
	QIcon movie;
	QIcon music;
	QIcon image;
	QIcon all;
	QIcon workgroup;
	QIcon share;

	QIcon networkPlaces;
	QIcon networkFolder;

	QIcon localPlaces;
	QIcon localFolder;

	QIcon playlists;

};

#endif // TICONPROVIDER_H
