#include "mediaplayer.h"
#include <algorithm>
#include <sys/statvfs.h>
#include <QGroupBox>
#include <QtPlugin>
#include <QLabel>
#include <QKeyEvent>
#include <QFileSystemModel>
#include <QScrollBar>
#include <QAction>
#include <QWidgetAction>
#include <QMenu>
#include <QTranslator>
#include <QProcess>
#include <QTextCodec>
#include <QTimer>
#include <QSettings>
#include <QTextStream>
#include <QFileSystemWatcher>

#include <errno.h>
#include "player.h"
#include "cprogressbar.h"
#include "completeplayer.h"
#include "progressbar.h"
#include "trackselector.h"
#include "timesearch.h"
#include "rclayout.h"
#include "source.h"
#include "clongoperationmanager.h"

#include "const_strings.h"
#include "cpushbutton.h"
#include "ciconprovider.h"
#include "treeview.h"
#include "copypanel.h"
#include "cinfobar.h"
#include "dialogs.h"
#include "tools.h"
#include "mntres.h"
#include "cfilecopierclient.h"
#include "csizemanager.h"
#include "cmediainfoparser.h"
#include "csyscommandthread.h"
#include "cinfomessage.h"
#include "cfileopendialog.h"
#include "cshareslistdialog.h"
#include "caboutcontent.h"
#include "cscreenkeyboard.h"
#include "csharemanager.h"
#include "csharemounter.h"
#include "cnativesyscall.h"
#include "cautoshareswatcher.h"
#include "sharemigration.h"

CSizeManager sizeM;

#define MODULE "mediaplayer"
//#define NDEBUG
#include "debugmacros.h"
#include "widgetgrabber.h"

const QString FS::rootPath = "/mnt/stb";
const QString FS::network = rootPath + "/network";
const QString FS::local = rootPath + "/local";
const QString FS::playlists = rootPath + "/playlists";
#ifdef BRCM
    const QString mediainfoCmd = "/usr/local/bin/utils/mediainfo";
    const QString mediainfoConfig = "/usr/local/config/mediainfo/mi.tmpl";
#else
    const QString mediainfoConfig = "/sdk/config/config_files/mi.tmpl ";
    const QString mediainfoCmd = "/sdk/tools/bin/mediainfo";
#endif
const QString mediainfoSymLink = "/tmp/minfolink";
const QString mediainfoOutFile = "/tmp/minfo";
CMediaPlayer::EMountState mountState = CMediaPlayer::MOUNT_NONE;

namespace{
const QString localSettingsFilename = QLatin1String("/tmp/mediaplayer.local.conf");
}

const QString CMediaPlayer::settingsFilename = "/home/user/config/config_files/mediaplayer.conf";

namespace {
boost::shared_ptr<QWidget> createDarkenedWidget()
{
	boost::shared_ptr<QWidget> widget(new QWidget());
	widget->setGeometry(0,0,1920,1080);
	QPalette pal;
	pal.setColor(QPalette::Window,QColor(0,0,0,155));
	widget->setPalette(pal);
	widget->showFullScreen();
	return widget;
}

}

CMediaPlayer::CMediaPlayer(QWidget *parent):CStbApplication(parent),
	playerIsRunning_(false),cutflg_(false), cutcancel_(false),myTranslator(0),mediaInfoThread(0)
{
	DEBUG_INFO_ONE ("=================================");
    #ifdef BRCM
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(true);
    #endif
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));

    qDebug()<<"create dlna";
    window = new Widget();
    qDebug()<<"stop  create dlna";

	// should be removed at 13.11.2014
	ShareMigration::migrate();

	//	TPROF ("constructor");
	if (Common::getDisplayMode() == DisplayMode::SD)
		sizeM.setDisplayMode (CSizeManager::SD_MODE);
	else
		sizeM.setDisplayMode (CSizeManager::HD_MODE);
    proc = new QProcess(this);

    hideTimer = new QTimer(this);
    hideTimer->setSingleShot(true);
    hideTimer->setInterval(8000);
    connect(hideTimer,SIGNAL(timeout()),this,SLOT(hideControls()));
    
	createTranslater ();

    connect(this, SIGNAL(keybRename(QString)), this, SLOT(runRename(QString)));
    connect(this, SIGNAL(keybNewdir(QString)), this, SLOT(runNewDir(QString)));

    //dlna
    connect(window, SIGNAL(ReadyToPlay(QString)), this, SLOT(ToPlay(QString)));
    connect(window, SIGNAL(WindowStop()), this, SLOT(stopPlayer()));
}

CMediaPlayer::~CMediaPlayer()
{
	DEBUG_INFO();
    destroyShareManager();
	delete aboutContent;
	CLongOperationManager::instance()->deactivate();
	if(copier_->state() == CFileCopierClient::Waiting)
		copier_->shutDown();

	if (myTranslator!=0) {
		QApplication::removeTranslator (myTranslator);
		myTranslator = 0;
	}
    delete window;
}

void CMediaPlayer::ToPlay(QString s)
{
    qDebug()<<"1 ToPlay "<<s;
    QList<Media::SourcePtr> pl;
    pl << Media::createSource(s);

    qDebug()<<"2 ToPlay "<<s;
    player_->player()->setPlayList(pl);
    qDebug()<<"3 ToPlay "<<s;
    player_->player()->play(0);
    qDebug()<<"end ToPlay "<<s;
}

void CMediaPlayer::start()
{
	DEBUG_("1");
	createBackground();

	DEBUG_("2");
	createModel();

	DEBUG_("3");
	createFilterPanel();

	DEBUG_("4");
	createTree();

	DEBUG_("5");
	createCopyPanel();

	DEBUG_("6");
	createCopier();

	DEBUG_("7");
	createActions();

	DEBUG_("8");
	createMediaInfoLabels();

	DEBUG_("9");
	createInfoBar();

	DEBUG_("10");
	createPlayer();

	DEBUG_("11");
	initIndexes();

	DEBUG_("12");
	createShareManager();


	DEBUG_("Exit");
}

void CMediaPlayer::hideControls()
{
    DEBUG_INFO();
    if(aboutContent){   
        aboutContent->hide();
        player_->setFocus();
    }
}

void CMediaPlayer::createTranslater ()
{
	//	TPROF ("createTranslater)");
	//	if (Sett::getValue("system", "language") == "rus") {
	if (Common::getCurrentLanguage() == QLocale::Russian){
		//install translator
		DEBUG_INFO_ONE ("set language: RU");
		myTranslator = new QTranslator;
		myTranslator->load(":/mediaplayer.qm");
		QApplication::installTranslator(myTranslator);
	} else {
		DEBUG_INFO_ONE ("set language: EN");
		myTranslator = 0;
	}
}


void CMediaPlayer::createBackground()
{
	//	TPROF ("createBackgroud ()");
	DEBUG_INFO();
	QString bgFromMenu;
	QString filename = pluginPath_ + "/mediaplayer_desc.txt";
	if(QFile::exists(filename)){
		QFile desc(filename);
		desc.open(QIODevice::ReadOnly);
		QTextStream in(&desc);
		QString str;
		while(!in.atEnd()) {
			str = in.readLine();
			if(str.startsWith("activeBackground"))
				bgFromMenu = str.mid(str.indexOf('=')+1);
			}
		desc.close();
	}
	background_ = new QLabel(this);
	background_->move(0,0);
	background_->setFixedSize(1920,1080);
	if(bgFromMenu == "true")
		background_->setPixmap (QPixmap("/sdk/qt-install-4.7.0/STBGUI_PLUGIN/stbmain_images/background.png"));
	else
		background_->setPixmap(QPixmap(":/mpimg/background.png"));
}

void CMediaPlayer::createFilterPanel()
{
	//	TPROF ("createFilterPanel()");
	DEBUG_INFO();
	label1_ = new QLabel(background_);
	if (sizeM.getMode() == CSizeManager::HD_MODE)
		label1_->move(180,65);
	else
		label1_->move(180,90);
	label1_->setText(tr("Filtering:"));
	label1_->setFont (sizeM.getFont (CSizeManager::_FONT_MED));
	label1_->setStyleSheet (sizeM.getStyle (CSizeManager::_ITEM_LABEL));
	label1_->show();

	int posX = 380;
	const int posY = 139;
	const int heightB = 120;
	const int step = 5;
	const QLatin1String ss("QPushButton:checked { background-color: rgb(39, 135, 183);border-radius: 15px; }");

	QPixmap logoAll(":/mpimg/all.png");
	filterAll_ = new CPushButton(background_);
	filterAll_->move(posX, posY);
	posX += logoAll.size().width() + step;
	filterAll_->setFixedSize(logoAll.size().width(),heightB);
	filterAll_->setCheckable(true);
	filterAll_->setFlat(true);
	filterAll_->setIconSize(logoAll.size());
	//    filterAll_->setIcon(logoAll);
	filterAll_->setStyleSheet(ss);
	filterAll_->setFocusPolicy(Qt::NoFocus);
	filterAll_->hide();
	connect(filterAll_,SIGNAL(clicked()),this,SLOT(disableFilters()));

	QPixmap logoImage(":/mpimg/image.png");
	filterImage_ = new CPushButton(background_);
	filterImage_->move(posX, posY);
	posX += logoImage.size().width() + step;
	filterImage_->setFixedSize(logoImage.size().width(),heightB);
	filterImage_->setCheckable(true);
	filterImage_->setFlat(true);
	filterImage_->setIconSize(logoImage.size());
	//    filterImage_->setIcon(logoImage);
	filterImage_->setStyleSheet(ss);
	filterImage_->setFocusPolicy(Qt::NoFocus);
	filterImage_->hide();
	connect(filterImage_,SIGNAL(clicked()),this,SLOT(enableImageFilter()));

	QPixmap logoMusic(":/mpimg/music.png");
	filterMusic_ = new CPushButton(background_);
	filterMusic_->move(posX, posY);
	posX += logoMusic.size().width() + step;
	filterMusic_->setFixedSize(logoMusic.size().width(),heightB);
	filterMusic_->setCheckable(true);
	filterMusic_->setFlat(true);
	filterMusic_->setIconSize(logoMusic.size());
	//    filterMusic_->setIcon(logoMusic);
	filterMusic_->setStyleSheet(ss);
	filterMusic_->setFocusPolicy(Qt::NoFocus);
	filterMusic_->hide();
	connect(filterMusic_,SIGNAL(clicked()),this,SLOT(enableMusicFilter()));

	QPixmap logoMovie(":/mpimg/movie.png");
	filterMovie_ = new CPushButton(background_);
	filterMovie_->move(posX, posY);
	filterMovie_->setFixedSize(logoMovie.size().width(),heightB);
	filterMovie_->setCheckable(true);
	filterMovie_->setFlat(true);
	filterMovie_->setIconSize(logoMovie.size());
	//    filterMovie_->setIcon(logoMovie);
	filterMovie_->setStyleSheet(ss);
	filterMovie_->setFocusPolicy(Qt::NoFocus);
	filterMovie_->hide();
	connect(filterMovie_,SIGNAL(clicked()),this,SLOT(enableMovieFilter()));

	disableFilters();
}

void CMediaPlayer::deselectAllFilterButtons()
{
	filterAll_->setChecked(false);
	filterImage_->setChecked(false);
	filterMusic_->setChecked(false);
	filterMovie_->setChecked(false);
}

void CMediaPlayer::disableFilters()
{
	DEBUG_INFO();
	deselectAllFilterButtons();
	filterAll_->setChecked(true);
	model_->setNameFilters(QStringList("*"));
	model_->setNameFilterDisables(true);
}

void CMediaPlayer::enableImageFilter()
{
	DEBUG_INFO();
	lastParent_ = tree_->currentIndex().parent();
	deselectAllFilterButtons();
	filterImage_->setChecked(true);
	model_->setNameFilterDisables(false);
	model_->setNameFilters(CFileTypeDetect().getImageList());
}

void CMediaPlayer::enableMovieFilter()
{
	DEBUG_INFO();
	lastParent_ = tree_->currentIndex().parent();
	deselectAllFilterButtons();
	filterMovie_->setChecked(true);
	model_->setNameFilterDisables(false);
	model_->setNameFilters(CFileTypeDetect().getVideoList());
}

