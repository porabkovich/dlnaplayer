#ifndef COPYPANEL_H
#define COPYPANEL_H

#include <QWidget>

class QGroupBox;
class QProgressBar;
class QLabel;

class CCopyPanel  :public QWidget
{
	Q_OBJECT

	QGroupBox *box;
	QProgressBar *pBar;
	QLabel *label;
public:

	CCopyPanel(QWidget *parent);
	void setTitleText(const QString &);
public slots:
	void setFileName(const QString &);
	void setMaximum(int);
	void setMinimum(int);
	void setValue(int);

};

#endif // COPYPANEL_H
