#ifndef CABOUTCONTENT_H
#define CABOUTCONTENT_H
#include <QLabel>
class QPixmap;
class QLabel;
class CAboutContent: public QWidget
{
	void makeBackground(const QSize &size);
	QPixmap background_;
	QLabel *label_;
public:
	CAboutContent(QWidget *parent = 0);
	void setText(QString str);
	~CAboutContent();

protected:
	void keyPressEvent(QKeyEvent *);
	void paintEvent(QPaintEvent *);
	void resizeEvent(QResizeEvent *);
//	bool event(QEvent *event);
};

#endif // CABOUTCONTENT_H
