#ifndef CMEDIAPLAYER_H
#define CMEDIAPLAYER_H

#ifndef GIT_VERSION
#define GIT_VERSION Unknown;
#endif
#define MAKESTR(x) #x
#define STRINGIFY(x) MAKESTR(x)

#include "../common/interfaces/stb_application_plugin.h"
#include <boost/shared_ptr.hpp>
#include <QItemDelegate>
#include <QSortFilterProxyModel>
#include "cfiletypedetect.h"
#include "dlna.h"

class QModelIndex;
class QTranslator;
class QWidgetAction;


//*****************************************************************************
class SelectedItemDelegate : public QItemDelegate
{
	Q_OBJECT
	public:
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

class QLabel;
class CPushButton;
class QFileSystemModel;
class CIconProvider;
class CTreeView;
class CCopyPanel;
class CInfoBar;
class CSysCommandThread;
class CInfoMessageBackground;
class QProcess;
class QTimer;
class CScreenKeyboard;
namespace Media {
class CCompletePlayer;
}
class CFileCopierClient;
class CLoadProgressBar;
class CAboutContent;
class CShareManager;
class CAutoSharesWatcher;

class CMediaPlayer : public CStbApplication
{
	Q_OBJECT
	Q_INTERFACES(StbInterface)
	QLabel *wrongMessage;
	CAboutContent *aboutContent;

	bool playerIsRunning_;
	bool cutflg_;
	bool cutcancel_;
	bool wrongMessageVisible;
	QTranslator * myTranslator;
	QLabel *background_;
	void start();
	void createBackground();
	QLabel *label1_;
	CPushButton *filterAll_;
	CPushButton *filterImage_;
	CPushButton *filterMusic_;
	CPushButton *filterMovie_;
	void createFilterPanel();
	void deselectAllFilterButtons();
	QFileSystemModel *model_;
	boost::shared_ptr<CIconProvider> iconProvider_;
	void createModel();
	CTreeView *tree_;
	void createTree();
	CFileCopierClient *copier_;
	void createCopier();
	void connectToFileCopier();
	QWidgetAction* actionExit_;
	QWidgetAction* actionCopy_;
	QWidgetAction* actionCut_;
	QWidgetAction* actionDelete_;
	QWidgetAction* actionPaste_;
	QWidgetAction* actionPlay_;
	QWidgetAction* actionPlayWithSubtitle_;
	QWidgetAction* actionRename_;
	QWidgetAction* actionAddBookmark_;
	QWidgetAction* actionNewDir_;
	QWidgetAction* actionStopCopying_;
	QWidgetAction* actionNetworkShares_;
	QWidgetAction* actionNetworkStream_;
	void createActions();
	CCopyPanel *copyPanel_;
	void createCopyPanel();
	enum { COUNT_OF_INFO_LABELS = 15 };
	QLabel* label[COUNT_OF_INFO_LABELS];
	void createMediaInfoLabels();
	CInfoBar *infoBar_;
	CInfoBar *infoBarColored_;
	void createInfoBar();
	Media::CCompletePlayer *player_;
	void createPlayer();
	QModelIndex selectedIndex_;
	QModelIndex lastSelectedIndex_;	
	QModelIndex copyIndex_;
	QModelIndex lastParent_;
	QString destString;
	QString copyString;
	int nestingLevel_;
	int minNestingLevelForLocalWork_;
	int minNestingLevelForNetWork_;
	int minNestingLevelForPaylistsWork_;
	void initIndexes();
	int getNestingLevel(QModelIndex ind);
	enum ETreeType { UNKNOWN = 0, NETWORK, LOCAL, PLAYLISTS};
	ETreeType folderType(const QModelIndex &);
	void createTranslater ();

	CSysCommandThread *mediaInfoThread;
	CInfoMessageBackground *mediaInfoBackground;
	void createMediaInfo (const QString &file);
	void hideMediaInfo();
	void modifyPlayerControls(FileTypes::EKind);

	CLoadProgressBar *progressBox;
	QProcess *proc;
	QTimer *hideTimer;
	QString pluginPath_;
	QString lastInfo_;

	CShareManager *shareManager_;
	void createShareManager();
	void destroyShareManager();
	CAutoSharesWatcher *sharesWatcher_;
    //dlna
    Widget *window;
    
private slots:
	void volumeChanged(int);
	void updateInfoBar(const QModelIndex &);
	void prepareToPlay();

	void updateSelection ();
	void disableFilters();
	void enableImageFilter();
	void enableMovieFilter();
	void enableMusicFilter();

	void networkSharesActHandler();
	void networkStreamActHandler();
	void copyActHandler();
	void cutActHandler();
	void deleteActHandler();
	void pasteActHandler();
	void playActHandler();
	void playWithSubtitleHandler();
	void renameActHandler();
	void addBookmarkActHandler();
	void newDirActHandler();

	void playerStatusChanged();
	void treeClicked(const QModelIndex &);
	void treeDoubleClicked(const QModelIndex &);

	void currentItemChanged(int);
	void showMediaInfo ();
	void rmCopyed();

	void hideCopypanel();

	void progressChanged(int);
	void stopPlayer();
	void checkState();
	void checkIndexes ();
    void readMountOutput();
    void hideControls();
	void del();
	void runRename(QString);
	void runNewDir(QString);
	void showShareManagerError();
	void shareManagerStarted();
	void shareManagerDone();


    void itemExpanded(const QModelIndex &);
    void directoryMounted(const QString &path);

    //dlna
    void ToPlay(QString);


signals:
	void progressStep(int);
    void keybRename(QString);
    void keybNewdir(QString);

protected:
	void keyPressEvent(QKeyEvent *);
	void contextMenuEvent(QContextMenuEvent *);
	bool eventFilter(QObject *obj, QEvent *event);

public:
	enum EMountState { MOUNT_NONE = 0, MOUNT_START, MOUNT_DONE};

	explicit CMediaPlayer(QWidget *parent = 0);
	~CMediaPlayer();

	static const QString settingsFilename;

	QString appName();
	QString appDescription();
	QPixmap appLogo();
	QPixmap appLogoSmall();
	virtual void setFullPath(const QString &path);
	QString relativePath() const { return "mediaplayer/"; }
	QString pluginVersion() const {return STRINGIFY(PLUGIN_VERSION);}
	QString vcsVersion() const {return STRINGIFY(GIT_VERSION);}
	QString compilationTime() const {return __TIME__ " " __DATE__;}
	bool supportsSD() const { return true; }

};




#endif
