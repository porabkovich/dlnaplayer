#include "dialogs.h"

#include <iostream>
#include <iomanip>

#include <QPainter>
#include <QTimerEvent>
#include <QLabel>
#include <QLineEdit>
#include <QSignalMapper>
#include <QTextDocument>

#include "cpushbutton.h"
#include "../ext/inc/rclayout.h"
#include "csizemanager.h"
#include "../common/classes/cscreenkeyboard.h"
#include "cinfobar.h"

#ifndef GLOBAL_DEBUG_ENABLE
//#define NDEBUG
#endif
#include "debugmacros.h"

extern CSizeManager sizeM;

CDialogBase::CDialogBase(QWidget *parent)
	: QDialog(parent)
{
	setFont(QFont("Century Gothic", 28, QFont::Normal));

	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	title_ = new QLabel(this);
	title_->move(BorderRadius,0);
	title_->setFixedHeight(HeaderHeight);
	title_->setFixedWidth(width() - 2 * BorderRadius);
	title_->setAlignment(Qt::AlignCenter);
	title_->setStyleSheet("QLabel {color:#FFFFFF;}");
	title_->show();
}

QString CDialogBase::title() const
{
	return title_->text();
}

void CDialogBase::setTitle(const QString &str)
{
	title_->setText(str);
}

void CDialogBase::resizeEvent(QResizeEvent *)
{
//	std::cout << Q_FUNC_INFO << std::endl;
	title_->setFixedWidth(width() - 2 * BorderRadius);
	update();
}

void CDialogBase::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QLinearGradient myGradient(0,0,0,HeaderHeight);
	myGradient.setColorAt(0,	QColor(0x79,0xB4,0xD4,0xFF));
	myGradient.setColorAt(1,	QColor(0x25,0x8E,0xC7,0xFF));

	QLinearGradient myGradient1(0,HeaderHeight,0,height());
	myGradient1.setColorAt(0,	QColor(0xFA,0xFE,0xFF,0xFF));
	myGradient1.setColorAt(1,	QColor(0xC6,0xE9,0xFC,0xFF));

	QPainterPath myPath;
	myPath.moveTo(0,HeaderHeight);
	myPath.lineTo(0,BorderRadius);
	myPath.cubicTo(0,0,0,0,BorderRadius,0);
	myPath.lineTo(width() - BorderRadius,0);
	myPath.cubicTo(width(),0,width(),0,width(),BorderRadius);
	myPath.lineTo(width(),HeaderHeight);

	QPainterPath myPath1;
	myPath1.moveTo(0,HeaderHeight);
	myPath1.lineTo(0,height() - BorderRadius);
	myPath1.cubicTo(0,height(),0,height(),BorderRadius,height());
	myPath1.lineTo(width() - BorderRadius,height());
	myPath1.cubicTo(width(),height(),width(),height(),width(),height() - BorderRadius);
	myPath1.lineTo(width(),HeaderHeight);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillPath(myPath,myGradient);
	painter.fillPath(myPath1,myGradient1);
}

void CDialogBase::keyPressEvent(QKeyEvent *ke)
{
	switch(ke->key())	{
	case Qt::Key_F1:
		RClayout::switchNext();
		break;

	default:
		QDialog::keyPressEvent(ke);
	}
}

CDialogBase::~CDialogBase()
{

}


QLatin1String lss("QLabel {color:#00406C;}");

QLatin1String bss("QPushButton:focus {"
				  "background-color: #2787B7;"
				  "border: none;"
				  "border-radius: 20px;}"
				  "QPushButton {"
				  "background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
				  "stop:0 #ACDDFE, stop:1 #70BBFC);"
				  "border: none;"
				  "border-radius: 20px;}");

QLatin1String less("QLineEdit:focus {"
				   "border: 2px solid #366C90;"
				   "padding: 0 4px;"
				   "background: #FFFFFF;}"
				   "QLineEdit {"
				   "border: 1px solid #366C90;"
				   "padding: 0 4px;"
				   "background: #F3FBFE;}");

