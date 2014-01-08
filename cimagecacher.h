#ifndef _IMAGECACHER_H_
#define _IMAGECACHER_H_

#include <QThread>
#include <QStringList>
#include <QPixmap>


class QPixmap;

//*****************************************************************************
class CDecodeThread : public QThread {
	
	Q_OBJECT

	int dec_index;
public:
	bool stop_;
	QString src_dir;
	QString lastRequest;
	QPixmap * pixmap;
	QStringList img_files;
	CDecodeThread (void);
	void run();
	void stop () {stop_ = true;}
	void setIndex (int new_ind) {dec_index = new_ind;}
signals:
	void complite (QString);
};

//*****************************************************************************
class CImageCacher: public QObject {
	
	Q_OBJECT

	QPixmap defaultPix;
	QString img_dir;
	CDecodeThread * thread;
public:
	CImageCacher ();
	~CImageCacher();
	QPixmap getPreview (QString name);
	void startCaching (QString directory);
	void stop (void);
	void dropCache (void);
private slots:
	void itemDecoded (QString);
signals:
	void previewReady (QString);	
}; 

#endif
