#include "cshareeditdialog.h"
#include "ui_shareeditdialog.h"
#include <QRegExpValidator>

#ifndef QT_TEST
#include <cscreenkeyboard.h>
#include "csizemanager.h"
#include "cinfobar.h"

extern CSizeManager sizeM;
#endif

CShareEditDialog::CShareEditDialog(QWidget *parent,
								   const QVector<QSharedPointer<SShare> > &shares,
								   QString title,
								   QSharedPointer<SShare> share) :
	QDialog(parent), share_(share), shares_(shares), ui(new Ui::CShareEditDialog)
{
	ui->setupUi(this);
#ifndef QT_TEST
	setWindowFlags(Qt::FramelessWindowHint);
	setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_DIALOG) + sizeM.getStyle (CSizeManager::_ITEM_LABEL));
	QFont bigfont = sizeM.getFont (CSizeManager::_FONT_BIG);
	QFont medfont = sizeM.getFont (CSizeManager::_FONT_MED);

	ui->title->setFont(bigfont);
	ui->title->setText(title);

	ui->labelDirectory->setFont(medfont);
	ui->labelIP->setFont(medfont);
	ui->labelLogin->setFont(medfont);
	ui->labelName->setFont(medfont);
	ui->labelPassword->setFont(medfont);
	ui->labelType->setFont(medfont);

	ui->title->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_LABEL));
	ui->labelDirectory->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_LABEL));
	ui->labelIP->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_LABEL));
	ui->labelLogin->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_LABEL));
	ui->labelName->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_LABEL));
	ui->labelPassword->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_LABEL));
	ui->labelType->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_LABEL));

	ui->dir->setFont(medfont);
	ui->ip->setFont(medfont);
	ui->login->setFont(medfont);
	ui->name->setFont(medfont);
	ui->password->setFont(medfont);
	ui->sharetype->setFont(medfont);

	ui->dir->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_EDIT));
	ui->ip->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_EDIT));
	ui->login->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_EDIT));
	ui->name->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_EDIT));
	ui->password->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_EDIT));
	ui->sharetype->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_COMBO) + sizeM.getStyle(CSizeManager::_ITEM_COMBO_DROPDOWN));
	ui->dir->setFrame(true);
	ui->horizontalSpacer->changeSize(ui->labelLogin->sizeHint().width(), 0);
	ui->dir->installEventFilter(this);
	ui->ip->installEventFilter(this);
	ui->login->installEventFilter(this);
	ui->name->installEventFilter(this);
	ui->password->installEventFilter(this);
	ui->sharetype->installEventFilter(this);
	connect(ui->sharetype, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChanged(int)));

	ui->sharetype->addItem("NFS_UDP", QVariant((int) SShare::Nfs_Udp));
	ui->sharetype->addItem("NFS_TCP", QVariant((int) SShare::Nfs_Tcp));
	ui->sharetype->addItem("SAMBA", QVariant((int) SShare::Samba));
	ui->sharetype->addItem("FTP", QVariant((int) SShare::Ftp));
	int indexResource = 0;
	QVector<QSharedPointer<SShare> >::const_iterator it;
	for(it = shares_.constBegin(); it != shares_.constEnd(); ++it){
		if((*it)->name.startsWith("New_Resource")){
			QString cutName = (*it)->name;
			cutName.remove("New_Resource");
			QRegExp re("\\d*");
			if (re.exactMatch(cutName)){
				if(indexResource <= cutName.toInt())
					indexResource = cutName.toInt()+1;
			}
		}
	}

	if(share_)
	{
		ui->name->setText(share->name);
		ui->ip->setText(share->ip);
		ui->dir->setText(share->path);
		ui->login->setText(share->username);
		ui->password->setText(share->password);
		ui->sharetype->setCurrentIndex(ui->sharetype->findData(share_->type));
	}
	else{
		ui->name->setText(tr("New_Resource") + (indexResource == 0 ? QString("") : QString::number(indexResource)));
	}
	labelButton = new QLabel(this);
	labelButton->setObjectName("labelButton");
	labelButton->setText(tr("Save"));
	labelButton->setStyleSheet("QLabel {border-color: grey; border-style: solid; border-width: 3px;}");
	labelButton->setStyleSheet(labelButton->styleSheet() + "QLabel:focus{background-color: #004d79;}");
	labelButton->setAlignment(Qt::AlignCenter);
	labelButton->setFixedSize(250, 80);
	labelButton->move(this->width()/2 - labelButton->width()/2, this->height()-100);
	labelButton->setFont(medfont);
	labelButton->installEventFilter(this);
	labelButton->setFocusPolicy(Qt::TabFocus);


	ui->sharetype->setFocus();
	ui->sharetype->setFrame(true);
	ui->sharetype->setStyleSheet(ui->sharetype->styleSheet() + "QComboBox {border:2px solid #004d79;}");
	shown = false;
