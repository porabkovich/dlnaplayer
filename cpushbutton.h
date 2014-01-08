#ifndef CPUSHBUTTON_H
#define CPUSHBUTTON_H
#include <QPushButton>
#include <QKeyEvent>


class CPushButton : public QPushButton{

protected:
	virtual void	keyPressEvent ( QKeyEvent * event ){
		switch (event->key())			// play on Enter press
		{
		case	Qt::Key_Return:
			animateClick(100);
			break;
		default:
			QPushButton::keyPressEvent(event);
			break;
		}
	}
public:
	CPushButton( QWidget * parent = 0 ):
			QPushButton (parent){}
	CPushButton(const QString & text,QWidget * parent = 0 ):
			QPushButton (text,parent){}
	CPushButton(const QIcon & icon, const QString & text, QWidget * parent = 0 ):
			QPushButton (icon,text,parent){}
};

#endif // CPUSHBUTTON_H
