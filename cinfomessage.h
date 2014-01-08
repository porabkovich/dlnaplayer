#ifndef CINFOMESSAGE_H
#define CINFOMESSAGE_H

#include <QWidget>

class CInfoMessageBackground : public QWidget
{
	Q_OBJECT

	bool borderEnabled_;
	QColor color_;
	QColor borderColor_;
	int borderRadius_;
	int borderWidth_;
	int borderSize_;

protected:
	void paintEvent(QPaintEvent *);

public:
	explicit CInfoMessageBackground(QColor c,QWidget *parent = 0);
	void setColor(QColor c)	{ color_ = c; update();}
	QColor color() const	{return color_;}
	void setBorderEnabled(bool e)	{borderEnabled_ = e; update();}
	void setBorderColor(QColor c)	{ borderColor_ = c; update();}
	QColor borderColor() const	{return borderColor_;}
	void setBorderRadius(int r)	{ borderRadius_ = r; update();}
	int borderRadius() const	{return borderRadius_;}
	void setBorderWidth(int w)	{ borderWidth_ = w; update();}
	int borderWidth() const	{return borderWidth_;}
	void setBorderSize(int w)	{ borderSize_ = w; update();}
	int borderSize() const	{return borderSize_;}
};

//
//class QLabel;
//class QProgressBar;
//class CPushButton;
//class QTimer;
//class QVBoxLayout;
//
//class CInfoMessage : public QDialog
//{
//    Q_OBJECT
//
//	bool blocking_;
//	CInfoMessageBackground *background;
//	QLabel *text;
//	QProgressBar *progress;
//	CPushButton *button;
//	QTimer *timer;
//	QVBoxLayout *layout;
//
//protected:
//	void keyPressEvent(QKeyEvent *);
//
//public:
//	enum {PROGRESS = 0x1, BUTTON = 0x2, WAIT_ANIMATION = 0x4};
//
//	explicit CInfoMessage(int flags,bool blocking,int ttl=0,QWidget *parent = 0);
//	QSize sizeHint();
//	void setProgress(int);
//	void setText(const QString &str);
//
//signals:
//	void finished();
//
//public slots:
////	void onTimer();
//
//};

#endif // CINFOMESSAGE_H