#endif
}

void CShareEditDialog::paintEvent(QPaintEvent *e)
{
	QDialog::paintEvent(e);
	if(!shown)
	{
//		QKeyEvent dropdown_event(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
//		ui->sharetype->event(&dropdown_event);

//		ui->sharetype->setStyleSheet( "QComboBox { border: 1px solid rgb(0, 0, 255);"
//									  "background: transparent; }");
		shown = true;
	}
}

CShareEditDialog::~CShareEditDialog()
{
#ifndef QT_TEST
	delete ui;
#endif
}

QSharedPointer<SShare> CShareEditDialog::getShare()
{
	return share_;
}

bool CShareEditDialog::eventFilter(QObject *object, QEvent *event)
{
	if(event->type() == QEvent::KeyPress)
	{
		QKeyEvent *ke = static_cast<QKeyEvent *>(event);
		switch(ke->key())
		{
		case Qt::Key_Return:
			if(object == labelButton){
				saveShare();
				return true;
			}

			if(object == ui->sharetype){
				QKeyEvent dropdown_event(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier);
				ui->sharetype->event(&dropdown_event);
				break;
			}
#ifndef QT_TEST
			CScreenKeyboardDialog::run(this, static_cast<QLineEdit *>(this->focusWidget()));
			if(object == ui->password)
				ui->password->setEchoMode(QLineEdit::PasswordEchoOnEdit);
#endif
			break;

		case Qt::Key_Down:
			this->focusNextChild();
			if(this->focusWidget() == ui->sharetype)
				ui->sharetype->setStyleSheet(ui->sharetype->styleSheet() + "QComboBox {border:2px solid #004d79;}");
			 else
				ui->sharetype->setStyleSheet(ui->sharetype->styleSheet() + "QComboBox {border:2px solid #000000;}");
			return true;
			break;

		case Qt::Key_Up:
			this->focusPreviousChild();
			if(this->focusWidget() == ui->sharetype)
				ui->sharetype->setStyleSheet(ui->sharetype->styleSheet() + "QComboBox {border:2px solid #004d79;}");
			else
				ui->sharetype->setStyleSheet(ui->sharetype->styleSheet() + "QComboBox {border:2px solid #000000;}");
			return true;
			break;

		case Qt::Key_Dead_Diaeresis:
			if(object == ui->sharetype)
				break;
#ifndef QT_TEST
			CScreenKeyboardDialog::run(this, static_cast<QLineEdit *>(this->focusWidget()));
#endif
			break;
case Qt::Key_Escape:
			break;
		default:
			break;
		}
	}
	return false;
}

bool CShareEditDialog::isIPValid(const QString &s) const
{
	QStringList bytes = s.split(".");
	foreach(QString byte, bytes)
	{
		// Cannot be < 0 because only digits are allowed
		// in text field
		if(byte == "" || byte.toInt() > 255)
			return false;
	}
	return true;
}

bool CShareEditDialog::isFolderNameValid(const QString &s) const
{
	QString errorText="";
	QString inputString = s;
	int index = 0;
	QRegExp validURLRegex("[^\?\\*\"\\>\\<\\:\\|]*$");
	QRegExpValidator validator(validURLRegex, 0);

	SShare::Type shareType = SShare::Type(ui->sharetype->itemData(ui->sharetype->currentIndex()).toInt());
	if(shareType == SShare::Ftp && inputString.isEmpty())
		return true;

	if(inputString.isEmpty())
		errorText = tr("Directory must have name");
	if (validator.validate(inputString, index) != QValidator::Acceptable || inputString.startsWith("."))
		errorText = tr("Directory must have name without special symbols.");

	if(!errorText.isEmpty()){
#ifndef QT_TEST
			CYesDialog d;
			d.setTitle(tr("Invalid name"));
			d.setText(errorText);
			d.exec();
#endif
			ui->dir->setFocus();
			return false;
	}
	return true;
}

