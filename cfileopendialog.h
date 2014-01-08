#ifndef _CFILEOPENDIALOG_H_
#define _CFILEOPENDIALOG_H_

#include <QDialog>
#include <QTreeView>
#include <QItemDelegate>

class QModelIndex;

class QFileSystemModel;
class CIconProvider;
class CTreeView;
// file open simple dialog
class CFileOpenDialog : public QDialog {

	Q_OBJECT

	QFileSystemModel * model;
	QModelIndex fileIndex;
	CIconProvider *iconprovider;
	CTreeView * tree;
	QString root_;
public:
	CFileOpenDialog (QWidget * parent, const QString &caption, const QString &root, const QStringList &filter);
	~CFileOpenDialog();
	QString getOpenFilePath (void);
	static QString getFileName (QWidget * parent, const QString &caption, const QString &root, const QStringList &filter);
private slots:
	void fileSelected (QModelIndex);
	void modelLoaded (void);
	void treeDoubleClicked(const QModelIndex & ind);
protected:
//	void keyPressEvent(QKeyEvent *);
};

#endif

