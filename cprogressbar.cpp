#include "cprogressbar.h"
#include <QProgressBar>
#include <QLabel>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>

CLoadProgressBar::CLoadProgressBar(QWidget *parent) :
	QDialog(parent, Qt::FramelessWindowHint)
{
	QLabel *piclabel = new QLabel(this);
	piclabel->move(0, 0);
	piclabel->setPixmap(QPixmap(":/mpimg/progress.png"));
	piclabel->setFixedSize(piclabel->pixmap()->size());
	setFixedSize(piclabel->pixmap()->size());
	QLabel *messText = new QLabel(piclabel);
	messText->setStyleSheet(QLatin1String("QLabel { color: #FFFFFF}; "));
	messText->setFont(QFont("MyriadPro", 45, QFont::Normal));
	messText->setText(QString(QLatin1String("<font color='#FFFFFF'>%1</font>")).arg(tr("Loading, please wait.")));
	messText->move(10, 100);
	progress_ = new QProgressBar(this);
	progress_->setFixedSize(piclabel->pixmap()->width() - 30, 50);
	progress_->move(15, 200);
	progress_->setMaximum(100);
	progress_->setFont(QFont("MyriadPro", 45, QFont::Normal));
	progress_->setValue (0);
}

void CLoadProgressBar::progressChanged(int state)
{
	progress_->setValue(state);
	if (state == 100) QTimer::singleShot(1000, this, SLOT(close()));
}

void CLoadProgressBar::keyPressEvent(QKeyEvent *event)
{
	if(event->key() == Qt::Key_Escape)	{
		emit returnPressed();
		close();
	}
}