bool CShareEditDialog::isNameValid(const QSharedPointer<SShare> &p, const QString &s) const
{
	QString errorText="";

	QRegExp validURLRegex("[^\\\\/\?\\*\"\\>\\<\\:\\|]*$");
	QRegExpValidator validator(validURLRegex, 0);
	int index = 0;
	QString inputString = s;
	if (validator.validate(inputString, index) != QValidator::Acceptable || inputString == ".")
		errorText = tr("Resource must have name without special symbols.");
	QVector<QSharedPointer<SShare> >::const_iterator it;
	for(it = shares_.constBegin(); it != shares_.constEnd(); ++it)
		if(s == (*it)->name && p != *it)
			errorText = tr("Resource must have unique name.");
	if(!errorText.isEmpty()){
#ifndef QT_TEST
		CYesDialog d;
		d.setTitle(tr("Invalid name"));
		d.setText(errorText);
		d.exec();
		ui->name->setFocus();
#endif
		return false;
	}
	return true;
}

void CShareEditDialog::saveShare()
{
	QString name = ui->name->text().simplified();
	QString ip = ui->ip->text();
	ip.replace (QRegExp("^0*([1-9]?[0-9])"), "\\1");
	ip.replace (QRegExp("\\.0*([1-9]?[0-9])"), ".\\1");
	if(!isIPValid(ip))
	{
#ifndef QT_TEST
		CYesDialog d;
		d.setTitle(tr("Invalid IP"));
		d.setText(tr("IP address must consist of four numbers from range 0-255."));
		d.exec();
#endif
		ui->ip->setFocus();
		return;
	}
	if(name.isEmpty()){
#ifndef QT_TEST
		CYesDialog d;
		d.setTitle(tr("Invalid name"));
		d.setText(tr("Resource must have name"));
		d.exec();
#endif
		ui->name->setFocus();
		return;
	}
	else if(!isNameValid(share_, name))
		return;

	SShare::Type shareType = SShare::Type(ui->sharetype->itemData(ui->sharetype->currentIndex()).toInt());

	if(!isFolderNameValid(ui->dir->text()))
		return;

	if(!share_)
		share_ = QSharedPointer<SShare>(new SShare);

	share_->name = name;
	share_->ip = ip;
	share_->path = ui->dir->text();
	share_->username = ui->login->text();
	share_->password = ui->password->text();
	share_->type = shareType;
	this->accept();
}

void CShareEditDialog::onIndexChanged(int index)
{
	SShare::Type shareType = SShare::Type(ui->sharetype->itemData(index).toInt());
	if(shareType == SShare::Nfs_Tcp || shareType == SShare::Nfs_Udp)
	{
		ui->labelLogin->hide();
		ui->labelPassword->hide();
		ui->login->hide();
		ui->password->hide();
	}
	else
	{
		ui->labelLogin->show();
		ui->labelPassword->show();
		ui->login->show();
		ui->password->show();
	}
}

#ifndef QT_TEST
CInfoDialog::CInfoDialog(QWidget *parent) : QDialog(parent), title_(this),
	message_(this), infoBar(this)
{
	setWindowFlags(Qt::FramelessWindowHint);
	QFont bigfont = sizeM.getFont(CSizeManager::_FONT_BIG);
	QFont medfont = sizeM.getFont(CSizeManager::_FONT_MED);

	setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_DIALOG) + sizeM.getStyle(CSizeManager::_ITEM_LABEL));
	setFixedHeight(400);
	title_.setFont(bigfont);
	title_.move(20,20);
	title_.setFixedWidth(width()-40);
	title_.setAlignment(Qt::AlignCenter);
	title_.setWordWrap(true);
	title_.show();

	message_.setFont(medfont);
	message_.move(20, 100);
	message_.setFixedWidth(width()-40);
	message_.setFixedHeight(200);
	message_.setAlignment(Qt::AlignCenter);
	message_.setWordWrap(true);
	message_.show();

	infoBar.setFont (bigfont);
	infoBar.move (20, height () - 70);
	infoBar.setButtonsMask (CInfoBar::B_RETURN);
	infoBar.setButtonDescription(CInfoBar::B_RETURN, tr("Back"));

}
#else
CInfoDialog::CInfoDialog(QWidget *parent) : QDialog(parent), title_(this),
	message_(this)
{}
#endif

void CInfoDialog::setTitle(const QString &title)
{ title_.setText(title); }

void CInfoDialog::setMessage(const QString &message)
{ message_.setText(message); }
