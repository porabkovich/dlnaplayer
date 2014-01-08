#include "cfileopendialog.h"
#include "csizemanager.h"
#include "ciconprovider.h"
#include "mediaplayer.h"
#include "treeview.h"

#include <iostream>
#include <QBrush>
#include <QFileSystemModel>
#include <QLabel>
#include <QScrollBar>
#include <QTimer>
#include <QTextCodec>
#include <QPainter>
#include <QStyledItemDelegate>

#define NDEBUG
#include "debugmacros.h"

void CFileOpenDialog::treeDoubleClicked(const QModelIndex & ind)
{
//	DEBUG_INFO ();
	if (!ind.isValid()) {
		return;
	}
	if(model->isDir(ind))
	{
		if(tree->isExpanded(ind))	{
			tree->collapse(ind);
		}
		else {
			tree->expand(ind);
		}
		return;
	}
	fileSelected(ind);
}

//////////////////////////////////////////////////////////////////////////////////////

CFileOpenDialog::CFileOpenDialog (QWidget * parent, const QString &caption, const QString &root, const QStringList &filter) : root_ (root)
{
	Q_UNUSED(parent)
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
	QFont font = sizeM.getFont(CSizeManager::_FONT_BIG);
	setStyleSheet (sizeM.getStyle(CSizeManager::_ITEM_DIALOG));
	setWindowFlags(Qt::SplashScreen|Qt::FramelessWindowHint);
	setFixedSize (910,600);

	model = new QFileSystemModel (this);

	iconprovider = new CIconProvider ();
	model->setIconProvider(reinterpret_cast<QFileIconProvider*>(iconprovider));

	model->setNameFilters(filter);
	model->setNameFilterDisables(false);

	QLabel * lb = new QLabel (this);
	lb->setFont (font);
	lb->move (0,20);
	lb->setAlignment (Qt::AlignHCenter);
	lb->setStyleSheet (sizeM.getStyle(CSizeManager::_ITEM_LABEL));
	lb->setText (caption);

	tree = new CTreeView (this);
	connect(tree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(treeDoubleClicked(QModelIndex)));

	tree->move (20,120);
	tree->setFont (font);
	tree->setSortingEnabled(false);
	tree->setModel(model);
	tree->setRootIndex (model->setRootPath(root));

	tree->setStyleSheet(sizeM.getStyle (CSizeManager::_ITEM_TREEVIEW)
						.arg(":/mpimg/branch-closed.png")
						.arg(":/mpimg/branch-open.png"));

	tree->setItemDelegate (new SelectedItemDelegate);
	tree->setIconSize(QSize(64,64));
	tree->resize(870, 440);

	lb->setFixedWidth (tree->width());

	tree->setAnimated(false);
	tree->setIndentation(50);
	tree->setColumnHidden (1, true);
	tree->setColumnHidden (2, true);
	tree->setColumnHidden (3, true);
	tree->setHeaderHidden(true);
	tree->setEditTriggers(0);     //do not change item name

	tree->verticalScrollBar()->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_VERT_SCROLL));
	tree->horizontalScrollBar()->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_HORIZ_SCROLL));
//	tree->setIconSize(QSize(32,32));

//	connect (tree, SIGNAL (activated (QModelIndex)), this, SLOT (fileSelected (QModelIndex)));

	tree->show();
	tree->setFocus ();
	modelLoaded ();


//	treeView->verticalScrollBar()->setStyleSheet(getStyle(_ITEM_VERT_SCROLL));
//	treeView->horizontalScrollBar()->setStyleSheet(getStyle(_ITEM_HORIZ_SCROLL));

	tree->setSelectionMode(QAbstractItemView::SingleSelection);
	tree->setDragEnabled(false);
	tree->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	tree->setFrameShape(QFrame::NoFrame );
	tree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void CFileOpenDialog::modelLoaded (void)
{
	tree->setCurrentIndex(model->index("/mnt/stb/local"));
}

CFileOpenDialog::~CFileOpenDialog()
{
	delete iconprovider;
}

//*****************************************************************************
//
QString CFileOpenDialog::getOpenFilePath (void)
{
	if (exec () == QDialog::Accepted) {
		return model->filePath (fileIndex);
	}
	return "";
}

//*****************************************************************************
// check that it is file
void CFileOpenDialog::fileSelected (QModelIndex ind)
{
	if (!model->isDir(ind)) {
		fileIndex = ind;
		done (QDialog::Accepted);
	}
}

//*****************************************************************************
// static for get file name at one call
QString CFileOpenDialog::getFileName (QWidget * parent, const QString &caption, const QString &root, const QStringList &filter)
{
	CFileOpenDialog dialog (parent, caption, root, filter);
	return dialog.getOpenFilePath ();
}