void CMediaPlayer::enableMusicFilter()
{
	DEBUG_INFO();
	lastParent_ = tree_->currentIndex().parent();
	deselectAllFilterButtons();
	filterMusic_->setChecked(true);
	model_->setNameFilterDisables(false);
	model_->setNameFilters(CFileTypeDetect().getAudioList());
}

void CMediaPlayer::checkIndexes ()
{
	DEBUG_INFO ();
	DEBUG_INFO_TWO ("currentIndex().isValid: ", tree_->currentIndex().isValid());
	DEBUG_INFO_TWO ("lastParent_.isValid: ", lastParent_.isValid());
	if (!tree_->currentIndex().isValid() && lastParent_.isValid()) {
		tree_->setCurrentIndex (lastParent_);
	}
}

void CMediaPlayer::createModel()
{
	//	TPROF ("createModel()");
	DEBUG_INFO();
	iconProvider_.reset(new CIconProvider);
	model_ = new QFileSystemModel(this);
	model_->setRootPath(FS::rootPath);
	model_->setReadOnly(false);
	model_->setIconProvider(iconProvider_.get());

	//connect(model_,SIGNAL(directoryLoaded(QString)),this,SLOT(dirLoaded(QString)));
}

void drawSelected (QPainter * painter, QRect rect, qreal len, qreal Rad)
{
	painter->save();
	QColor startColor;
	QColor medlleColor;
	QColor endColor;
	int	x = rect.x()-3;
	int y = rect.y();
	int w = rect.width ()+3;
	int h = rect.height ();
	//	qreal Rad = 20;
	//	qreal len = 15;

	startColor = QColor (qRgba(37, 90, 225,255));
	medlleColor = QColor (qRgba(18,45,112,0));
	endColor = Qt::transparent;


	qreal mpoint = 0.7;

	painter->setRenderHint (QPainter::Antialiasing);
	painter->setPen (QPen (QColor (0,0,0,0)));

	QPainterPath path1(QPoint(x+Rad,y));
	path1.arcTo (x,y,2*Rad,2*Rad, 90, 90);
	path1.lineTo (x+len, y+Rad);
	path1.arcTo (x+len, y+len, (Rad-len)*2, (Rad-len)*2, 180, -90);
	path1.lineTo (x+Rad, y);
	QRadialGradient rGrad1 (x+Rad, y+Rad, Rad, x+Rad, y+Rad);
	rGrad1.setColorAt (1, QColor(startColor));
	rGrad1.setColorAt ((mpoint*len/Rad)+(1-len/Rad), QColor(medlleColor));
	rGrad1.setColorAt (1-len/Rad, endColor);
	painter->setBrush (rGrad1);
	painter->drawPath (path1);

	QPainterPath path2(QPoint(x+w,y+Rad));
	path2.arcTo (x+w-2*Rad,y,2*Rad,2*Rad, 0, 90);
	path2.lineTo (x+w-Rad, y+len);
	path2.arcTo (x+w-len-2*(Rad-len), y+len, (Rad-len)*2, (Rad-len)*2, 90, -90);
	path2.lineTo (x+w, y+Rad);
	QRadialGradient rGrad2 (x+w-Rad, y+Rad, Rad, x+w-Rad, y+Rad);
	rGrad2.setColorAt (1, QColor(startColor));
	rGrad2.setColorAt (1-(len*(1-mpoint)/Rad), QColor(medlleColor));
	rGrad2.setColorAt (1-len/Rad, endColor);
	painter->setBrush (rGrad2);
	painter->drawPath (path2);

	QPainterPath path3(QPoint(x+w-Rad,y+h));
	path3.arcTo (x+w-2*Rad,y+h-2*Rad,2*Rad,2*Rad, -90, 90);
	path3.lineTo (x+w-len, y+h-Rad);
	path3.arcTo (x+w-len-2*(Rad-len), y+h-len-2*(Rad-len), (Rad-len)*2, (Rad-len)*2, 0, -90);
	path3.lineTo (x+w-Rad, y+h);
	QRadialGradient rGrad3 (x+w-Rad, y+h-Rad, Rad, x+w-Rad, y+h-Rad);
	rGrad3.setColorAt (1, QColor(startColor));
	rGrad3.setColorAt (1-(len*(1-mpoint)/Rad), QColor(medlleColor));
	rGrad3.setColorAt (1-len/Rad, endColor);
	painter->setBrush (rGrad3);
	painter->drawPath (path3);

	QPainterPath path4(QPoint(x,y+h-Rad));
	path4.arcTo (x,y+h-2*Rad,2*Rad,2*Rad, 180, 90);
	path4.lineTo (x+Rad, y+h-len);
	path4.arcTo (x+len, y+h-len-2*(Rad-len), (Rad-len)*2, (Rad-len)*2, -90, -90);
	path4.lineTo (x, y+h-Rad);
	QRadialGradient rGrad4 (x+Rad, y+h-Rad, Rad, x+Rad, y+h-Rad);
	rGrad4.setColorAt (1, QColor(startColor));
	rGrad4.setColorAt (1-(len*(1-mpoint)/Rad), QColor(medlleColor));
	rGrad4.setColorAt (1-len/Rad, endColor);
	painter->setBrush (rGrad4);
	painter->drawPath (path4);


	QLinearGradient grad1(x, y + len, x+0.0005, y);
	grad1.setColorAt(1, QColor(startColor));
	grad1.setColorAt(mpoint, QColor(medlleColor));
	grad1.setColorAt(0, endColor);
	painter->fillRect(x + Rad, y, w-2*Rad, len, grad1);

	QLinearGradient grad3(x+len, y, x, y);
	grad3.setColorAt(1, QColor(startColor));
	grad3.setColorAt(mpoint, QColor(medlleColor));
	grad3.setColorAt(0, endColor);
	painter->fillRect(x, y+Rad, len, h-2*Rad, grad3);

	QLinearGradient grad2(x+Rad, y+h-len, x+Rad+0.0005, y+h);
	grad2.setColorAt(1, QColor(startColor));
	grad2.setColorAt(mpoint, QColor(medlleColor));
	grad2.setColorAt(0, endColor);
	painter->fillRect(x + Rad, y+h-len, w-2*Rad, len, grad2);

	QLinearGradient grad4(x+w-len, y, x+w, y);
	grad4.setColorAt(1, QColor(startColor));
	grad4.setColorAt(mpoint, QColor(medlleColor));
	grad4.setColorAt(0, endColor);
	painter->fillRect(x+w-len, y+Rad, len, h-2*Rad, grad4);
	painter->restore();
}

//*****************************************************************************
void SelectedItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	painter->save();
	QFontMetrics fm (painter->font());
	if(option.state & QStyle::State_Selected){
		QColor startColor;
		QColor medlleColor;
		QColor endColor;
		int	x = option.rect.x()-3;
		int y = option.rect.y();
		int w = option.rect.width ()+3;
		int h = option.rect.height ();
		qreal Rad = 20;
		qreal len = 15;

		startColor = QColor (qRgba(37, 90, 225,255));
		medlleColor = QColor (qRgba(18,45,112,0));
		endColor = Qt::transparent;


		qreal mpoint = 0.7;

		painter->setRenderHint (QPainter::Antialiasing);
		painter->setPen (QPen (QColor (0,0,0,0)));

		QPainterPath path1(QPoint(x+Rad,y));
		path1.arcTo (x,y,2*Rad,2*Rad, 90, 90);
		path1.lineTo (x+len, y+Rad);
		path1.arcTo (x+len, y+len, (Rad-len)*2, (Rad-len)*2, 180, -90);
		path1.lineTo (x+Rad, y);
		QRadialGradient rGrad1 (x+Rad, y+Rad, Rad, x+Rad, y+Rad);
		rGrad1.setColorAt (1, QColor(startColor));
		rGrad1.setColorAt ((mpoint*len/Rad)+(1-len/Rad), QColor(medlleColor));
		rGrad1.setColorAt (1-len/Rad, endColor);
		painter->setBrush (rGrad1);
		painter->drawPath (path1);

		QPainterPath path2(QPoint(x+w,y+Rad));
		path2.arcTo (x+w-2*Rad,y,2*Rad,2*Rad, 0, 90);
		path2.lineTo (x+w-Rad, y+len);
		path2.arcTo (x+w-len-2*(Rad-len), y+len, (Rad-len)*2, (Rad-len)*2, 90, -90);
		path2.lineTo (x+w, y+Rad);
		QRadialGradient rGrad2 (x+w-Rad, y+Rad, Rad, x+w-Rad, y+Rad);
		rGrad2.setColorAt (1, QColor(startColor));
		rGrad2.setColorAt (1-(len*(1-mpoint)/Rad), QColor(medlleColor));
		rGrad2.setColorAt (1-len/Rad, endColor);
		painter->setBrush (rGrad2);
		painter->drawPath (path2);

		QPainterPath path3(QPoint(x+w-Rad,y+h));
		path3.arcTo (x+w-2*Rad,y+h-2*Rad,2*Rad,2*Rad, -90, 90);
		path3.lineTo (x+w-len, y+h-Rad);
		path3.arcTo (x+w-len-2*(Rad-len), y+h-len-2*(Rad-len), (Rad-len)*2, (Rad-len)*2, 0, -90);
		path3.lineTo (x+w-Rad, y+h);
		QRadialGradient rGrad3 (x+w-Rad, y+h-Rad, Rad, x+w-Rad, y+h-Rad);
		rGrad3.setColorAt (1, QColor(startColor));
		rGrad3.setColorAt (1-(len*(1-mpoint)/Rad), QColor(medlleColor));
		rGrad3.setColorAt (1-len/Rad, endColor);
		painter->setBrush (rGrad3);
		painter->drawPath (path3);

		QPainterPath path4(QPoint(x,y+h-Rad));
		path4.arcTo (x,y+h-2*Rad,2*Rad,2*Rad, 180, 90);
		path4.lineTo (x+Rad, y+h-len);
		path4.arcTo (x+len, y+h-len-2*(Rad-len), (Rad-len)*2, (Rad-len)*2, -90, -90);
		path4.lineTo (x, y+h-Rad);
		QRadialGradient rGrad4 (x+Rad, y+h-Rad, Rad, x+Rad, y+h-Rad);
		rGrad4.setColorAt (1, QColor(startColor));
		rGrad4.setColorAt (1-(len*(1-mpoint)/Rad), QColor(medlleColor));
		rGrad4.setColorAt (1-len/Rad, endColor);
		painter->setBrush (rGrad4);
		painter->drawPath (path4);


		QLinearGradient grad1(x, y + len, x+0.0005, y);
		grad1.setColorAt(1, QColor(startColor));
		grad1.setColorAt(mpoint, QColor(medlleColor));
		grad1.setColorAt(0, endColor);
		painter->fillRect(x + Rad, y, w-2*Rad, len, grad1);

		QLinearGradient grad3(x+len, y, x, y);
		grad3.setColorAt(1, QColor(startColor));
		grad3.setColorAt(mpoint, QColor(medlleColor));
		grad3.setColorAt(0, endColor);
		painter->fillRect(x, y+Rad, len, h-2*Rad, grad3);

		QLinearGradient grad2(x+Rad, y+h-len, x+Rad+0.0005, y+h);
		grad2.setColorAt(1, QColor(startColor));
		grad2.setColorAt(mpoint, QColor(medlleColor));
		grad2.setColorAt(0, endColor);
		painter->fillRect(x + Rad, y+h-len, w-2*Rad, len, grad2);

		QLinearGradient grad4(x+w-len, y, x+w, y);
		grad4.setColorAt(1, QColor(startColor));
		grad4.setColorAt(mpoint, QColor(medlleColor));
		grad4.setColorAt(0, endColor);
		painter->fillRect(x+w-len, y+Rad, len, h-2*Rad, grad4);

	}
	QPen pen = QPen(sizeM.getColor(CSizeManager::COLOR_NORMAL));
	painter->setPen(pen);
	QPixmap icon (index.data(Qt::DecorationRole).value<QIcon>().pixmap(QSize(64,64)));
	QTextOption opt;
	opt.setWrapMode (QTextOption::NoWrap);
	if(index.data().toString().size() > 38){
		painter->drawText (option.rect.adjusted(icon.width()+10,(option.rect.height()-fm.height())/2,-1,-1), index.data().toString().mid(0, 38)+"...", opt);
	}
	else{
		painter->drawText (option.rect.adjusted(icon.width()+10,(option.rect.height()-fm.height())/2,-1,-1), index.data().toString(), opt);
	}
	const QFileSystemModel *model = static_cast<const QFileSystemModel*>(index.model());
	if(model->fileInfo(index).absoluteFilePath() == FS::network) {
		if(mountState == CMediaPlayer::MOUNT_START)
			painter->drawText (option.rect.adjusted(icon.width()+10,(option.rect.height()-fm.height())/2,-1,-1), index.data().toString()+" (" + tr("Mounting network drives...") + ")", opt);
	}
	painter->drawPixmap (QRect(option.rect.x(), option.rect.y(),option.rect.height(),option.rect.height()), icon);
	painter->restore();
}


