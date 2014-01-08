#ifndef CDIALOG_H
#define CDIALOG_H

#include <QDialog>
#include "ui_LoginPasswordDialog.h"
#include "ui_NameDialog.h"
#include "ui_QuestionDialog.h"

class CInfoBar;
class CPushButton;
class QLineEdit;
class QLabel;
class QSignalMapper;

class CDialogBase : public QDialog
{
	Q_OBJECT

	QLabel *title_;

protected:

	void paintEvent(QPaintEvent *);
	virtual void  resizeEvent(QResizeEvent *);
	void keyPressEvent(QKeyEvent *);

public:

	enum Buttons	{
		NoButtons = 0,
		OkButton ,
		CancelButton ,
		ApplyButton ,
		YesButton ,
		NoButton
	};

	enum {HeaderHeight = 80,BorderRadius = 40};

	CDialogBase(QWidget *parent = 0);
	~CDialogBase();
	QString title() const;

public slots:

	void setTitle(const QString &);
};

class CLoginPasswordDialog : public CDialogBase, private Ui::LoginPassword
{
	Q_OBJECT

protected:

	bool eventFilter(QObject *, QEvent *);

public:

	CLoginPasswordDialog(QWidget *parent = 0);
	QString login() const;
	QString password() const;

public slots:

	void setLogin(const QString &str);
	void setPassword(const QString &str);
};

class CEnterNameDialog : public CDialogBase, private Ui::AddingToFavouritesDialog
{
	Q_OBJECT

protected:

	bool eventFilter(QObject *, QEvent *);

public:

	CEnterNameDialog(QWidget *parent = 0);
	QString name() const;

public slots:

	void setName(const QString &str);
};

class CQuestionDialog : public CDialogBase, private Ui::QuestionDialog
{
	Q_OBJECT

	Buttons clickedButton;
	QSignalMapper *mapper;

private slots:

	void buttonClicked(int id);

protected:

	bool eventFilter(QObject *, QEvent *);

public:

	CQuestionDialog(QWidget *parent = 0);
	QString text() const;
	int button();
	CPushButton * getButton(Buttons b);

public slots:

	void setText(const QString &str);
};

//*****************************************************************************
class CLineEdit : public QLineEdit {

	Q_OBJECT

public:
	CLineEdit (QWidget * parent);
protected:
	virtual void keyPressEvent (QKeyEvent * event);
};


//*****************************************************************************
class CBaseDialog : public QDialog {

	Q_OBJECT

private:
	bool result;
public:
	QLabel * title_;
	CInfoBar * infoBar;
	CBaseDialog (QWidget * parent = NULL);
	virtual void setTitle (QString);
protected:
	virtual void resizeEvent (QResizeEvent * e);
};

//*****************************************************************************
class CYesNoDialog : public CBaseDialog {

	Q_OBJECT

	QLabel * text_;
public:
	CYesNoDialog (QWidget * parent = NULL);
//	virtual void setTitle (QString);
	virtual void setText (QString);
protected:
	virtual void keyPressEvent (QKeyEvent * e);
};

class CYesDialog : public CBaseDialog {

	Q_OBJECT

	QLabel * text_;
public:
	CYesDialog (QWidget * parent = NULL);
//	virtual void setTitle (QString);
	virtual void setText (QString);
	QString getText(){return text_->text();}
protected:
	virtual void keyPressEvent (QKeyEvent * e);
};

//*****************************************************************************
class COneInputDialog : public CBaseDialog {
	Q_OBJECT

	CLineEdit * edit;
	QLabel * label;
	QLabel * labelButton;
//	QStringList getInputtedText () {return edit0->text();}
public:
	COneInputDialog (QWidget * parent = NULL);
	void setText (QString line) {edit->setText(line);}
	void setLabel (QString line) {label->setText(line);}
	QString line (void){return edit->text();}
protected:
	virtual void resizeEvent(QResizeEvent *);
	bool eventFilter(QObject *, QEvent *);
};

//*****************************************************************************
class CTwoInputDialog : public CBaseDialog {

	Q_OBJECT

	CLineEdit * edit0;
	CLineEdit * edit1;
	QLabel * label0;
	QLabel * label1;
	QLabel * labelButton;
//	QStringList getInputtedText () {return edit0->text();}
public:
	CTwoInputDialog (QWidget * parent = NULL);
	void setText0 (QString line) {edit0->setText(line);}
	void setText1 (QString line) {edit1->setText(line);}
	void setLabel0 (QString line) {label0->setText(line);}
	void setLabel1 (QString line) {label1->setText(line);}
	QString line0 (void){return edit0->text();}
	QString line1 (void){return edit1->text();}
	static QStringList getText (QString title, QString label0, QString label1, bool * ok);
protected:
	virtual void resizeEvent(QResizeEvent *);
	bool eventFilter(QObject *, QEvent *);

};
#endif // CDIALOG_H