CLoginPasswordDialog::CLoginPasswordDialog(QWidget *parent)
	:CDialogBase(parent)
{
	setupUi(this);

	setTitle(trUtf8("Enter login and password"));

	label->setStyleSheet(lss);
	label_2->setStyleSheet(lss);

	ok_->setStyleSheet(bss);
	cancel_->setStyleSheet(bss);

	login_->setStyleSheet(less);
	password_->setStyleSheet(less);

	login_->setFocus();

	password_->setEchoMode(QLineEdit::PasswordEchoOnEdit);

	login_->installEventFilter(this);
	password_->installEventFilter(this);
	ok_->installEventFilter(this);
	cancel_->installEventFilter(this);
}

bool CLoginPasswordDialog::eventFilter(QObject *target, QEvent *event)
{
	if(event->type() != QEvent::KeyPress/*KeyRelease*/)		{
		return false;
	}
	QKeyEvent *ke = static_cast<QKeyEvent *>(event);
	switch(ke->key())	{
	case Qt::Key_Up:
		if(ok_->hasFocus() || cancel_->hasFocus())	{
			password_->setFocus();
			return true;
		}
		else if(password_->hasFocus())	{
			login_->setFocus();
			return true;
		}
		return QDialog::eventFilter(target,event);

	case Qt::Key_Down:
		if(login_->hasFocus())	{
			password_->setFocus();
			return true;
		}
		else if(password_->hasFocus())	{
			ok_->setFocus();
			return true;
		}
		else	if(ok_->hasFocus() || cancel_->hasFocus()){
			return true;
		}
		return QDialog::eventFilter(target,event);

	case Qt::Key_Right:
		if(ok_->hasFocus())	{
			cancel_->setFocus();
			return true;
		}
		else if(cancel_->hasFocus())	{
			return true;
		}
		return QDialog::eventFilter(target,event);

	case Qt::Key_Left:
		if(cancel_->hasFocus())	{
			ok_->setFocus();
			return true;
		}
		else if(ok_->hasFocus())	{
			return true;
		}

		return QDialog::eventFilter(target,event);
	default:
		break;
	}

	return QDialog::eventFilter(target,event);
}

QString CLoginPasswordDialog::login() const
{
	return login_->text();
}

QString CLoginPasswordDialog::password() const
{
	return password_->text();
}

void CLoginPasswordDialog::setLogin(const QString &str)
{
	login_->setText(str);
}

void CLoginPasswordDialog::setPassword(const QString &str)
{
	password_->setText(str);
}



CEnterNameDialog::CEnterNameDialog(QWidget *parent)
	:CDialogBase(parent)
{
	setupUi(this);

	label->setStyleSheet(lss);

	ok_->setStyleSheet(bss);
	cancel_->setStyleSheet(bss);

	name_->setStyleSheet(less);

	name_->setFocus();

	name_->installEventFilter(this);
	ok_->installEventFilter(this);
	cancel_->installEventFilter(this);
}

bool CEnterNameDialog::eventFilter(QObject *target, QEvent *event)
{
	if(event->type() != QEvent::KeyPress/*KeyRelease*/)		{
		return false;
	}
	QKeyEvent *ke = static_cast<QKeyEvent *>(event);
	switch(ke->key())	{
	case Qt::Key_Up:
		if(ok_->hasFocus() || cancel_->hasFocus())	{
			name_->setFocus();
			return true;
		}
		return QDialog::eventFilter(target,event);

	case Qt::Key_Down:
		if(name_->hasFocus())	{
			ok_->setFocus();
			return true;
		}
		else	if(ok_->hasFocus() || cancel_->hasFocus()){
			return true;
		}
		return QDialog::eventFilter(target,event);

	case Qt::Key_Right:
		if(ok_->hasFocus())	{
			cancel_->setFocus();
			return true;
		}
		else if(cancel_->hasFocus())	{
			return true;
		}
		return QDialog::eventFilter(target,event);

	case Qt::Key_Left:
		if(cancel_->hasFocus())	{
			ok_->setFocus();
			return true;
		}
		else if(ok_->hasFocus())	{
			return true;
		}

		return QDialog::eventFilter(target,event);
	default:
		break;
	}
	return QDialog::eventFilter(target,event);
}

QString CEnterNameDialog::name() const
{
	return name_->text();
}

void CEnterNameDialog::setName(const QString &str)
{
	name_->setText(str);
}

