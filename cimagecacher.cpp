#include "cimagecacher.h"
#include <time.h>
#include <sys/time.h>
#include <QFileInfo>
#include <QDir>

#define NDEBUG
#include "debugmacros.h"


#define _IMG_CACHE_DIR "/tmp/stbgui/img_cache/"
//*****************************************************************************
// return cache file name from full img path
QString convertToCacheName (QString fullImgPath)
{
	QFileInfo info (fullImgPath);
	return QString::number (info.size()) + info.fileName();
}

//*****************************************************************************
// return true if img was cached of false, set cache file name to 'cacheName'
bool isItCached (QString fullImgPath, QString & cacheName)
{
	QDir cacheDir (_IMG_CACHE_DIR);
	QStringList cacheList = cacheDir.entryList (QDir::Files);
	cacheName = convertToCacheName(fullImgPath);
	if (cacheList.contains (cacheName, Qt::CaseSensitive))
		return true;
	return false;
}

//*****************************************************************************
CDecodeThread::CDecodeThread () : QThread ()
{
	stop_ = false;
	dec_index = 0;
}

//*****************************************************************************
// thread function for caching, it iterate infinity via img_list and make cache
// if file was not caching
// if user query some image was not cached, we jump chaching from this files
void CDecodeThread::run (void)
{
	struct timeval begin, end;
	QString cacheName;

	DEBUG_INFO_ONE ("\033[33m+++++++++++++ Start caching thread +++++++++++++++\033[0m");
		
	while ((!img_files.isEmpty()) && (!stop_)) {

		// if user needs some preview - find it in list and start from it
		if (!lastRequest.isEmpty()) {
			DEBUG_INFO_TWO ("\033[33m set new index on: \033[0m", lastRequest.toUtf8().data());
			int tmp_ind = img_files.indexOf(lastRequest);
			if (tmp_ind != -1) {
				dec_index = tmp_ind;
			}
			lastRequest.clear();
		}
		
		if (dec_index >= img_files.size()) 
			dec_index = 0;
		
		DEBUG_INFO_TWO ("check image: ", img_files.at(dec_index).toUtf8().data());
		if (!isItCached (src_dir + img_files.at(dec_index), cacheName)) {
			DEBUG_INFO_TWO ("make cache for: ", img_files.at(dec_index).toUtf8().data());
			// make image cache
			
			gettimeofday (&begin, 0);
			
			if (pixmap->load (src_dir + img_files.at(dec_index))) {
				QThread::msleep (500); // for gui thread not freeze
				
				if (pixmap->width()*2 > pixmap->height()*3) {// if w/h > 1.5 
					pixmap->scaledToWidth (300).save (_IMG_CACHE_DIR + cacheName, 0/*, 70quality*/);
				} else {
					pixmap->scaledToHeight (200).save (_IMG_CACHE_DIR + cacheName, 0/*, 70quality*/);
				}
				
				emit complite (img_files.at(dec_index));
				pixmap->load ("/sdk/film.png");
			} else {
				DEBUG_INFO_TWO ("\033[31mLoading failed: \033[0m", img_files.at(dec_index).toUtf8().data());
			}

			gettimeofday (&end, 0);
//			DEBUG_INFO_THREE ("\033[33m time spend: \033[0m", end.tv_sec - begin.tv_sec, end.tv_usec - begin.tv_usec);
//			printf ("\033[33m time spend: %lu : %lu\033[0m\n", end.tv_sec - begin.tv_sec, end.tv_usec - begin.tv_usec);
		}
		QThread::msleep (500); // for gui thread not freeze
		dec_index++;
	}
	DEBUG_INFO_ONE ("\033[33m------------ Stop caching thread ------------\033[0m");
}








//*****************************************************************************
CImageCacher::CImageCacher (void)
{
	defaultPix.load ("/sdk/film.png");
	QDir cacheDir ("");
	cacheDir.mkpath (_IMG_CACHE_DIR);

	thread = new CDecodeThread ();
	thread->setPriority(QThread::IdlePriority);
	thread->pixmap = new QPixmap ();
	connect (thread, SIGNAL(complite(QString)), this, SLOT(itemDecoded(QString)));
}

//*****************************************************************************
CImageCacher::~CImageCacher ()
{
	DEBUG_INFO_ONE ("\033[31m DESTRUCTOR \033[0m");
	thread->stop ();
	thread->wait (); // wait for finish decoding process
	delete thread->pixmap;
	delete thread;
}

//*****************************************************************************
// slot for signal that item was decoded
void CImageCacher::itemDecoded (QString name)
{
	emit previewReady(name); //rise enother signal
}

//*****************************************************************************
// start caching img for dir_ (calling many time with same dir give no effect )
void CImageCacher::startCaching (QString dir_)
{
	if (img_dir != dir_) {
		DEBUG_INFO_TWO ("Starting Caching for:", dir_.toUtf8().data());
		thread->stop ();
		thread->wait (); // wait for finish decoding process
		img_dir = dir_;
		DEBUG_INFO_TWO ("CImageCacher.img_dir = ", img_dir.toUtf8().data());
		thread->src_dir = dir_;
		QDir dir (dir_);
		thread->img_files = dir.entryList(QDir::Files);
	//	thread->setIndex (index);
	}
	thread->stop_=false;
	if (!thread->isRunning())
		thread->start();
}

//*****************************************************************************
// return preview or 
// return blank pixmap if preview was not ready and make this preview at next time
// so if you call it with some not cached img, caching thread makes preview at with point
QPixmap CImageCacher::getPreview (QString name)
{
	DEBUG_INFO_TWO ("Check cache for: ", (img_dir + name).toUtf8().data());

	QString cacheName;
	if (isItCached (img_dir + name, cacheName)) {
		DEBUG_INFO_TWO ("\033[32mItem found!! \033[0m: ", cacheName.toUtf8().data());
		return QPixmap (_IMG_CACHE_DIR + cacheName);
	} else {
		// set uncached file name, what user requested
		thread->lastRequest = name;
		DEBUG_INFO_TWO ("\033[33mCache NOT found!!\033[0m: ", cacheName.toUtf8().data());
	}
	return defaultPix;
}

//*****************************************************************************
// stop caching thread
void CImageCacher::stop (void)
{
	DEBUG_INFO_ONE ("\033[33m Stopping \033[0m");
	thread->stop ();
	thread->wait (); // wait for finish decoding process
}

