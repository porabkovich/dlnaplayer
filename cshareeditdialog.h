#ifndef CSHAREEDITDIALOG_H
#define CSHAREEDITDIALOG_H

#include "cinfobar.h"
#include "cshareslistdialog.h"

namespace Ui {
class CShareEditDialog;
}

class CShareEditDialog : public QDialog
{
	Q_OBJECT

	friend class ShareEditDialogTest;

	QSharedPointer<SShare> share_;
	const QVector<QSharedPointer<SShare> > &shares_;
	Ui::CShareEditDialog *ui;
	bool shown;
	QLabel *labelButton;

	void saveShare();
	void paintEvent(QPaintEvent *e);
	bool isNameValid(const QSharedPointer<SShare> &p, const QString &s) const;
	bool isIPValid(const QString &s) const;
	bool isFolderNameValid(const QString &s) const;
	
public:
	explicit CShareEditDialog(QWidget *parent, const QVector<QSharedPointer<SShare> > &shares, QString title = "",
							  QSharedPointer<SShare> share = QSharedPointer<SShare>());
	~CShareEditDialog();

	bool eventFilter(QObject *, QEvent *);
	QSharedPointer<SShare> getShare();

private slots:
	void onIndexChanged(int index);
};

class CInfoDialog : public QDialog
{
	Q_OBJECT

	QLabel title_;
	QLabel message_;
#ifndef QT_TEST
	CInfoBar infoBar;
#endif

public:
	CInfoDialog (QWidget * parent = NULL);
	void setTitle(const QString &title);
	void setMessage(const QString &message);
};

#endif // CSHAREEDITDIALOG_H
