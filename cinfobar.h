#ifndef _CINFOBAR_H_
#define _CINFOBAR_H_

#include <QWidget>
#include <map>

class QLabel;

class CInfoBarButton : public QWidget
{
	QLabel *img;
	QLabel *desc;
	bool descVisible_;
public:
	CInfoBarButton(QWidget *parent=0);
	CInfoBarButton(const QPixmap &, const QString &, QWidget *parent=0);
	void setImg(const QPixmap & );
	void setText(const QString & );
	void setDescVisible(bool v = true);
	QString text() const;
	void updGeometry();
	//QSize size() const;
	void setFont (QFont font); 
};

class CInfoBar : public QWidget {
	Q_OBJECT
public:
	enum EButton	{
        B_UP		= 1 << 0,
        B_DOWN		= 1 << 1,
        B_LEFT		= 1 << 2,
        B_RIGHT		= 1 << 3,
        B_OK		= 1 << 4,
        B_RETURN	= 1 << 5,
        B_RED		= 1 << 6,
        B_GREEN		= 1 << 7,
        B_YELLOW	= 1 << 8,
		B_BLUE		= 1 << 9,
		B_MENU		= 1 << 10
		// to be continued
	};

private:
	std::map<EButton,CInfoBarButton *> cont;	// description of buttons
	unsigned long mask;
	int gap;
	int getVertPos(int h, const QSize &size);
//	QString makeImageFilename(const QString &str);
	void updGeometry();

public:
	CInfoBar (QWidget * parent);
	void setFont (QFont);
	void setGeometry(int x,int y,int w,int h);
	void setFixedSize(int w,int h);
	void setGap(int g)	{gap=g;}
//	void show();
	void setupView ();
	void setChecked(EButton);


public slots:
	void setButtonDescription(EButton but,const QString &str);
	void setButtonsMask(unsigned long mask);

//	setInfoMask (int mask);
};

#endif