void CMediaPlayer::createTree()
{
	DEBUG_INFO();
	tree_ = new CTreeView(background_);
	tree_->setModel(model_);
	tree_->setRootIndex(model_->index(FS::rootPath));

	// Demonstrating look and feel features
	tree_->setIconSize(QSize(64,64));
	tree_->setAnimated(false);
	tree_->setIndentation(40);
	tree_->setSortingEnabled(true);
	tree_->sortByColumn(0,Qt::AscendingOrder);
	//    tree_->setGeometry(120,290,1320,680);
	if (sizeM.getMode () == CSizeManager::HD_MODE)
		tree_->setGeometry (120,150,1320,830);
	else
		tree_->setGeometry (330,150,950,780);
	tree_->setFont(sizeM.getFont(CSizeManager::_FONT_BIG));
	tree_->setColumnHidden( 3, true );
	tree_->setColumnHidden( 2, true );
	tree_->setColumnHidden( 1, true );
	tree_->setHeaderHidden(true);
	tree_->setColumnWidth ( 0, 950 );
	tree_->setUniformRowHeights(true);
	tree_->setEditTriggers(0);			//do not change item name
	//    tree_->setStyleSheet(treeStyleSheet);
	tree_->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_TREEVIEW));
	//    tree_->verticalScrollBar()->setStyleSheet(verticalScrollBarStyleSheet);
	//    tree_->horizontalScrollBar()->setStyleSheet(horizontalScrollBarStyleSheet);
	tree_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tree_->setItemDelegate (new SelectedItemDelegate);

	connect(tree_, SIGNAL(clicked(QModelIndex)), this, SLOT(treeClicked(QModelIndex)));
	connect(tree_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(treeDoubleClicked(QModelIndex)));
	connect(tree_, SIGNAL(activated(QModelIndex)), this, SLOT(treeClicked(QModelIndex)));
	connect(tree_, SIGNAL(expanded(QModelIndex)), this, SLOT(updateInfoBar(QModelIndex)));
	connect(tree_, SIGNAL(collapsed(QModelIndex)), this, SLOT(updateInfoBar(QModelIndex)));
	connect(tree_, SIGNAL(collapsed(QModelIndex)), this, SLOT(updateInfoBar(QModelIndex)));
	connect(tree_, SIGNAL(ivalidSelectionIndex()), this, SLOT(updateSelection()));
	connect(tree_, SIGNAL(expanded(QModelIndex)), this, SLOT(itemExpanded(QModelIndex)));
	//	connect (tree_->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), tree_, SLOT(animateScroll(QModelIndex,QModelIndex)));
	tree_->show();
}

void CMediaPlayer::createCopier()
{
	DEBUG_INFO();
	copier_ = new CFileCopierClient(this);
	connectToFileCopier();
}

void CMediaPlayer::connectToFileCopier()
{
	DEBUG_INFO();
	DEBUG_("1");
//	connect(copier_,SIGNAL(progress(int)),copyPanel_,SLOT(show()));
	connect(copier_,SIGNAL(progress(int)),copyPanel_,SLOT(setValue(int)));

	DEBUG_("2");
	connect(copier_,SIGNAL(done()),copyPanel_,SLOT(hide()));
	connect(copier_,SIGNAL(done()),this,SLOT(rmCopyed()));
	DEBUG_("3");
	connect(copier_,SIGNAL(currentCopyingFName(QString)),copyPanel_,SLOT(setFileName(QString)));
	DEBUG_("4");
	connect(copier_, SIGNAL(process()), this, SLOT(checkState()));
//	QTimer::singleShot(3200, this, SLOT(checkState()));
	if(copier_->state() == CFileCopierClient::Working)
	{
		DEBUG_("5");
		copyPanel_->show();
	}
	DEBUG_("6");
}
void CMediaPlayer::checkState()
{
	DEBUG_INFO_TWO("state copier = ", copier_->state());
	if(copier_->state() == 1)
		copyPanel_->show();
	else
		copyPanel_->hide();
}

void CMediaPlayer::rmCopyed()
{
	if(cutflg_ && !cutcancel_){
		QString str = "rm -r \""+ model_->fileInfo(copyIndex_).absoluteFilePath()+"\"";
		system(str.toUtf8().constData());
        system("sync");
		cutflg_ = false;
	}
	cutcancel_ = false;
}

void CMediaPlayer::updateSelection ()
{
	DEBUG_INFO ();
	tree_->setCurrentIndex(model_->index(FS::local));
}

class CMenuItem : public QLabel {
	//	static int itemWidth;
	QPixmap pix_;
	QString text_;
public:
	CMenuItem (QString text, QWidget * parent) : QLabel (parent), text_(text) {
		setFocusPolicy (Qt::StrongFocus);
		setFixedSize (300, 50);
	};
	void setPixmap (QPixmap pix) {pix_ = pix;}
	virtual QSize sizeHint () const {
		QFontMetrics fm (sizeM.getFont(CSizeManager::_FONT_MED));
		return QSize (fm.width(text_)+50,50);
	}
protected:
	void paintEvent (QPaintEvent * event) {
		Q_UNUSED(event)
		QPainter painter (this);
		if (hasFocus()) {
			drawSelected (&painter, rect(),12,12);
		}
		painter.drawPixmap (QRect(2,5,38,38),pix_);
		painter.setFont (sizeM.getFont(CSizeManager::_FONT_MED));
		painter.setPen (QPen (QColor (Qt::white)));
		QTextOption opt;
		opt.setWrapMode (QTextOption::NoWrap);
		painter.drawText (rect().adjusted(45,0,0,0),text_,opt);
	}
};

void CMediaPlayer::createActions()
{
	DEBUG_INFO();
	CMenuItem * item = new CMenuItem (tr("Exit"), this);
	actionExit_ = new QWidgetAction (this); // to exit from context menu
	actionExit_->setDefaultWidget (item);

	actionNetworkShares_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Network resources"), this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/share.png"));
	actionNetworkShares_->setDefaultWidget (item);
	connect(actionNetworkShares_, SIGNAL(triggered()), this, SLOT(networkSharesActHandler()));

	actionNetworkStream_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Network stream"), this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/stream.png"));
	actionNetworkStream_->setDefaultWidget (item);
	connect(actionNetworkStream_, SIGNAL(triggered()), this, SLOT(networkStreamActHandler()));

	actionCopy_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Copy"), this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/copy.png"));
	actionCopy_->setDefaultWidget (item);
	connect(actionCopy_, SIGNAL(triggered()),this,SLOT(copyActHandler()));

	actionCut_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Cut"), this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/cut.png"));
	actionCut_->setDefaultWidget (item);
	connect(actionCut_, SIGNAL(triggered()),this,SLOT(cutActHandler()));

	actionDelete_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Delete"), this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/delete.png"));
	actionDelete_->setDefaultWidget (item);
	connect(actionDelete_,			SIGNAL(triggered()),this,SLOT(deleteActHandler()));

	actionPaste_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Paste"), this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/paste.png"));
	actionPaste_->setDefaultWidget (item);
	actionPaste_->setEnabled(false);
	actionPaste_->setVisible(false);
	connect(actionPaste_,			SIGNAL(triggered()),this,SLOT(pasteActHandler()));

	actionPlay_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Play"), this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/play.png"));
	actionPlay_->setDefaultWidget (item);
	connect(actionPlay_,			SIGNAL(triggered()),this,SLOT(prepareToPlay()));

	actionPlayWithSubtitle_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Play with subtitle"),this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/playws.png"));
	actionPlayWithSubtitle_->setDefaultWidget (item);
	connect(actionPlayWithSubtitle_,SIGNAL(triggered()),this,SLOT(playWithSubtitleHandler()));

	actionRename_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Rename"),this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/newdir.png"));
	actionRename_->setDefaultWidget (item);
	connect(actionRename_,		SIGNAL(triggered()),this,SLOT(renameActHandler()));

	actionAddBookmark_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Add Bookmark"),this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/bookmark.png"));
	actionAddBookmark_->setDefaultWidget (item);
	connect(actionAddBookmark_,		SIGNAL(triggered()),this,SLOT(addBookmarkActHandler()));

	actionNewDir_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Create new directory"),this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/newdir.png"));
	actionNewDir_->setDefaultWidget (item);
	connect(actionNewDir_,			SIGNAL(triggered()),this,SLOT(newDirActHandler()));

	actionStopCopying_ = new QWidgetAction (this);
	item = new CMenuItem (tr("Terminate copying"),this);
	item->setPixmap (QPixmap(":/mpimg/contexticons/termcopy.png"));
	actionStopCopying_->setDefaultWidget (item);
	connect(actionStopCopying_,SIGNAL(triggered()),copier_,SLOT(stop()));
	connect(actionStopCopying_,SIGNAL(triggered()),this,SLOT(hideCopypanel()));
}

void CMediaPlayer::prepareToPlay(){
	QTimer::singleShot(10, this, SLOT(playActHandler()));
}

void CMediaPlayer::hideCopypanel()
{
	copyPanel_->hide();
	if(cutflg_) cutcancel_ = true;
}

void CMediaPlayer::createCopyPanel()
{
	copyPanel_ = new CCopyPanel(background_);
	copyPanel_->setGeometry(995,145,1000,115);
	copyPanel_->hide();
	//copyPanel->show();
	copyPanel_->setMaximum(1000);
	copyPanel_->setMinimum(0);
}

void CMediaPlayer::createMediaInfoLabels()
{
	wrongMessage = new QLabel(this);
	const QString mesboxStyleSheet =
			"QLabel {"
			"border: 3px solid rgb(37, 90, 225);"
//			"border-radius: 10px;"
			"background-color:rgb(0,0,0, 255);"
			"}";
//	wrongMessage->setFixedHeight(80);
	wrongMessage->setStyleSheet(mesboxStyleSheet);
	wrongMessage->setWindowFlags(Qt::FramelessWindowHint);
	wrongMessage->setFont(QFont ("Century Gothic", 48, QFont::Normal));
	wrongMessage->move(700, 400);
	wrongMessageVisible = false;

	aboutContent = new CAboutContent (this);
	aboutContent->hide();
//	aboutContent->setFixedWidth (900);
	aboutContent->move (100,100);

	mediaInfoBackground	= new CInfoMessageBackground (Qt::transparent, background_);
	mediaInfoBackground->setBorderEnabled(true);
	mediaInfoBackground->hide();
	if (sizeM.getMode() == CSizeManager::HD_MODE) {
		mediaInfoBackground->setGeometry (750,100,1100,0);
		mediaInfoBackground ->setBorderWidth(2);
		mediaInfoBackground ->setBorderColor(QColor(37, 90, 225,255));
	} else {
		mediaInfoBackground->setGeometry (400,200,1100,0);
		mediaInfoBackground ->setBorderWidth(10);
		mediaInfoBackground ->setBorderColor(QColor(0x888888));
	}
	mediaInfoBackground ->setBorderSize(10);
//	mediaInfoBackground ->setBorderRadius(15);

	DEBUG_INFO();
	for(size_t i=0;i< COUNT_OF_INFO_LABELS;++i)	{
		label[i] = new QLabel(mediaInfoBackground);
		if (sizeM.getMode() == CSizeManager::HD_MODE)
			label[i]->setGeometry(1470,300+i*50,400,50);
		else
			label[i]->setGeometry(1300,300+i*50,400,50);
		label[i]->show();
		label[i]->setFont (sizeM.getFont(CSizeManager::_FONT_MED));
		label[i]->setStyleSheet (sizeM.getStyle (CSizeManager::_ITEM_LABEL));
	}
}

