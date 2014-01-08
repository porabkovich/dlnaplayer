#include "treeview.h"
#include "tools.h"
#include <QKeyEvent>
#include <QTreeView>
#include <QScrollBar>

#define MODULE "mediaplayer"
//#define NDEBUG
#include "debugmacros.h"
#include <QFileSystemModel>


CTreeView::CTreeView(QWidget *parent):QTreeView(parent)/*,
								   selectionFrame (this),
															 animateSelection (&selectionFrame, "geometry"),
															 animateView (verticalScrollBar(), "value")*/{
	// Create a semi-transparent frame that doesn't interact with anything
//
//	animateSelection.setDuration(0);
//	animateSelection.setEasingCurve(QEasingCurve::Linear);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

//	animateView.setEasingCurve (QEasingCurve::Linear);
//	animateView.setDuration(0);
//	connect (&animateView, SIGNAL(finished()), this, SLOT(correctSelection()));
//	connect (&animateSelection, SIGNAL(finished()), this, SLOT(selectionMoved()));
}

//void CTreeView::selectionMoved ()
//{
//}

//void CTreeView::correctSelection ()
//{
//	DEBUG_INFO ();
////	updateSelection(visualRect(selectionModel()->selectedIndexes().front()));
//}


void CTreeView::selectionChanged (const QItemSelection & selected, const QItemSelection & deselected )
{
	DEBUG_INFO ();
	if ((selected.indexes().isEmpty()) || (!selected.indexes().front().isValid())) {
		DEBUG_INFO_ONE ("invalid or empty selection!");
//		setCurrentIndex (static_cast<QFileSystemModel*>(model())->index ("/mnt/stb/local"));
//		QTreeView::selectionChanged (QItemSelection (model()->index(0,0), model()->index(0,0)),deselected);
//		setCurrentIndex (model()->index (0, 0));
//		QTreeView::selectionChanged(selected,deselected);
		emit ivalidSelectionIndex();
		return;
	}
	QTreeView::selectionChanged(selected,deselected);
	emit clicked(selected.indexes().front());
//	updateSelection(visualRect(selectionModel()->selectedIndexes().front()));
}

void CTreeView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())			// play on Enter press
	{
	case Qt::Key_Return:
		emit doubleClicked(currentIndex());
		break;
		case Qt::Key_Back: {        //"Rev"
				QKeyEvent event(QEvent::KeyPress, Qt::Key_PageUp, 0, 0);
				QTreeView::keyPressEvent(&event);
			}
			break;

		case Qt::Key_Forward: {     //"Fwd"
				QKeyEvent event(QEvent::KeyPress, Qt::Key_PageDown, 0, 0);
				QTreeView::keyPressEvent(&event);
			}
			break;
	default:
		QTreeView::keyPressEvent(event);
		break;
	}
}


//void CTreeView::animateScroll(QModelIndex cur, QModelIndex prev)
//{
//	if ((!cur.isValid()) || (!prev.isValid())) {
//		return;
//	}
//
//	if (prev.isValid() /*&& prev.parent() == cur.parent()*/) {
//		int diff = 0;
//
//		QRect rect(visualRect(cur));
//		static QRect oldRect;
//
//		if (animateView.state() == QAbstractAnimation::Running) {
//			// correct animation duration time
//			animateView.stop();
//			diff = qAbs(animateView.endValue().toInt() - verticalScrollBar()->value());
//			DEBUG_INFO_TWO ("we running, calc diff:", diff);
//		}
//		DEBUG_INFO_TWO ("duration now:", animateView.duration());
//
////		if (((cur.row() - prev.row() > 0) && (cur.parent().row() == 0)) || ((cur.parent().row() != 0)) && (cur.parent().row() == prev.row())) {
//			if (visualRect(cur).y() > visualRect(prev).y()) {
//			DEBUG_INFO_ONE ("move down");
//			animateView.setStartValue(verticalScrollBar()->value());
//			animateView.setEndValue(verticalScrollBar()->value() + rect.height() + diff);
//			if ((verticalScrollBar()->value() < verticalScrollBar()->maximum()) && (rect.y() > height()/2.0)) {
//				DEBUG_INFO_ONE ("start scrolling down");
//				animateView.start();
//			} else {
////				rect.moveTopLeft(mapToParent(rect.topLeft()));
////				updateSelection (rect);
//				DEBUG_INFO_ONE ("not move, just correct selection");
//			}
//		} else {
//			DEBUG_INFO_ONE ("move up");
//			animateView.setStartValue(verticalScrollBar()->value());
//			animateView.setEndValue(verticalScrollBar()->value() - rect.height() - diff);
//			if ((verticalScrollBar()->value() > verticalScrollBar()->minimum()) && rect.y() < (height()/2.0)) {
//				animateView.start();
//				DEBUG_INFO_ONE ("start scrolling up");
//			} else {
////				rect.moveTopLeft(mapToParent(rect.topLeft()));
////				updateSelection (rect);
//				DEBUG_INFO_ONE ("not move, just correct selection");
//			}
//		}
//	} else if (cur.isValid()) {
////		QRect rect(visualRect(cur));
////		rect.moveTopLeft(mapToParent(rect.topLeft()));
//	}
//}
//
//void CTreeView::lower()
//{
//	selectionFrame.lower();
//}
