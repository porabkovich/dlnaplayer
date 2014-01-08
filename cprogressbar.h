#ifndef CLoadProgressBar_H
#define CLoadProgressBar_H

#include <QDialog>

class QProgressBar;

class CLoadProgressBar : public QDialog
{
	Q_OBJECT
	QProgressBar *progress_;
public:
	CLoadProgressBar(QWidget *parent = 0);
protected:
	void keyPressEvent(QKeyEvent *event);
signals:
	void returnPressed(void);
public slots:
	void progressChanged(int);
};

#endif // CLoadProgressBar_H