void CMediaPlayer::createInfoBar()
{
	//	TPROF ("createInfoBar");
	DEBUG_INFO();
	infoBar_ = new CInfoBar(background_);
	if (sizeM.getMode() == CSizeManager::HD_MODE)
		infoBar_->move(150,1000);
	else
		infoBar_->move(350,950);

	infoBar_->setFixedSize(1770,80);
	infoBar_->setFont (sizeM.getFont(CSizeManager::_FONT_MED));
	infoBar_->show();
	infoBar_->setButtonsMask (CInfoBar::B_OK |
							  CInfoBar::B_RETURN |
							  CInfoBar::B_MENU);

	infoBar_->setButtonDescription(CInfoBar::B_OK,tr("Open"));
	infoBar_->setButtonDescription(CInfoBar::B_RETURN,tr("Exit"));
	infoBar_->setButtonDescription(CInfoBar::B_MENU,tr("Menu"));
	infoBar_->setupView ();

	infoBarColored_ = new CInfoBar(background_);
	if (sizeM.getMode() == CSizeManager::HD_MODE)
		infoBarColored_->move(350,50);
	else
		infoBarColored_->move(350,80);

	infoBarColored_->setFixedSize(1770,80);
	infoBarColored_->setFont (sizeM.getFont(CSizeManager::_FONT_MED));
	infoBarColored_->show();
	infoBarColored_->setButtonsMask (CInfoBar::B_RED |
								  CInfoBar::B_GREEN |
								  CInfoBar::B_BLUE |
								  CInfoBar::B_YELLOW);

	infoBarColored_->setButtonDescription(CInfoBar::B_RED,tr("Filter off"));
	infoBarColored_->setButtonDescription(CInfoBar::B_GREEN,tr("Images"));
	infoBarColored_->setButtonDescription(CInfoBar::B_YELLOW,tr("Music"));
	infoBarColored_->setButtonDescription(CInfoBar::B_BLUE,tr("Movies"));
	infoBarColored_->setupView();
	infoBarColored_->setChecked(CInfoBar::B_RED);

}

void CMediaPlayer::createPlayer()
{
	//	TPROF ("createPlayer()");
    progressBox = new CLoadProgressBar(this);
	connect(progressBox, SIGNAL(returnPressed()), this, SLOT(stopPlayer()));
	connect(this, SIGNAL(progressStep(int)), progressBox, SLOT(progressChanged(int)));

	DEBUG_INFO();
	using namespace Media;
	player_ = new CCompletePlayer(CCompletePlayer::Progress | CCompletePlayer::Volume |
								  CCompletePlayer::TrackList | CCompletePlayer::TimeSearch | CCompletePlayer::ZoomPanel,
								  this);
	player_->move(0,0);
	player_->setFixedSize(1920,1080);
	player_->hide();
	QSettings set(localSettingsFilename, QSettings::NativeFormat);
	bool ok = false;
	int lastVolume = set.value("mediaplayer/volume").toInt(&ok);
	if(ok)	{
		player_->player()->setVolume(lastVolume);
	}
	connect(player_->player(),SIGNAL(statusChanged(Media::CPlayer::EStatus)),this,SLOT(playerStatusChanged()));
	connect(player_->player(),SIGNAL(currentPlayingItemChanged(int)),this,SLOT(currentItemChanged(int)));
	connect(player_->player(), SIGNAL(p2pProgressChanged(int)), this, SLOT(progressChanged(int)));
	connect(player_->player(),SIGNAL(volumeChanged(int)),this,SLOT(volumeChanged(int)));
	player_->installEventFilter(this);
}

void CMediaPlayer::progressChanged(int x)
{
//	progressBox->show();
	if(/*!progressBox->isVisible() && */playerIsRunning_){
//		progressBox->exec();
		progressBox->show();
	}
	emit progressStep(x);
}
void CMediaPlayer::stopPlayer()
{
	if(!progressBox->isVisible()){
		progressBox->hide();
		//progressBox->deleteLater();
	}
//	disconnect(player_->player(), SIGNAL(p2pProgressChanged(int)), this, SLOT(progressChanged(int)));
	player_->player()->stop();
	player_->hide();
	playerIsRunning_ = false;
	background_->show();
	tree_->setFocus();
}

void CMediaPlayer::currentItemChanged(int ind)
{
	DEBUG_INFO();
	Media::SourcePtr p = player_->player()->playList().at(ind);
	tree_->scrollTo(model_->index(p->url()),QAbstractItemView::PositionAtCenter);
	tree_->setCurrentIndex(model_->index(p->url()));
}

void CMediaPlayer::initIndexes()
{
	//	TPROF ("initIndexes");
	selectedIndex_ = model_->index(FS::rootPath);
	lastSelectedIndex_ = selectedIndex_;
	nestingLevel_ = getNestingLevel(selectedIndex_);
	minNestingLevelForLocalWork_ = nestingLevel_ + 2;
	minNestingLevelForNetWork_ = nestingLevel_ + 3;
	minNestingLevelForPaylistsWork_ = nestingLevel_ + 1;
	tree_->setCurrentIndex(model_->index(FS::local));
	connect (model_, SIGNAL(layoutChanged()), this, SLOT (checkIndexes()));
}

void CMediaPlayer::createMediaInfo(const QString &file)
{
	if(!QFileInfo(file).isFile()) {
		DEBUG_ ("it is not file");
		return;
	}
	if(mediaInfoThread) {
		DEBUG_ ("thread alrady running");
		return;
	}
	if(QFile::exists(mediainfoSymLink))	{
		DEBUG_INFO_ONE("deleting symlink");
		if(unlink(mediainfoSymLink.toUtf8().data()))	{
			ERROR_INFO_ONE("symlink deleting fail");
			return;
		}
	}
	if(symlink(file.toUtf8().data(),mediainfoSymLink.toUtf8().data()))	{
		ERROR_INFO_ONE("symlink creation fail");
		return;
	}

    QString command = mediainfoCmd + " --Output=XML --Inform=file://" + mediainfoConfig + " --LogFile="
			+ mediainfoOutFile + " \"" + mediainfoSymLink + "\"";
	DEBUG_INFO_ONE(command);
	mediaInfoThread = new CSysCommandThread(this,command);
	connect(mediaInfoThread,SIGNAL(finished()),this,SLOT(showMediaInfo()));
	mediaInfoThread->start();
}

void CMediaPlayer::showMediaInfo()
{
	DEBUG_INFO();
	disconnect(mediaInfoThread,SIGNAL(finished()),this,SLOT(showMediaInfo()));
	delete mediaInfoThread;
	mediaInfoThread=0;

	if(QFile::exists(mediainfoSymLink))	{
		DEBUG_INFO_ONE("deleting symlink");
		if(unlink(mediainfoSymLink.toUtf8().data()))	{
			ERROR_INFO_ONE("symlink deleting fail");
		}
	}
	QString str = model_->fileName(selectedIndex_);
	if(str.size() > 50){
		str = str.mid(0, 50) + "...";
	}
	str += "<br>";
	std::for_each(label,label+COUNT_OF_INFO_LABELS,std::mem_fun(&QLabel::hide));
	QFile file(mediainfoOutFile);
	if(file.exists(mediainfoOutFile) && file.size() > 0)	{
		//		tree->setFixedWidth(700);
		CMediaInfoParser media(mediainfoOutFile);
		const std::list<QString>& out = media.outRef();
		std::list<QString>::const_iterator it=out.begin();
		treeClicked(selectedIndex_);	
		label[0]->setGeometry(25,20,1020,50);
		label[0]->setText(label[0]->text() + *it);
		label[0]->show();
		lastInfo_ = label[0]->text();
		str += label[0]->text();
		str += " <br>";
		it++;
		if(it==out.end()){
			mediaInfoBackground->setFixedHeight(100);
			aboutContent->setFixedHeight(120);
			for(int j=1; j<COUNT_OF_INFO_LABELS; ++j)
				label[j]->clear();
		}
		for(size_t i=1; i<COUNT_OF_INFO_LABELS && it!=out.end();++i,++it)	{
			//			label[i]->setGeometry(850,300+i*50,1020,50);
			label[i]->setGeometry(25,20+i*50,1020,50);
			label[i]->setText(*it);
			str = str + *it + " <br>";
			label[i]->show();
			mediaInfoBackground->setFixedHeight (100+i*50);
			aboutContent->setFixedHeight(100+i*50);
		}
		aboutContent->setText(str);
		if(playerIsRunning_){
			aboutContent->setFixedWidth(1020);
			aboutContent->raise();
			aboutContent->setFocus();
			aboutContent->show();
            hideTimer->start();
		}
		else
			mediaInfoBackground->show();
	}
}

void CMediaPlayer::hideMediaInfo()
{
	DEBUG_INFO();
	if(!wrongMessageVisible)
		wrongMessage->hide();
	wrongMessageVisible = false;
	mediaInfoBackground->hide();
}

bool CMediaPlayer::eventFilter(QObject *obj, QEvent *event)
{
	if(obj == player_) {
		if(event->type() == QEvent::FocusIn){
			if(aboutContent->isVisible()){
				DEBUG_INFO_TWO("          FocusIn ", event->type());
				aboutContent->setFocus();
			}
		}
	}
	return QObject::eventFilter(obj, event);
}

void CMediaPlayer::keyPressEvent(QKeyEvent *event)
{
	DEBUG_INFO_THREE(std::hex,event->key(),std::dec);
	if(playerIsRunning_)   {
		if(event->key() == Qt::Key_Escape)  {
            if(!aboutContent->hasFocus()){
                DEBUG_INFO_ONE("IS NOT VISIBLE ");
                player_->player()->stop();
            }
            else{
                aboutContent->hide();
                aboutContent->clearFocus();
            }
		} else {
//			aboutContent->clearFocus();
			player_->setFocus();
			if(event->key() == Qt::Key_F4){
				if(lastSelectedIndex_ == selectedIndex_){
				aboutContent->raise();
				aboutContent->setFocus();
				aboutContent->show();
				hideTimer->start();
				return;
			      }
				else{
				lastSelectedIndex_ = selectedIndex_;
				createMediaInfo(model_->filePath(selectedIndex_));
			      }
			}
		}
		return;
	}
	switch(event->key())	{

	case Qt::Key_F5:
		WidgetGrabber::grab(this);
		break;
		//case Qt::Key_Backspace:
	case Qt::Key_Escape:
		//TODO uncomment
		//player_->player()->stop();
		if (mediaInfoBackground->isVisible() || wrongMessage->isVisible()) {
			DEBUG_INFO_ONE ("visible");
			hideMediaInfo ();
		}
		else {
			DEBUG_INFO_ONE ("hidden");
			doExit();
		}
		break;
	case Qt::Key_F9:
		filterAll_->click();
		infoBarColored_->setChecked(CInfoBar::B_RED);
		break;
	case Qt::Key_F10:
		filterImage_->click();
		infoBarColored_->setChecked(CInfoBar::B_GREEN);
		break;
	case Qt::Key_F11:
		filterMusic_->click();
		infoBarColored_->setChecked(CInfoBar::B_YELLOW);
		break;
	case Qt::Key_F12:
		filterMovie_->click();
		infoBarColored_->setChecked(CInfoBar::B_BLUE);
		break;
	case Qt::Key_Super_L:
	{
		QRect listRect(tree_->visualRect(tree_->currentIndex()));
		int indent = 210;
		QContextMenuEvent cmevent(QContextMenuEvent::Mouse,QPoint(listRect.x(),0),QPoint(listRect.x()+indent*2,listRect.y()+indent));
		hideMediaInfo ();

		//runFromKeyboard = true;
		contextMenuEvent(&cmevent);
	}
		break;
	case Qt::Key_MediaPlay:
		playActHandler();
		break;
	case Qt::Key_F4:
		DEBUG_INFO_ONE("Key_F4");
		if(lastSelectedIndex_ == selectedIndex_){
		    if(mediaInfoBackground->isVisible())
		      mediaInfoBackground->hide();
			else{
				label[0]->setText(lastInfo_);
		      mediaInfoBackground->show ();
			}
		    return;
		  }
		else{
		    DEBUG_INFO_ONE("Key_F4 new parse");		    
		    lastSelectedIndex_ = selectedIndex_;
		createMediaInfo(model_->filePath(selectedIndex_));
		  }
		break;

		////	case Qt::Key_B:
		////			infoBar_->setButtonsMask(0x3);
		////		break;
	case Qt::Key_F1:
		RClayout::switchNext();
		break;

	case Qt::Key_Dead_Caron:
		WARNING_INFO_ONE("starting /sdk/config/scripts/mount_res.sh");
		if(!QProcess::startDetached(QLatin1String("/sdk/config/scripts/mount_res.sh"),QStringList() << QLatin1String("mount")))
			ERROR_("Cannot run /sdk/config/scripts/mount_res.sh");
		break;

	default:
		QWidget::keyPressEvent(event);
	}
	hideMediaInfo();
}

