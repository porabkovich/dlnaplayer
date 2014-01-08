#include "cinfomessage.h"
#include "csizemanager.h"
#include <iostream>
#include <iomanip>

#include <QPainter>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QTimer>


#define NDEBUG
#include "debugmacros.h"

extern CSizeManager sizeM;

CInfoMessageBackground::CInfoMessageBackground(QColor c, QWidget *parent):
	QWidget(parent),borderEnabled_(false),color_(c),borderColor_(0xFFFFFFFF),
	borderRadius_(10),borderWidth_(2),borderSize_(10)
{

}

void CInfoMessageBackground::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.fillRect(0,0,width(),height(),color_);
	painter.setRenderHint(QPainter::Antialiasing);

	if(!borderEnabled_)
		return;

	QPen pen;
	pen.setStyle(Qt::SolidLine);
	pen.setColor(borderColor_);
	if (sizeM.getMode() == CSizeManager::SD_MODE)
		pen.setColor (QColor(0x999999));
	pen.setWidth(borderWidth_);
//	pen.setBrush(Qt::white);
	pen.setCapStyle(Qt::SquareCap);
	pen.setJoinStyle(Qt::RoundJoin);

	painter.setPen(pen);

	QPoint start(0,0);
	QPoint finish(width(),height());
	QRect borderRect(start + QPoint(borderSize_,borderSize_),finish - QPoint(borderSize_,borderSize_));

	QPainterPath mainPath;
	mainPath = painter.clipPath();
	mainPath.addRoundRect(borderRect, 0);
	painter.fillPath(mainPath, QColor(0, 0, 0, 255));

//	if(borderRadius_ > 0)	{
//		painter.drawRoundedRect(borderRect,borderRadius_,borderRadius_);
//	}
//	else	{
	painter.drawRect(borderRect);
//	}
}

//CInfoMessage::CInfoMessage(int flags, bool blocking, int ttl, QWidget *parent) :
//	QDialog(parent),blocking_(blocking),background(0),text(0),progress(0),button(0),timer(0)
//{
//	background = new CInfoMessageBackground(Qt::black,this);
//	background ->setBorderEnabled(true);
//	if (sizeM.getMode() == HD_MODE) {
//		background ->setBorderWidth(2);
//		background ->setBorderColor(Qt::gray);
//	} else {
//		background ->setBorderWidth(10);
//		background ->setBorderColor(QColor(0x999999));
//	}
//	background ->setBorderSize(15);
//	background ->setBorderRadius(30);
//	background ->setFixedSize(600,300);
//
//	layout = new QVBoxLayout();
//	layout->setMargin(45);
//
//	text = new QLabel;
////	text->setFixedSize(600,110);
////	text->move(10,10);
//	if (sizeM.getMode() == HD_MODE) {
//		text->setFont(QFont("Century Gothic", 32, QFont::Bold));
//	}	else {
//		text->setFont(QFont("Century Gothic", 36, QFont::Bold, true));
//	}
//	text->setStyleSheet(sizeM.getStyle(_ITEM_LABEL));
//
//	text->setWordWrap(true);
//	text->setAlignment(Qt::AlignCenter);
//	layout->addWidget(text);
//
//	if(flags & PROGRESS)	{
//		progress = new QProgressBar;
//	//	progress->setFixedSize(540,40);
//	//	progress->move(30,130);
//		if (sizeM.getMode() == HD_MODE)
//			progress->setFont(QFont("Century Gothic", 24, QFont::Bold));
//		else
//			progress->setFont(QFont("Century Gothic", 28, QFont::Bold, true));
//		progress->setMaximum(100);
//		progress->setValue(0);
//		layout->addWidget(progress);
//	}
//
//	if(flags & BUTTON)		{
//		button = new CPushButton;
//		button->setFixedSize(300,40);
//	//	button->move(160,200);
//		if (sizeM.getMode() == HD_MODE)
//			button->setFont(QFont("Century Gothic", 32, QFont::Bold));
//		else
//			button->setFont(QFont("Century Gothic", 36, QFont::Bold, true));
//		button->setText("OK");
//		layout->addWidget(button);
//		layout->setAlignment(button,Qt::AlignHCenter);
//	}
//
//	if (flags & WAIT_ANIMATION) {
//		CWaitWidgetRound * wait = new CWaitWidgetRound (this,300);
//		setFixedSize (wait->size());
//		wait->move (150,0);
//		background->hide ();
//		QPalette palette (QColor(0,0,0,0));
//		setPalette (palette);
//	}
//
//	background->setLayout(layout);
//	setWindowFlags(Qt::FramelessWindowHint);
//
//	if(!ttl)
//		return;
//
//	timer = new QTimer(this);
//	timer ->setInterval(ttl);
//	connect(timer,SIGNAL(timeout()),this,SIGNAL(finished()));
//	timer->start();
//
//}
//
//QSize CInfoMessage::sizeHint()
//{
//	return background->size();
//}
//
//void CInfoMessage::setProgress(int p)
//{
//	if(!progress)
//		return;
//	if(p<0)
//		p = 0;
//	if(p>100)
//		p = 100;
//	progress->setValue(p);
//}
//
//void CInfoMessage::keyPressEvent(QKeyEvent *event)
//{
//	DEBUG_INFO_ONE(event->key());
//	switch(event->key())	{
//	case Qt::Key_Escape:
//		if(!blocking_)
//			emit finished();
//		break;
//	default:
//		QDialog::keyPressEvent(event);
//		break;
//	}
//}
//
//void CInfoMessage::setText(const QString &str)
//{
//	text->setText(str);
//}
//
