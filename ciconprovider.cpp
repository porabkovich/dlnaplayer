#include <iostream>
#include <iomanip>

#include "ciconprovider.h"
#include "cfiletypedetect.h"
#include "tools.h"
#include "const_strings.h"
#include "mntres.h"

#ifndef GLOBAL_DEBUG_ENABLE
#define NDEBUG
#endif
#include "debugmacros.h"

CIconProvider::CIconProvider()
{
    folder = QIcon(QPixmap(":/mpimg/navicons/folder.png"));
    file = QIcon(QPixmap(":/mpimg/navicons/file.png"));
    movie = QIcon(QPixmap(":/mpimg/navicons/movie.png"));
    music = QIcon(QPixmap(":/mpimg/navicons/music.png"));
    image = QIcon(QPixmap(":/mpimg/navicons/image.png"));
    networkPlaces = QIcon(QPixmap(":/mpimg/navicons/networkPlaces.png"));
    networkFolder = QIcon(QPixmap(":/mpimg/navicons/networkFolder.png"));
    folderNotMounted = QIcon(QPixmap(":/mpimg/navicons/folderNotMounted.png"));
    localPlaces = QIcon(QPixmap(":/mpimg/navicons/localPlaces.png"));
    localFolder = QIcon(QPixmap(":/mpimg/navicons/localFolder.png"));
    playlists = QIcon(QPixmap(":/mpimg/navicons/playlists.png"));
	all = QIcon(QPixmap(":/mpimg/navicons/all.png"));
	workgroup = QIcon(QPixmap(":/mpimg/navicons/workgroup.png"));
	share = QIcon(QPixmap(":/mpimg/navicons/share.png"));
}

QIcon	CIconProvider::icon (QFileIconProvider::IconType type) const
{
	type = type;
	return folder;
}

QIcon	CIconProvider::icon (const QFileInfo & info ) const
{
	CFileTypeDetect fileTypeDetector;
	if(info.isDir())
	{
		QString dir = info.absoluteFilePath();
        if(dir.indexOf(FS::rootPath) == 0)
		{
            if(dir == FS::network)
                return networkPlaces;
            else if(dir == FS::local)
				return localPlaces;
            else if(dir == FS::playlists)
				return playlists;
            else if(dir.contains(FS::network+"/"))
			{
				DEBUG_INFO_ONE(info.absoluteFilePath());
                dir.remove(FS::network+"/");
				if(!dir.contains("/")){
					if(dir == "all")
						return all;
					return share;
				}
				if(dir.startsWith("all/")){
					dir.remove("all/");
					if(!dir.contains("/")){
						return workgroup;
					}
					else if(dir.count("/") == 1){
						return networkFolder;
					}
				}
//				else if(!isResourceMounted(info.absoluteFilePath())){
//					return folderNotMounted;
//				}

			}
            else if(dir.contains(FS::local+"/"))
			{
                dir.remove(FS::local+"/");
				if(!dir.contains("/"))
					return localFolder;
			}
		}
		return folder;
	}
	else if(fileTypeDetector.isAudioFile(info))
		return music;
	else if(fileTypeDetector.isVideoFile(info))
		return movie;
	else if(fileTypeDetector.isImageFile(info))
		return image;
	else return file;
}