void CMediaPlayer::contextMenuEvent(QContextMenuEvent *event)
{
	DEBUG_INFO();
	int minNestingLevelForWork=0;
	ETreeType type = folderType(selectedIndex_);
	bool ismounted = true;

	switch(type) {
	case LOCAL:
		minNestingLevelForWork = minNestingLevelForLocalWork_;
		break;
	case NETWORK:
		minNestingLevelForWork = minNestingLevelForNetWork_;
		if (!isResourceMounted(model_->filePath(selectedIndex_)))	{
			ERROR_INFO_THREE("network folder:", model_->filePath(selectedIndex_),"is not mounted. Context menu denied");
			ismounted = false;
		}
		break;
	case PLAYLISTS:
		minNestingLevelForWork = minNestingLevelForPaylistsWork_;
		break;
	case UNKNOWN:
		ERROR_INFO_ONE("folder type is unknown");
		return;
		break;
	}

	if(nestingLevel_< minNestingLevelForWork && type != NETWORK)
	{
		//		QMessageBox::critical(this,"Warning","Please select the folder available to work!!!");
		return;
	}

	const QString contextMenuStyleSheet =
			"QMenu {"
			"border: 3px solid rgb(37, 90, 225);"
			"border-radius: 10px;"
			"background-color:rgb(0,0,0, 255);"
			"}";

	QMenu contextMenu;
	QList<QWidgetAction*> actList;
	contextMenu.setStyleSheet(contextMenuStyleSheet);//(sizeM.getStyle (CSizeManager::_ITEM_CONTEXTMENU));
	contextMenu.setFont(sizeM.getFont(CSizeManager::_FONT_MED));
	actList.push_back(actionExit_);
	//	contextMenu.addAction(actionExit_);
	//	actionExit_->defaultWidget()->setFocus();

	if (copier_->state() == CFileCopierClient::Working)	{
		//		contextMenu.addAction(actionStopCopying_);
		actList.push_back (actionStopCopying_);
	}

	switch(type) {
	case LOCAL:
	case NETWORK:
		if(type == NETWORK){
			actList.push_back(actionNetworkShares_);
			actList.push_back(actionNetworkStream_);
		}

		if(!ismounted)
			break;

		if(nestingLevel_ > minNestingLevelForWork){
			//			contextMenu.addAction(actionCopy_);
			//			contextMenu.addAction(actionCut_);
			//			contextMenu.addAction(actionDelete_);
			actList.push_back(actionCopy_);
			actList.push_back(actionCut_);
			actList.push_back(actionDelete_);
			actList.push_back(actionRename_);

		}
		//		contextMenu.addAction(actionPaste_);
		//		contextMenu.addAction(actionNewDir_);
		//		contextMenu.addAction(actionPlay_);
		actList.push_back(actionCopy_);
//        if(model_->fileInfo(selectedIndex_).absoluteDir() != model_->fileInfo(copyIndex_).absoluteDir())
        if(actionPaste_->isEnabled()){
            if(model_->fileInfo(selectedIndex_).isDir()){
                DEBUG_INFO_TWO("Dir1 = ", model_->fileInfo(selectedIndex_).absoluteFilePath());            
                if(model_->fileInfo(copyIndex_).absolutePath() != model_->fileInfo(selectedIndex_).absoluteFilePath())
                    actList.push_back(actionPaste_);     
            }
            else if(!model_->fileInfo(selectedIndex_).isDir()){
				if(copyIndex_.isValid())
					if(model_->fileInfo(copyIndex_).absolutePath() != model_->fileInfo(selectedIndex_).absolutePath())
						actList.push_back(actionPaste_);
            }
        }

//        DEBUG_INFO_TWO("Dir22 = ", model_->fileInfo(copyIndex_).absolutePath());
//        actList.push_back(actionPaste_);
		actList.push_back(actionNewDir_);
		actList.push_back(actionPlay_);
		if(!model_->isDir(selectedIndex_) && FileTypes::isVideoFile(model_->fileInfo(selectedIndex_)) )
			//			contextMenu.addAction(actionPlayWithSubtitle_);
			actList.push_back(actionPlayWithSubtitle_);
		//		contextMenu.addAction(actionSlideShow_);
		//contextMenu.addAction(actionAddBookmark_);
		break;
	case PLAYLISTS:
		//		contextMenu.addAction(actionCopy_);
		//		contextMenu.addAction(actionCut_);
		//		contextMenu.addAction(actionDelete_);
		//		contextMenu.addAction(actionPaste_);
		actList.push_back(actionCopy_);
		actList.push_back(actionPaste_);
		if(nestingLevel_ > minNestingLevelForWork){
			actList.push_back(actionRename_);
			actList.push_back(actionCut_);
			actList.push_back(actionDelete_);
		}


		break;
	default:
		break;
	}
	int maxWidth = 0;
	for (int i = 0; i < actList.size(); ++i) {
		DEBUG_INFO_TWO ("item size is ", actList.at(i)->defaultWidget()->sizeHint().width());
		if (actList.at(i)->defaultWidget()->sizeHint().width() > maxWidth) {
			maxWidth = actList.at(i)->defaultWidget()->sizeHint().width();
		}
	}
	for (int i = 0; i < actList.size(); ++i) {
		actList.at(i)->defaultWidget()->setFixedWidth(maxWidth);
	}

	for (int i = 0; i < actList.size(); ++i) {
		contextMenu.addAction (actList.at(i));
	}

	if (Common::getDisplayMode() == DisplayMode::SD) {
		contextMenu.exec(event->globalPos()-QPoint(350,0));
	} else {
		contextMenu.exec(event->globalPos());
	}
}

void CMediaPlayer::networkSharesActHandler()
{
	CSharesListDialog d(0, *shareManager_);
	d.exec();
}

void CMediaPlayer::networkStreamActHandler()
{
	COneInputDialog oneInputDialog;
	oneInputDialog.setLabel(tr("Stream URL"));
	oneInputDialog.setTitle(tr("Input stream URL"));
	oneInputDialog.setText("http://");
	if(oneInputDialog.exec() == QDialog::Accepted){
		QString URL = oneInputDialog.line();
		Media::SourcePtr ptr = Media::createSource(URL);
		QStringList urlSplitted = URL.split("://");
		if(ptr.get() == 0 || (urlSplitted.count() == 2 ? urlSplitted.at(1).isEmpty() : true)){
			CYesNoDialog dialog;
			dialog.setTitle(tr("You are inputed invalid URL"));
			dialog.setText (tr("Retry?"));
			if(dialog.exec() == QDialog::Accepted){
				networkStreamActHandler();
			}
		}
		else
			player_->player()->play(ptr);
	}
}

void CMediaPlayer:: copyActHandler()
{
	if(copier_->state() == 1){
		DEBUG_INFO_TWO("selectedIndex_", model_->fileInfo(selectedIndex_).absoluteFilePath());
		DEBUG_INFO_TWO("destString", destString);
		DEBUG_INFO_TWO("copyString", copyString);
		wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("Unable to copy. <br>Now something is copied") + "</p></font>");
		wrongMessage->show();
		wrongMessageVisible = true;
		return;
	}
	DEBUG_INFO_ONE(model_->fileInfo(selectedIndex_).absoluteFilePath());
	actionPaste_->setEnabled(true);
	actionPaste_->setVisible(true);
	copyIndex_ = selectedIndex_;
	cutflg_ = false;
}

void CMediaPlayer::cutActHandler()
{
	if(copier_->state() == 1){
		DEBUG_INFO_TWO("selectedIndex_", model_->fileInfo(selectedIndex_).absoluteFilePath());
		DEBUG_INFO_TWO("destString", destString);
		DEBUG_INFO_TWO("copyString", copyString);
		wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("Unable to cut. <br>Now something is copied") + "</p></font>");
		wrongMessage->show();
		wrongMessageVisible = true;
		return;
	}
	DEBUG_INFO_ONE(model_->fileInfo(selectedIndex_).absoluteFilePath());
	actionPaste_->setEnabled(true);
	actionPaste_->setVisible(true);
	copyIndex_ = selectedIndex_;
	cutflg_ = true;
}

void CMediaPlayer::deleteActHandler()
{
	DEBUG_INFO();
	if(copyIndex_ == selectedIndex_)
			copyIndex_ = copyIndex_.sibling(-1, -1);
	boost::shared_ptr<QWidget> dw = createDarkenedWidget();
	QString str = model_->fileName(selectedIndex_);
	CYesNoDialog dialog;
	dialog.setTitle(tr("Are you sure you want to delete:"));
	dialog.setText ("<b>"+str+"</b>");
	bool ok = false;
	if(copier_->state() == 1){
		if(copyString.startsWith(model_->fileInfo(selectedIndex_).absoluteFilePath()) ||
				destString.startsWith(model_->fileInfo(selectedIndex_).absoluteFilePath())||
				model_->fileInfo(selectedIndex_).absoluteFilePath().startsWith(copyString) ||
				model_->fileInfo(selectedIndex_).absoluteFilePath().startsWith(destString)){
			wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("Unable to delete. <br>The file/folder is used") + "</p></font>");
			wrongMessage->show();
			wrongMessageVisible = true;
			return;
		}
	}
	if(dialog.exec() == QDialog::Accepted){
		DEBUG_INFO_ONE("QDialog::Accepted");
		ok = true;
	}
	else {
		DEBUG_INFO_ONE("QDialog::Rejected");
	}
	if(ok){
		wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("Deleting ") +"<br>"+str + "<br>" + tr("Please wait...")+ "</p></font>");
		wrongMessage->show();
		wrongMessageVisible = true;
		QTimer::singleShot(1, this, SLOT(del()));
	}
}
void CMediaPlayer::del()
{
	DEBUG_INFO_TWO("model_->remove(selectedIndex_);", model_->fileInfo(selectedIndex_).absoluteFilePath());
	model_->remove(selectedIndex_);
	selectedIndex_ = model_->index(FS::rootPath);
	wrongMessage->hide();
	wrongMessageVisible = false;
}
void CMediaPlayer::readMountOutput()
{
    disconnect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readMountOutput()));
    
	QByteArray mountByteArray;
	mountByteArray = proc->readAllStandardOutput();
	QList<QByteArray> list;
	list = mountByteArray.split('	');
    float copieredSize = list.at(0).toFloat();
    DEBUG_INFO_TWO("\nSize = ", copieredSize);
    struct statvfs st;    
    
    if(model_->fileInfo(selectedIndex_).isDir()){
        statvfs(model_->fileInfo(selectedIndex_).absoluteFilePath().toLocal8Bit().data(),&st); 
    }
    else{
        statvfs(model_->fileInfo(selectedIndex_).absolutePath().toLocal8Bit().data(),&st); 
    }
//    qDebug()<<"Free = "<<((st.f_bsize/1024)*st.f_bavail);
    DEBUG_INFO_TWO("Free! = ", (st.f_bsize/1024)*st.f_bavail);
    DEBUG_INFO_TWO("Available = ", model_->fileInfo(selectedIndex_).absoluteFilePath().toLocal8Bit().data());
    if(copieredSize > ((st.f_bsize/1024)*st.f_bavail)){
        wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("Not enough disk space") + "</p></font>");
        wrongMessage->show();
        wrongMessageVisible = false;
        return;