CQuestionDialog::CQuestionDialog(QWidget *parent)
	:CDialogBase(parent),clickedButton(NoButtons)
{
	setupUi(this);

	caption_->setStyleSheet(lss);

	yes_->setStyleSheet(bss);
	no_->setStyleSheet(bss);

	mapper = new QSignalMapper(this);
	mapper->setMapping(yes_,YesButton);
	mapper->setMapping(no_,NoButton);

	connect(yes_,SIGNAL(clicked()),mapper,SLOT(map()));
	connect(no_,SIGNAL(clicked()),mapper,SLOT(map()));

	connect(mapper,SIGNAL(mapped(int)),this,SLOT(buttonClicked(int)));

	no_->setFocus();

	yes_->installEventFilter(this);
	no_->installEventFilter(this);
}

void CQuestionDialog::buttonClicked(int id)
{
	DEBUG_INFO_ONE(id);
	clickedButton = Buttons(id);
	accept();
}

QString CQuestionDialog::text() const
{
	return caption_->text();
}

void CQuestionDialog::setText(const QString &str)
{
	caption_->setText(str);
}

int CQuestionDialog::button()
{
	return clickedButton;
}

bool CQuestionDialog::eventFilter(QObject *target, QEvent *event)
{
	if(event->type() != QEvent::KeyPress/*KeyRelease*/)		{
		return false;
	}
	QKeyEvent *ke = static_cast<QKeyEvent *>(event);
	switch(ke->key())	{
	case Qt::Key_Up:
		return true;

	case Qt::Key_Down:
		return true;

	case Qt::Key_Right:
		if(yes_->hasFocus())	{
			no_->setFocus();
			return true;
		}
		else if(no_->hasFocus())	{
			return true;
		}

	case Qt::Key_Left:
		if(no_->hasFocus())	{
			yes_->setFocus();
			return true;
		}
		else if(yes_->hasFocus())	{
			return true;
		}
	default:
		break;
	}
	return QDialog::eventFilter(target,event);
}

CPushButton * CQuestionDialog::getButton(Buttons b)
{
	if(b == YesButton)
		return yes_;
	else if(b == NoButton)
		return no_;
	return 0;
}

//*****************************************************************************
// class for edit with virtual keyboard and good looking
CLineEdit::CLineEdit (QWidget * parent) : QLineEdit (parent)
{
	home (false);
	setFont (sizeM.getFont (CSizeManager::_FONT_MED));
	setStyleSheet (sizeM.getStyle(CSizeManager::_ITEM_EDIT));
	setFrame (true);
}

//*****************************************************************************
void CLineEdit::keyPressEvent (QKeyEvent * event)
{
	switch (event->key()) {
	DEBUG_INFO_ONE ("key pressed");
		case Qt::Key_Dead_Diaeresis:
		DEBUG_INFO_ONE ("keyboard run");
				CScreenKeyboardDialog::run (parentWidget(), this);
			break;

		default:
			DEBUG_INFO_ONE ("Default key pressed");
			QLineEdit::keyPressEvent (event);
			break;
	}
}


//*****************************************************************************
CBaseDialog::CBaseDialog (QWidget * parent) : QDialog (parent)
{
	setWindowFlags(Qt::FramelessWindowHint);
//	setStyleSheet (sizeM.getStyle(CSizeManager::_ITEM_DIALOG));

	QFont font = sizeM.getFont (CSizeManager::_FONT_BIG);

	title_ = new QLabel (this);
	setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_DIALOG) + sizeM.getStyle (CSizeManager::_ITEM_LABEL));
	title_->setFont(font);
	title_->move (20,20);
	title_->setFixedWidth (width()-40);
	title_->setAlignment (Qt::AlignCenter);
	title_->setWordWrap (true);

	infoBar = new CInfoBar (this);
	infoBar->setFont (font);
	infoBar->move (20, height () - 70);
	infoBar->setButtonsMask (
							CInfoBar::B_RETURN |
							CInfoBar::B_OK);
	infoBar->setButtonDescription (CInfoBar::B_OK, tr("Accept"));
	infoBar->setButtonDescription (CInfoBar::B_RETURN, tr("Cancel"));
}

//*****************************************************************************
void CBaseDialog::resizeEvent (QResizeEvent*)
{
	DEBUG_ ();
	title_->setFixedWidth (width()-40);
	infoBar->move (((width()-infoBar->width())/2)+20, height()-70);
}

