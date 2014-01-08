#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>
#include <QFrame>
#include <QPropertyAnimation>
#include <QPainter>

//class QPropertyAnimation;



class CTreeView : public QTreeView
{
	Q_OBJECT

public:
	CTreeView(QWidget *parent);
	void selectionChanged(const QItemSelection &, const QItemSelection &);
	void keyPressEvent(QKeyEvent *event);
signals:
	void ivalidSelectionIndex ();
};

#endif // TREEVIEW_H