//        DEBUG_INFO_ONE("         TOO BIG");
	}
	else{
			DEBUG_INFO_ONE("         TOO SMALL");
	}
//    DEBUG_INFO_TWO("     READING", list.at(1));
//	for(int i = 0; i < list.size(); i++){
//		QString str(list.at(i).constData());
//		QStringList lst;
////        if(str.at(0).isDigit())
////		lst = str.split(" ");

////		if(mypath1.startsWith(lst.at(2))){
////			if(mypath2.startsWith(lst.at(2)))
////				ok = true;
////		}
//		qDebug()<<"?? "<<str;//.at(31);
//	}
  QDir currentDir;
    
//    struct statvfs st;
    
//    DEBUG_INFO_TWO("OK", (st.f_bsize/1024)*st.f_bavail);
    
	if(!model_->isDir(selectedIndex_))	{
		currentDir = model_->fileInfo(selectedIndex_).absoluteDir();
	}
	else{
		currentDir.setPath(model_->fileInfo(selectedIndex_).absoluteFilePath() + "/.");
	}
    
  QString name;
	name = model_->fileInfo(copyIndex_).fileName();
	DEBUG_INFO_TWO("selected", model_->fileInfo(selectedIndex_).absoluteFilePath());
	DEBUG_INFO_TWO("copyIndex_", model_->fileInfo(copyIndex_).absoluteFilePath());
	if(!(model_->fileInfo(selectedIndex_).isDir())){
		if(model_->fileInfo(selectedIndex_).absolutePath() == model_->fileInfo(copyIndex_).absolutePath()){
			wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("This file/folder is already here") + "</p></font>");
			wrongMessage->show();
			wrongMessageVisible = true;
			return;
		}
	}
    
    bool ok = false;
    
  for(int i = 0; i < currentDir.entryList().size(); i++){
		if(name.toUpper() == currentDir.entryList().at(i).toUpper()){
			CYesNoDialog dialog;
//			dialog.hideEditLine();
            if(name.size() > 40){
               name = name.mid(0, 40)+"...";
            }
            
			if(model_->fileInfo(copyIndex_).isDir())
				dialog.setTitle (tr("Folder ")+"\n"+name+"\n"+tr("already exists. ")+ tr("Combine?"));
			else
				dialog.setTitle (tr("File ")+"\n"+name+"\n"+tr("already exists. Replaced file will be deleted. ")+ tr("Replace?"));

			if (dialog.exec () == QDialog::Accepted){
				DEBUG_INFO_ONE("        Accepted!");
				ok = true;
			}
			if(ok){
				dialog.close();
				DEBUG_INFO_ONE("        PASTE!");
			}
			else{
				DEBUG_INFO_ONE("        CANCEL!");
				return;
			}
		}
	}

	copyString = model_->fileInfo(copyIndex_).absoluteFilePath();
	if(model_->fileInfo(selectedIndex_).isDir())
		destString = model_->fileInfo(selectedIndex_).absoluteFilePath() +"/"+ model_->fileInfo(copyIndex_).fileName();
	else
		destString = model_->fileInfo(selectedIndex_).absolutePath() +"/"+ model_->fileInfo(copyIndex_).fileName();
	DEBUG_INFO_TWO("DESTINATION = ", destString);
	DEBUG_INFO_TWO("SOURCE = ", copyString);

	if(cutflg_)
	{
		QFileInfo dest = model_->fileInfo(selectedIndex_);
		QFileInfo src = model_->fileInfo(copyIndex_);

		QStringList listDest;
		listDest = dest.absoluteFilePath().split('/');
		QStringList listSrc;
		listSrc = src.absoluteFilePath().split('/');

		if(folderType(selectedIndex_) == LOCAL && folderType(copyIndex_) == LOCAL){
			if(listDest.at(4) == listSrc.at(4)){
				DEBUG_INFO_TWO("ok = ", ok);
				if(ok){
					if(copier_->copy(dest.isDir() ? dest.absoluteFilePath() + "/" : dest.absoluteFilePath(),
									 src.isDir() ? src.absoluteFilePath() + "/" : src.absoluteFilePath()))
					{
						DEBUG_INFO_TWO("copier state", copier_->state());
						copyPanel_->setValue(0);
						copyPanel_->setFileName("");
						copyPanel_->show();
//						QTimer::singleShot(100, this, copyPanel_->show());
					}
				}
				else{
					QString str= "mv \"" + model_->fileInfo(copyIndex_).absoluteFilePath();
					if(model_->isDir(copyIndex_))
						str += "\"";
					else
						str += "\"";
					if(dest.isDir()){
						str += " \"" + dest.absoluteFilePath() + "/";
					}
					else{
						str += " \"" + dest.absolutePath() + "/";
					}
					if(model_->isDir(copyIndex_)){
						str += model_->data(copyIndex_).toString();
					}
					str += "\"";
					DEBUG_INFO_TWO("STR ", str.toUtf8().constData());
					system(str.toUtf8().constData());
                    system("sync");
					cutflg_ = false;
					actionPaste_->setEnabled(false);
					actionPaste_->setVisible(false);
					return;
				}
			}
		}
		if(copier_->copy(dest.isDir() ? dest.absoluteFilePath() + "/" : dest.absoluteFilePath(),
						 src.isDir() ? src.absoluteFilePath() + "/" : src.absoluteFilePath()))
		{
			copyPanel_->setValue(0);
			copyPanel_->setFileName("");
			copyPanel_->show();
//			QTimer::singleShot(100, this, copyPanel_->show());
		}
	}
	else
	{
		QFileInfo dest = model_->fileInfo(selectedIndex_);
//		DEBUG_INFO_TWO("destination = ", model_->fileInfo(selectedIndex_).absoluteFilePath());
		QFileInfo src = model_->fileInfo(copyIndex_);

		if(copier_->copy(	dest.isDir() ? dest.absoluteFilePath() + "/" : dest.absoluteFilePath(),
						 src.isDir() ? src.absoluteFilePath() + "/" : src.absoluteFilePath()))
		{
			copyPanel_->setValue(0);
			copyPanel_->setFileName("");
			copyPanel_->show();
		}
	}
	actionPaste_->setEnabled(false);
	actionPaste_->setVisible(false);
}
void CMediaPlayer::pasteActHandler()
{
  QStringList arguments;
  arguments<<"-sck";
  arguments<<model_->fileInfo(copyIndex_).absoluteFilePath();
  proc->start("du", arguments);
	connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readMountOutput()));
}


static inline QChar getNextChar(const QString &s, int location)
{
	return (location < s.length()) ? s.at(location) : QChar();
}

/*!
 Natural number sort, skips spaces.
 Examples:
 1, 2, 10, 55, 100
 01.jpg, 2.jpg, 10.jpg
 Note on the algorithm:
 Only as many characters as necessary are looked at and at most they all
 are looked at once.
 Slower then QString::compare() (of course)
  */

int naturalCompare(const QString &s1, const QString &s2,  Qt::CaseSensitivity cs)
{
	for (int l1 = 0, l2 = 0; l1 <= s1.count() && l2 <= s2.count(); ++l1, ++l2) {
		// skip spaces, tabs and 0's
		QChar c1 = getNextChar(s1, l1);
		while (c1.isSpace())
			c1 = getNextChar(s1, ++l1);
		QChar c2 = getNextChar(s2, l2);
		while (c2.isSpace())
			c2 = getNextChar(s2, ++l2);
		if (c1.isDigit() && c2.isDigit()) {
			while (c1.digitValue() == 0)
				c1 = getNextChar(s1, ++l1);
			while (c2.digitValue() == 0)
				c2 = getNextChar(s2, ++l2);
			int lookAheadLocation1 = l1;
			int lookAheadLocation2 = l2;
			int currentReturnValue = 0;
			// find the last digit, setting currentReturnValue as we go if it isn't equal
			for (
				 QChar lookAhead1 = c1, lookAhead2 = c2;
				 (lookAheadLocation1 <= s1.length() && lookAheadLocation2 <= s2.length());
				 lookAhead1 = getNextChar(s1, ++lookAheadLocation1),
				 lookAhead2 = getNextChar(s2, ++lookAheadLocation2)
				 ) {
				bool is1ADigit = !lookAhead1.isNull() && lookAhead1.isDigit();
				bool is2ADigit = !lookAhead2.isNull() && lookAhead2.isDigit();
				if (!is1ADigit && !is2ADigit)
					break;
				if (!is1ADigit)
					return -1;
				if (!is2ADigit)
					return 1;
				if (currentReturnValue == 0) {
					if (lookAhead1 < lookAhead2) {
						currentReturnValue = -1;
					} else if (lookAhead1 > lookAhead2) {
						currentReturnValue = 1;
					}
				}
			}
			if (currentReturnValue != 0)
				return currentReturnValue;
		}
		if (cs == Qt::CaseInsensitive) {
			if (!c1.isLower()) c1 = c1.toLower();
			if (!c2.isLower()) c2 = c2.toLower();
		}
		int r = QString::localeAwareCompare(c1, c2);
		if (r < 0)
			return -1;
		if (r > 0)
			return 1;
	}
	// The two strings are the same (02 == 2) so fall back to the normal sort
	DEBUG_INFO_ONE ("");
	return QString::compare(s1, s2, cs);
}

struct ci_less  {
	bool operator() (const QString &s1, const QString &s2)    {
		return naturalCompare(s1, s2, Qt::CaseInsensitive) < 0;
	}
};

void CMediaPlayer::modifyPlayerControls(FileTypes::EKind kind)
{
	switch(kind)	{
	case FileTypes::Video:
        if(!player_->progressBar())	{
			player_->setProgressBar(new Media::CProgressBar(player_));
			player_->progressBar()->hide();
		}
		if(!player_->timeSearchBar())	{
			player_->setTimeSearchBar(new Media::CTimeSearchBar(player_));
			player_->timeSearchBar()->hide();
		}
		if(!player_->trackSelector())	{
			player_->setTrackSelector(new Media::CTrackSelector(player_));
			player_->trackSelector()->hide();
		}
		break;

	case FileTypes::Audio:
		if(!player_->progressBar())	{
			player_->setProgressBar(new Media::CProgressBar(player_));
			player_->progressBar()->hide();
		}
		if(!player_->timeSearchBar())	{
			player_->setTimeSearchBar(new Media::CTimeSearchBar(player_));
			player_->timeSearchBar()->hide();
		}
		player_->setTrackSelector(0);
		break;

	case FileTypes::Image:
		player_->setProgressBar(0);
		player_->setTimeSearchBar(0);
		player_->setTrackSelector(0);
		break;

	default:
		break;
	}
}

void CMediaPlayer::createShareManager()
{
	DEBUG_INFO();
	CShareMounter *mounter = new CShareMounter(new CNativeSysCall());
	mounter->setMountPoint(FS::network);
	shareManager_ = new CShareManager(mounter);
	connect(shareManager_, SIGNAL(hasErrors()), this, SLOT(showShareManagerError()));
	connect(shareManager_, SIGNAL(working()), this, SLOT(shareManagerStarted()));
	connect(shareManager_, SIGNAL(done()), this, SLOT(shareManagerDone()));
	QSettings sett(settingsFilename, QSettings::NativeFormat);
	QVector<QSharedPointer<SShare> > shares = CSharesListDialog::loadSettings(sett);
	QVector<QSharedPointer<SShare> >::ConstIterator it;
	for(it = shares.constBegin(); it != shares.constEnd(); ++it)
		shareManager_->addAutoShare(*it);
	shareManager_->start();
	sharesWatcher_ = new CAutoSharesWatcher(this);
	connect(sharesWatcher_, SIGNAL(appeared(QSharedPointer<SShare>)), shareManager_, SLOT(addAutoShare(QSharedPointer<SShare>)));
	connect(sharesWatcher_, SIGNAL(disappeared(QSharedPointer<SShare>)), shareManager_, SLOT(removeAutoShare(QSharedPointer<SShare>)));
	connect(shareManager_, SIGNAL(mounted(QString)), this, SLOT(directoryMounted(QString)));
}

void CMediaPlayer::shareManagerStarted(){
	mountState = MOUNT_START;
	update();
}