void CBaseDialog::setTitle (QString text)
{
	title_->setText (text);
	QFontMetrics fm (title_->font());
	int len = fm.width (text);
	if (width()-40 < len) {
		if (len < 1200) {
			resize (len+40, height());
			title_->setAlignment (Qt::AlignCenter);
			title_->setFixedWidth (width()-40);
		} else {
			resize (1200, height());
//			title_->setAlignment (Qt::AlignLeft);
			title_->setFixedWidth (width()-40);
		}
	}
}

//*****************************************************************************
CYesNoDialog::CYesNoDialog (QWidget * parent) : CBaseDialog (parent) {
	infoBar->setButtonDescription (CInfoBar::B_OK, tr("Yes"));
	infoBar->setButtonDescription (CInfoBar::B_RETURN, tr("No"));
	text_ = new QLabel (this);
	text_->setFont (sizeM.getFont (CSizeManager::_FONT_BIG));
	text_->setAlignment (Qt::AlignCenter);
	resize (900, 350);
	text_->setFixedWidth (width()-20);
	text_-> move (10,100);
}

//*****************************************************************************
void CYesNoDialog::keyPressEvent (QKeyEvent * e)
{
	if (e->key() == Qt::Key_Return) {
		accept ();
	} else {
		CBaseDialog::keyPressEvent (e);
	}
}

void CYesNoDialog::setText (QString text)
{
	text_->setText (text);
	QFontMetrics fm (text_->font());
	int len = fm.width (text_->text());
	if (width()-40 < len) {
		if (len < 1200) {
			DEBUG_ ("increase width");
			resize (len+40, height());
			text_->setAlignment (Qt::AlignCenter);
			text_->setFixedWidth (width()-40);
		} else {
			DEBUG_ ("width limit");
			resize (1200, height());
			text_->setAlignment (Qt::AlignLeft);
			text_->setFixedWidth (width()-40);
		}
	}
}


CYesDialog::CYesDialog (QWidget * parent) : CBaseDialog (parent) {
	infoBar->setButtonsMask (CInfoBar::B_OK);
//	infoBar->setButtonDescription (CInfoBar::B_OK, tr("Yes"));
	text_ = new QLabel (this);
	text_->setFont (sizeM.getFont (CSizeManager::_FONT_BIG));
	text_->setAlignment (Qt::AlignLeft);
	text_-> move (30,100);
	text_->setWordWrap(true);
}
void CYesDialog::keyPressEvent (QKeyEvent * e)
{
	if (e->key() == Qt::Key_Return) {
		accept ();
	} else {
		CBaseDialog::keyPressEvent (e);
	}
}

void CYesDialog::setText (QString text)
{
	text_->setFixedWidth(1200);
	text_->setText (text);
	QTextDocument *doc=new QTextDocument();
	doc->setDefaultFont(text_->font());
	doc->setPlainText(text);
	doc->setPageSize(QSizeF(1200,-1));
	text_->setAlignment (Qt::AlignLeft);
	setFixedSize(1300, doc->size().height()+250);
}

//*****************************************************************************
COneInputDialog::COneInputDialog (QWidget * parent) : CBaseDialog (parent)
{
	label = new QLabel (this);
	label->setFont(sizeM.getFont (CSizeManager::_FONT_BIG));
	setFixedSize(1200, 350);

	edit  = new CLineEdit (this);
	edit->installEventFilter (this);
	infoBar->setButtonsMask (0);

	labelButton = new QLabel(this);
	labelButton->setObjectName("labelButton");
	labelButton->setText(tr("OK"));
	labelButton->setStyleSheet("QLabel {border-color: grey; border-style: solid; border-width: 3px;}");
	labelButton->setStyleSheet(labelButton->styleSheet() + "QLabel:focus{background-color: #004d79;}");
	labelButton->setAlignment(Qt::AlignCenter);
	labelButton->setFixedSize(300, 80);
	labelButton->move(this->width()/2 - labelButton->width()/2, this->height()-100);
	labelButton->setFont(sizeM.getFont (CSizeManager::_FONT_BIG));
	labelButton->installEventFilter(this);
	labelButton->setFocusPolicy(Qt::TabFocus);
}

void COneInputDialog::resizeEvent (QResizeEvent * e)
{
	label->move(30,160);
	edit-> move(320,160);
	edit->setFixedWidth(this->width()-350);
	CBaseDialog::resizeEvent (e);
}

