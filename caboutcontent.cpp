#include "caboutcontent.h"
#include <QPixmap>
#include <QPainter>
#include <QLinearGradient>
#include <QPoint>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QCoreApplication>
#include <QLabel>
#include <QEvent>
//#include <QDebug>
const QString labelStyleSheet =
"QLabel {"
"border: 3px solid rgb(37, 90, 225);"
"background-color:rgb(0,0,0, 255);"
"color : white;"
"}";

CAboutContent::CAboutContent(QWidget *parent) :
	QWidget(parent)
{
	label_ = new QLabel(this);
	label_->move(20,20);
	label_->setAutoFillBackground(false);
	QFont font("Century Gothic", 32, QFont::Normal);
	font.setItalic(true);
	label_->setFont(font);
//	label->setFont(QFont("MyriadPro", 42, QFont::Normal));
	label_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	label_->setStyleSheet ("QLabel {color:#FFFFFF};");
    label_->setFixedWidth(1020);
	makeBackground(size());
}
void CAboutContent::setText(QString str)
{
    label_->clear();    
	label_->setText(str);
}

void CAboutContent::makeBackground(const QSize &size)
{
	background_ = QPixmap(size);
#ifdef BRCM
	background_.fill(Qt::transparent);
#endif
	QPoint start(0,0);
	QPoint finish(size.width(),size.height());
	QLinearGradient gradient(QPoint(0,size.height()),start);
	gradient.setColorAt(0,		QColor(0,0,0,160));//opacity
	gradient.setColorAt(1,		QColor(0,0,0,160));
	QPainter painter(&background_);
	painter.setRenderHint(QPainter::Antialiasing, true);

	QRect    rect(start,finish);
	QPainterPath mainPath;
	mainPath = painter.clipPath();
	mainPath.addRoundRect(rect, 15);
	painter.fillPath(mainPath, gradient);
}
void CAboutContent::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event)
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.drawPixmap(0,0,background_);
}
void CAboutContent::keyPressEvent (QKeyEvent * e)
{
//	deleteLater();
	hide();
//	clearFocus();
	setFocus();
//	clearFocus();
//	e->ignore();
//	qDebug()<<"                               CAboutContent  keypress!";
//	QCoreApplication::sendEvent(, e);
	QWidget::keyPressEvent(e);
}
void CAboutContent::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event)
//    qDebug()<<"                   resize!!!!!!!!!!!!!" << size();
//	if(event->oldSize() != size())	{
    makeBackground(size());
    label_->setFixedHeight(size().height());
//	}
}
CAboutContent::~CAboutContent()
{
}
//bool CAboutContent::event(QEvent *event)
//{
//	qDebug()<<"       send          1           2       KEYPRESS!";
////    switch (e->type()) {
////    case QEvent::KeyPress:
////        keyPressEvent((QKeyEvent *)event);
////        if (!((QKeyEvent *)event)->isAccepted())
////            return false;
////        break;
////    case QEvent::KeyRelease:
////        keyReleaseEvent((QKeyEvent *)event);
////        if (!((QKeyEvent *)event)->isAccepted())
////            return false;
////        break;
////        ...
////    }
////	hide();
//	return QWidget::event(event);
//}