void CMediaPlayer::shareManagerDone(){
	mountState = MOUNT_DONE;
	update();
	showShareManagerError();
}

void CMediaPlayer::showShareManagerError()
{
	QVector<QPair<QSharedPointer<SShare>, QPair<int, QString> > > errors = shareManager_->errors();
	if(errors.count()){

		QString dialogText;
		for(int i = 0; i < errors.count(); i++) {
			if(errors[i].second.first == EACCES){
				CTwoInputDialog twoInputDialog;
				twoInputDialog.setLabel0(tr("Login"));
				twoInputDialog.setLabel1(tr("Password"));
				twoInputDialog.setTitle(tr("Permissions denied"));
				if(twoInputDialog.exec() == QDialog::Accepted){
					QSharedPointer<SShare> share = errors[i].first;
					share->username = twoInputDialog.line0();
					share->password = twoInputDialog.line1();
					shareManager_->tryMountAutoShare(share);
				}
			}
			else
				dialogText = dialogText+tr("Share: ") + errors[i].first->name + tr(" on ") + errors[i].first->ip + "<br>"+tr("Error: ")+errors[i].second.second+"<br>";
		}
		if(!dialogText.isEmpty()){
			CYesDialog dialog;
			dialog.setTitle(tr("Error occurred mounting:"));
			dialog.setText(dialogText);
			dialog.exec();
		}
	}
}

void CMediaPlayer::itemExpanded(const QModelIndex &index)
{
	QString path = model_->fileInfo(index).absoluteFilePath();
	WARNING_INFO_ONE(path);
	shareManager_->tryMountAutoShare(path);
}

void CMediaPlayer::directoryMounted(const QString &path)	{
	WARNING_INFO_ONE(path);
	model_->setRootPath(path);
	model_->setRootPath(FS::rootPath);
}

void CMediaPlayer::destroyShareManager()
{
	DEBUG_INFO();
	shareManager_->umountAll();
	shareManager_->wait();
	delete shareManager_;
}

void CMediaPlayer::playActHandler()
{
	DEBUG_INFO_TWO("play action:",model_->data(selectedIndex_).toString());
	int indexToPlay = 0;
	QStringList filesToPlay;
	FileTypes::EKind kind = FileTypes::Video;
	if(model_->isDir(selectedIndex_))	{
		filesToPlay = FileTypes::createFilesList(FileTypes::Video,model_->fileInfo(selectedIndex_).absoluteFilePath());
		std::sort(filesToPlay.begin(),filesToPlay.end(),ci_less());
	}
	else	{
		if(FileTypes::isAudioFile(model_->fileInfo(selectedIndex_)))
			kind = FileTypes::Audio;
		else if(FileTypes::isImageFile(model_->fileInfo(selectedIndex_)))
			kind = FileTypes::Image;
		else if(! FileTypes::isVideoFile(model_->fileInfo(selectedIndex_)))
			return;
		filesToPlay = FileTypes::createFilesList(kind, model_->fileInfo(selectedIndex_).absolutePath());
		//find file to start
		std::sort(filesToPlay.begin(),filesToPlay.end(),ci_less());
		QString firstFile = model_->fileInfo(selectedIndex_).absoluteFilePath();
		if(filesToPlay.contains(firstFile))	{
			indexToPlay = filesToPlay.indexOf(firstFile);
		}
		else	{
			ERROR_INFO_TWO("filesToPlay does not contain: ",firstFile);
		}
	}
	//	foreach(QString f,filesToPlay)	{
	//		DEBUG_( << f);
	//	}
	if(filesToPlay.isEmpty())
		return;

	QList<Media::SourcePtr> pl;
	foreach(const QString &f, filesToPlay)	{
		pl << Media::createSource(f);
		if(kind != FileTypes::Video)
			pl.back()->setMemorized(false);
	}
	player_->player()->setPlayList(pl);
	Media::CFileSourceInfo *info = dynamic_cast<Media::CFileSourceInfo *>(player_->player()->playList().at(indexToPlay)->info());
	if(info && info->position() > 0 && info->position() != info->duration() )	{
		boost::shared_ptr<QWidget> dw = createDarkenedWidget();
		QTime duration_ = QTime(0,0).addSecs(info->duration()/1000);
		QTime position_ = QTime(0,0).addSecs(info->position()/1000);
		QLatin1String format("hh:mm:ss");
		//		CQuestionDialog dialog;
		//		dialog.setTitle(tr("Play file"));
		//		dialog.setText(tr("Last time you watched %1 of %2. Would you like to continue?").arg(position_.toString(format)).arg(duration_.toString(format)));
		//		if(dialog.exec() && dialog.button() == CQuestionDialog::NoButton)		{
		//			info->setPosition(0);
		//		}
		CYesNoDialog dialog;
		dialog.setTitle (tr("Last time you watched %1 of %2. Would you like to continue?").arg(position_.toString(format)).arg(duration_.toString(format)));
		if(!dialog.exec())	{
			info->setPosition(0);
		}
	}
	if(info && info->position() == info->duration())	{
		info->setPosition(0);
	}
	modifyPlayerControls(kind);
	player_->player()->play(indexToPlay);
}

void CMediaPlayer::playWithSubtitleHandler()
{
	DEBUG_INFO();
	// selectedIndex_ can only be a file because it is unpossible
	// to run actionPlayWithSubtitle_ on directory
	QStringList filesToPlay = FileTypes::createFilesList(FileTypes::Video, model_->fileInfo(selectedIndex_).absolutePath());
	std::sort(filesToPlay.begin(),filesToPlay.end(),ci_less());
	QString firstFile = model_->fileInfo(selectedIndex_).absoluteFilePath();
	int indexToPlay = 0;
	if(filesToPlay.contains(firstFile))	{
		indexToPlay = filesToPlay.indexOf(firstFile);
	}
	else	{
		ERROR_INFO_TWO("filesToPlay does not contain: ",firstFile);
		return;
	}
	//	foreach(QString f,filesToPlay)	{
	//		DEBUG_( << f);
	//	}
	if(filesToPlay.isEmpty())
		return;

	boost::shared_ptr<QWidget> dw = createDarkenedWidget();
	QString fileName = CFileOpenDialog::getFileName(this, tr("Select subtitles file"), FS::rootPath, QStringList() << "*.srt" << "*.sub" << "*.idx" << "*.smi" << "*.ssa" << "*.ass" << "*.s2k");
	if (fileName.isEmpty())
	{
		return;
	}

	QList<Media::SourcePtr> pl;
	foreach(const QString &f, filesToPlay)	{
		pl << Media::createSource(f);
	}
	player_->player()->setPlayList(pl);
	Media::CFileSourceInfo *info = dynamic_cast<Media::CFileSourceInfo *>(player_->player()->playList().at(indexToPlay)->info());
	if(!info)
		return;
	info->setSubtitleFilename(fileName);
	if(info->position() > 0 && info->position() != info->duration() )	{
		boost::shared_ptr<QWidget> dw = createDarkenedWidget();
		QTime duration_ = QTime(0,0).addSecs(info->duration()/1000);
		QTime position_ = QTime(0,0).addSecs(info->position()/1000);
		QLatin1String format("hh:mm:ss");
		//		CQuestionDialog dialog;
		//		dialog.setTitle(tr("Play file"));
		//		dialog.setText(tr("Last time you watched %1 of %2. Would you like to continue?").arg(position_.toString(format)).arg(duration_.toString(format)));
		//		if(dialog.exec() && dialog.button() == CQuestionDialog::NoButton)		{
		//			info->setPosition(0);
		//		}
		CYesNoDialog dialog;
		dialog.setTitle (tr("Last time you watched %1 of %2. Would you like to continue?").arg(position_.toString(format)).arg(duration_.toString(format)));
		if(!dialog.exec()){
			info->setPosition(0);
		}
	}
	if(info->position() == info->duration()){
		info->setPosition(0);
	}
	modifyPlayerControls(FileTypes::Video);
    player_->player()->play(indexToPlay);
	player_->player()->setSubtitleEnabled(true);
}

void CMediaPlayer::renameActHandler()
{
    DEBUG_INFO();
	if(copier_->state() == 1){
		DEBUG_INFO_TWO("selectedIndex_", model_->fileInfo(selectedIndex_).absoluteFilePath());
		DEBUG_INFO_TWO("destString", destString);
		DEBUG_INFO_TWO("copyString", copyString);
		if(copyString.startsWith(model_->fileInfo(selectedIndex_).absoluteFilePath()) ||
				destString.startsWith(model_->fileInfo(selectedIndex_).absoluteFilePath())||
				model_->fileInfo(selectedIndex_).absoluteFilePath().startsWith(copyString) ||
				model_->fileInfo(selectedIndex_).absoluteFilePath().startsWith(destString)){
			wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("Unable to rename. <br>The file/folder is used") + "</p></font>");
			wrongMessage->show();
			wrongMessageVisible = true;
			DEBUG_INFO_ONE("its copying!!");
			return;
		}
	}
	QString fName;
	fName = model_->fileInfo(selectedIndex_).fileName().remove(model_->fileInfo(selectedIndex_).fileName().lastIndexOf(model_->fileInfo(selectedIndex_).suffix())-1, model_->fileInfo(selectedIndex_).suffix().size()+1);

	QLineEdit *edit = new QLineEdit(this);
	if(model_->isDir(selectedIndex_) || model_->fileInfo(selectedIndex_).suffix().isEmpty())
		edit->setText(model_->fileInfo(selectedIndex_).fileName());
	else
		edit->setText(fName);

    CScreenKeyboardDialog::run (this, edit);
    emit keybRename(edit->text());
}
void CMediaPlayer::runRename(QString str)
{
	QString name;
	if(model_->isDir(selectedIndex_) || model_->fileInfo(selectedIndex_).suffix().isEmpty())
        name = str;//inputDialog.getInputtedText();// + model_->fileInfo(selectedIndex_).suffix();
	else
        name = str + "." + model_->fileInfo(selectedIndex_).suffix();
	QString currentName = model_->fileInfo(selectedIndex_).fileName();
	QRegExp validURLRegex("[^\\\\/\?\\*\"\\>\\<\\:\\|]*$");
	if(name.isEmpty() || name.startsWith(".")){
		wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("Name is invalid") + "</p></font>");
		wrongMessage->show();
		wrongMessageVisible = true;
		return;
	}

	if(currentName.toUpper() != name.toUpper()){
		for(int i = 0; i < model_->fileInfo(selectedIndex_).absoluteDir().entryList().size(); i++){
			if(name.toUpper() == model_->fileInfo(selectedIndex_).absoluteDir().entryList().at(i).toUpper()){
				wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("This name is already used") + "</p></font>");
				wrongMessage->show();
				wrongMessageVisible = true;
				return;
			}
		}
	}

	QRegExpValidator validator(validURLRegex, this);
	int index = 0;
	if (validator.validate(name, index) == QValidator::Acceptable) {
		if(selectedIndex_.data().toString() != "local"){
			QFileInfo destCurrent = model_->fileInfo(selectedIndex_);
			QString str = "mv \"" +
					destCurrent.absoluteFilePath() + "\"" + " " + "\"" +
					destCurrent.absolutePath() + "/" +
					name + "\"";
			system(str.toUtf8().constData());
            system("sync");
			QModelIndex parent = model_->index(model_->fileInfo(selectedIndex_).path() + "/" + name);
			tree_->setCurrentIndex(parent);
			if(actionPaste_->isEnabled())
				copyIndex_ = selectedIndex_;
		}
	}
	else {
		wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("Name is invalid") + "</p></font>");
		wrongMessage->show();
		wrongMessageVisible = true;
		return;
	}
}

// add by helius
void CMediaPlayer::addBookmarkActHandler()
{
	DEBUG_INFO();
	//    QString file = model->filePath(selectedIndex);	// get filename
	//    QFileInfo fi (file);
	//    BookMark::addBookMark(selectedItem, fi.baseName());
}

void CMediaPlayer::newDirActHandler()
{
	DEBUG_INFO();
//	boost::shared_ptr<QWidget> dw = createDarkenedWidget();
	QLineEdit *edit = new QLineEdit(this);

    CScreenKeyboardDialog::run (this, edit);
    emit keybNewdir(edit->text());
}