bool COneInputDialog::eventFilter(QObject *target, QEvent *event)
{
	if(event->type() != QEvent::KeyPress/*KeyRelease*/)		{
		return false;
	}
	QKeyEvent *ke = static_cast<QKeyEvent *>(event);
	switch(ke->key())	{
	case Qt::Key_Up:
		focusNextPrevChild(false);
		return true;
	case Qt::Key_Down:
		focusNextPrevChild(true);
		return true;

	case Qt::Key_Return:
		if(target == edit){
			CScreenKeyboardDialog::run(this, static_cast<QLineEdit *>(this->focusWidget()));
		}
		else{
			accept();
		}
	break;

	case Qt::Key_Escape:
		reject ();
	break;

	default:
		break;
	}
	return QDialog::eventFilter(target,event);
}


//*****************************************************************************
CTwoInputDialog::CTwoInputDialog (QWidget * parent) : CBaseDialog (parent)
{
	label0 = new QLabel (this);
	label0->setFont(sizeM.getFont (CSizeManager::_FONT_BIG));
	label1 = new QLabel (this);
	label1->setFont(sizeM.getFont (CSizeManager::_FONT_BIG));

	edit0  = new CLineEdit (this);
	edit1  = new CLineEdit (this);
	edit1->setEchoMode(QLineEdit::PasswordEchoOnEdit);

	edit0->installEventFilter (this);
	edit1->installEventFilter (this);
	infoBar->setButtonsMask (0);

	labelButton = new QLabel(this);
	labelButton->setObjectName("labelButton");
	labelButton->setText(tr("Save"));
	labelButton->setStyleSheet("QLabel {border-color: grey; border-style: solid; border-width: 3px;}");
	labelButton->setStyleSheet(labelButton->styleSheet() + "QLabel:focus{background-color: #004d79;}");
	labelButton->setAlignment(Qt::AlignCenter);
	labelButton->setFixedSize(300, 80);
	labelButton->move(this->width()/2 - labelButton->width()/2, this->height()-100);
	labelButton->setFont(sizeM.getFont (CSizeManager::_FONT_BIG));
	labelButton->installEventFilter(this);
	labelButton->setFocusPolicy(Qt::TabFocus);
}

//*****************************************************************************
void CTwoInputDialog::resizeEvent (QResizeEvent * e)
{
	label0->move(30,160);
	label1->move(30,230);
	edit0-> move(280,160);
	edit1-> move(280,230);
	edit0->setFixedWidth(this->width()-300);
	edit1->setFixedWidth(this->width()-300);
	CBaseDialog::resizeEvent (e);
}

//*****************************************************************************
bool CTwoInputDialog::eventFilter(QObject *target, QEvent *event)
{
	if(event->type() != QEvent::KeyPress/*KeyRelease*/)		{
		return false;
	}
	QKeyEvent *ke = static_cast<QKeyEvent *>(event);
	switch(ke->key())	{
	case Qt::Key_Up:
		focusNextPrevChild(false);
		return true;
	case Qt::Key_Down:
		focusNextPrevChild(true);
		return true;

	case Qt::Key_Return:
		if(target == edit0 || target == edit1){
			CScreenKeyboardDialog::run(this, static_cast<QLineEdit *>(this->focusWidget()));
			edit1->setEchoMode(QLineEdit::PasswordEchoOnEdit);
		}
		else{
			if(edit0->text().isEmpty() || edit1->text().isEmpty()){
				CYesDialog d;
				d.setTitle(tr("Empty data"));
				d.setText(tr("Please input login and password"));
				d.setFixedWidth(650);
				d.exec();
			}
			else
				accept();
		}
	break;

	case Qt::Key_Escape:
		reject ();
	break;

	default:
		break;
	}
	return QDialog::eventFilter(target,event);
}

//*****************************************************************************
QStringList CTwoInputDialog::getText (QString title, QString lb0, QString lb1, bool * ok)
{
	QStringList result;
	*ok = false;
	CTwoInputDialog dialog;
	dialog.setTitle (title);
	dialog.setLabel0 (lb0);
	dialog.setLabel1 (lb1);
	dialog.resize(800,400);
	if (dialog.exec () == QDialog::Accepted)
		*ok = true;
	result << dialog.line0() << dialog.line1();
	return result;
}