void CMediaPlayer::runNewDir(QString name)
{
    if(name.isEmpty()) return;
	QDir currentDir;
	if(!model_->isDir(selectedIndex_))	{
		currentDir = model_->fileInfo(selectedIndex_).absoluteDir();
	}
	else{
		currentDir.setPath(model_->fileInfo(selectedIndex_).absoluteFilePath() + "/.");
	}

	if(name.isEmpty() || name.startsWith(".")){
		wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("Name is invalid") + "</p></font>");
		wrongMessage->show();
		wrongMessageVisible = true;
		return;
	}
	for(int i = 0; i < currentDir.entryList().size(); i++){
		if(name.toUpper() == currentDir.entryList().at(i).toUpper()){
			wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("This name is already used") + "</p></font>");
			wrongMessage->show();
			wrongMessageVisible = true;
			return;
		}
	}
	QRegExp validURLRegex("[^\\\\/\?\\*\"\\>\\<\\:\\|]*$");
	QRegExpValidator validator(validURLRegex, this);
	int index = 0;
	if (validator.validate(name, index) == QValidator::Acceptable) {
		if(selectedIndex_.data().toString() != "local")	{
			DEBUG_INFO_TWO("ok,", name);
			if(!model_->isDir(selectedIndex_))	{
				DEBUG_INFO_ONE("!dir");
				QModelIndex parent = model_->parent(selectedIndex_);
				if(parent.isValid())	{
					DEBUG_INFO_TWO("parent is valid",model_->fileName(parent));
					parent = model_->mkdir(parent, name);
					DEBUG_INFO_TWO("result = ",model_->fileName(parent));
					tree_->setCurrentIndex(parent);
				}
			}
			else	{
				QModelIndex parent = model_->mkdir(selectedIndex_, name);
				tree_->setCurrentIndex(parent);
			}
		}
	}
	else {
		wrongMessage->setText("<font color=\"white\"><p align='center'>" + tr("Name is invalid") + "</p></font>");
		wrongMessage->show();
		wrongMessageVisible = true;
		return;
	}
}

void CMediaPlayer::playerStatusChanged()
{
    QFileInfo playedFile(player_->player()->trackName());
	if(player_->progressBar())
		player_->progressBar()->setTitle(playedFile.fileName());
	int status = player_->player()->status();
	DEBUG_( << std::hex << status << std::dec );
	if(status & Media::CPlayer::Error)
		CLongOperationManager::instance()->deactivate();
	if((status & Media::CPlayer::PlayingMask) || status == Media::CPlayer::Moving ||
			status == Media::CPlayer::Paused || status == Media::CPlayer::Loading) {
		QSettings set(localSettingsFilename, QSettings::NativeFormat);
		bool ok = false;
		int lastVolume = set.value("mediaplayer/volume").toInt(&ok);
		if(ok)	{
			player_->player()->setVolume(lastVolume);
		}
		playerIsRunning_ = true;
		background_->hide();
		player_->show();
		progressBox->hide();
		player_->setFocus();
		if(status == Media::CPlayer::Moving || status == Media::CPlayer::Loading)
			CLongOperationManager::instance()->activate();
		if((status & Media::CPlayer::PlayingMask) || status == Media::CPlayer::Paused)
			CLongOperationManager::instance()->deactivate();
		return;
	}
	else if(status == Media::CPlayer::AtEnd)	{
		int ind = player_->player()->currentPlayListIndex();
		Media::CFileSourceInfo *info = dynamic_cast<Media::CFileSourceInfo *>(player_->player()->playList().at(ind)->info());
		if(info && info->position() == 0)	{
			player_->player()->play();	// we probably did rewind
			return;
		}
		int sz = player_->player()->playList().size();
		if(ind < (sz-1))	{
			player_->player()->playNext();
			player_->timeSearchBar()->hide();
			return;
		}
	}
	playerIsRunning_ = false;
	background_->show();
	tree_->setFocus();
	player_->hide();
}

int CMediaPlayer::getNestingLevel(QModelIndex ind)
{
	int result=0;
	while(ind.parent().isValid()){
		ind = ind.parent();
		++result;
	}
	//	DEBUG_INFO_TWO("nesting level ==", result);
	return result;
}

void CMediaPlayer::treeClicked(const QModelIndex & ind)
{
	DEBUG_INFO();
	if (!ind.isValid()) {
		DEBUG_ ("invalid index");
		return;
	}

	hideMediaInfo();
	QFileInfo info = model_->fileInfo(ind);
	if(info.isDir())	{
		label[0]->setText(tr("Type: folder"));
		label[0]->show();
		label[1]->hide();
		//        // eugene: hide preview label and stop caching
		//        cacher->stop ();
		//        lbPreview->hide();
	}
	else	{
		QString tmp = info.suffix();
		if(tmp.isEmpty())	{
			label[0]->setText(tr("Type: file"));
			// eugene: hide preview label and stop caching
			//            cacher->stop ();
			//            lbPreview->hide();
		}
		//        else if ((tmp == "JPG") || (tmp == "jpg")) {
		//eugene: run images caching thread here
		//            cacher->startCaching (info.path() + "/");
		//            lbPreview->setPixmap (cacher->getPreview(info.fileName()));
		//            lbPreview->show();
		//        }
		else	{
			label[0]->setText(tr("Type: file *.") + tmp);
			// eugene: hide preview label and stop caching
			//            cacher->stop ();
			//            lbPreview->hide();
		}
		label[0]->show();
		label[1]->show();
		double size = info.size();
		int unit = -1;
		do	{
			++unit;
			if(size > 100)	{
				tmp = "Size: <b>" + QString::number(size,'f',1);
			}
			else if(size > 10)	{
				tmp = "Size: <b>" + QString::number(size,'f',2);
			}
			else if(size > 1)	{
				tmp = "Size: <b>" + QString::number(size,'f',3);
			}
			else	{	//size == 0
				tmp = "Size: <b>0" ;
			}
			size /= 1024;
		} while(size>1);
		switch(unit)	{
		case 0:
			tmp +=" B</b>";
			break;
		case 1:
			tmp +=" KB</b>";
			break;
		case 2:
			tmp +=" MB</b>";
			break;
		case 3:
			tmp +=" GB</b>";
			break;
		case 4:
			tmp +=" TB</b>";
			break;
		default:
			break;
		}
		label[0]->setText(label[0]->text() + "; " + tmp);
		label[0]->show();
	}

	selectedIndex_ = ind;
	nestingLevel_ = getNestingLevel(selectedIndex_);
	updateInfoBar(ind);
}

void CMediaPlayer::treeDoubleClicked(const QModelIndex & ind)
{
	DEBUG_INFO ();
	if (!ind.isValid()) {
		return;
	}
	hideMediaInfo ();
	if(model_->isDir(ind))
	{
		if(tree_->isExpanded(ind))	{
			tree_->collapse(ind);
		}
		else {
			tree_->expand(ind);
		}
		updateInfoBar(ind);
		return;
	}
	int minNestingLevelForWork=0;
	selectedIndex_ = ind;
	ETreeType type = folderType(selectedIndex_);

	switch(type) {
	case LOCAL:
		minNestingLevelForWork = minNestingLevelForLocalWork_;
		break;
	case NETWORK:
		minNestingLevelForWork = minNestingLevelForNetWork_;
		break;
	case PLAYLISTS:
		minNestingLevelForWork = minNestingLevelForPaylistsWork_;
		break;
	case UNKNOWN:
		ERROR_INFO_ONE("folder type is unknown");
		return;
		break;
	}

	if(nestingLevel_ < minNestingLevelForWork)  {
		return ;
	}
	playActHandler();
}

void CMediaPlayer::updateInfoBar(const QModelIndex &ind)
{
	if (!ind.isValid()) {
		return;
	}
	if(model_->filePath(ind).length() <= FS::rootPath.length())
		return;
	if(model_->isDir(ind))	{
		if(tree_->isExpanded(ind))
			infoBar_->setButtonDescription(CInfoBar::B_OK,tr("Close"));
		else
			infoBar_->setButtonDescription(CInfoBar::B_OK,tr("Open"));
	}
	else {
		infoBar_->setButtonDescription(CInfoBar::B_OK,tr("Play"));
	}
}

CMediaPlayer::ETreeType CMediaPlayer::folderType(const QModelIndex & ind)
{
	QString temp = model_->fileInfo(ind).absoluteFilePath();
	DEBUG_INFO_TWO("temp =",temp);

	if(temp.indexOf(FS::network) == 0){
		DEBUG_INFO_ONE("return NETWORK");
		return NETWORK;
	}
	if(temp.indexOf(FS::local) == 0){
		DEBUG_INFO_ONE("return LOCAL");
		return LOCAL;
	}
	if(temp.indexOf(FS::playlists) == 0){
		DEBUG_INFO_ONE("return PLAYLISTS");
		return PLAYLISTS;
	}
	DEBUG_INFO_ONE("return UNKNOWN");
	return UNKNOWN;
}

void CMediaPlayer::volumeChanged(int val)
{
	DEBUG_INFO_ONE(val);
	QSettings set(localSettingsFilename, QSettings::NativeFormat);
	set.setValue("mediaplayer/volume", val);
}

QPixmap CMediaPlayer::appLogo() {
	if (QFile::exists(pluginPath_+"/mediaplayer_big.png")){
		return QPixmap(pluginPath_+"/mediaplayer_big.png");
	}
	else
		return QPixmap(":/mpimg/player.png");
}

QPixmap CMediaPlayer::appLogoSmall() {
	if (QFile::exists(pluginPath_+"/mediaplayer_small.png")){
		return QPixmap(pluginPath_+"/mediaplayer_small.png");
	}
	else
		return QPixmap(":/mpimg/player_small.png");
}

void CMediaPlayer::setFullPath(const QString &path)
{
	pluginPath_ = path;
	start();
}

QString CMediaPlayer::appName()
{
	if(QFile::exists(pluginPath_+"/mediaplayer_desc.txt")){
		QString name_rus;
		QString name_eng;
		QFile desc(pluginPath_+"/mediaplayer_desc.txt");
	    if (!desc.open(QIODevice::ReadOnly)) {
	        DEBUG_INFO_ONE("error open desc");
			return trUtf8("Media Player");
	    }
	    QTextStream in(&desc);
	    QString str;
	    while(!in.atEnd()) {
	        str = in.readLine();
			if(str.startsWith("name_rus")){
				name_rus = str.mid(str.indexOf('=')+1);
			}
			if(str.startsWith("name_eng")){
				name_eng = str.mid(str.indexOf('=')+1);
			}
	    }
	    desc.close();
		if(name_eng.isEmpty() && name_rus.isEmpty())
			return trUtf8("Media Player");
		
		if(Common::getCurrentLanguage() == QLocale::Russian)
			return name_rus;
		else
			return name_eng;
	}
	else
		return trUtf8("Media Player");
}

QString CMediaPlayer::appDescription()
{
	if(QFile::exists(pluginPath_+"/mediaplayer_desc.txt")){
		QString desc_rus;
		QString desc_eng;
		QFile desc(pluginPath_+"/mediaplayer_desc.txt");
	    if (!desc.open(QIODevice::ReadOnly)) {
	        DEBUG_INFO_ONE("error open desc");
			return trUtf8("Playing movies and music, view photos, from external drive\n"
						  "with USB and E-SATA, or from your PC via network");
		}
	    QTextStream in(&desc);
	    QString str;
	    while(!in.atEnd()) {
	        str = in.readLine();
			if(str.startsWith("desc_rus")){
				desc_rus = str.mid(str.indexOf('=')+1);
			}
			if(str.startsWith("desc_eng")){
				desc_eng = str.mid(str.indexOf('=')+1);
			}
	    }
	    desc.close();
		if(desc_eng.isEmpty() && desc_rus.isEmpty())
			return trUtf8("Playing movies and music, view photos, from external drive\n"
						  "with USB and E-SATA, or from your PC via network");
		
		if(Common::getCurrentLanguage() == QLocale::Russian)
			return desc_rus;
		else
			return desc_eng;
	}
	else
		return trUtf8("Playing movies and music, view photos, from external drive\n"
					  "with USB and E-SATA, or from your PC via network");
}

Q_EXPORT_PLUGIN2(mediaplayer, CMediaPlayer)
