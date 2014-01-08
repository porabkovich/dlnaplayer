#include "cshareslistdialog.h"
#include "cshareeditdialog.h"
#include "ui_shareslistdialog.h"
#ifndef QT_TEST
#include "mediaplayer.h"
#endif

#include <QSettings>
#include "csizemanager.h"
#include "cinfobar.h"
#include "dialogs.h"

const QString ShareSettingsKeys::blockPrefix = "shares";
const QString ShareSettingsKeys::name = "name";
const QString ShareSettingsKeys::ip = "ip";
const QString ShareSettingsKeys::path = "path";
const QString ShareSettingsKeys::username = "username";
const QString ShareSettingsKeys::password = "password";
const QString ShareSettingsKeys::type = "type";

extern CSizeManager sizeM;

CSharesListDialog::CSharesListDialog(QWidget *parent, CShareManager &manager) :
	QDialog(parent), ui(new Ui::CSharesListDialog), manager_(manager)
{
	ui->setupUi(this);
#ifndef QT_TEST
	setWindowFlags(Qt::FramelessWindowHint);
	setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_DIALOG) + sizeM.getStyle (CSizeManager::_ITEM_LABEL));
	QFont font = sizeM.getFont (CSizeManager::_FONT_BIG);

	infoBar_ = new CInfoBar (this);
	infoBar_->setFont (font);
	infoBar_->move (20, height () - 70);
	infoBar_->setButtonsMask (
							CInfoBar::B_RETURN |
							CInfoBar::B_OK |
							CInfoBar::B_RED |
							CInfoBar::B_GREEN);

	infoBar_->setButtonDescription (CInfoBar::B_GREEN, tr("Add"));
	infoBar_->setButtonDescription (CInfoBar::B_RED, tr("Remove"));
	infoBar_->setButtonDescription (CInfoBar::B_OK, tr("Edit"));
	infoBar_->setButtonDescription (CInfoBar::B_RETURN, tr("Exit"));

	ui->title->setFont(font);
	ui->sharesList->setFont(font);

	ui->title->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_LABEL));
	ui->sharesList->setStyleSheet(sizeM.getStyle(CSizeManager::_ITEM_LISTVIEW));

	QSettings sett(CMediaPlayer::settingsFilename, QSettings::NativeFormat);
	shares_ = loadSettings(sett);
	this->refreshList();
#endif
}

CSharesListDialog::~CSharesListDialog()
{
#ifndef QT_TEST
	QSettings sett(CMediaPlayer::settingsFilename, QSettings::NativeFormat);
	saveSettings(sett, shares_);
	delete ui;
	delete infoBar_;
#endif
}

QVector<QSharedPointer<SShare> > CSharesListDialog::loadSettings(QSettings &sett)
{
	QVector<QSharedPointer<SShare> > shares;
	int size = sett.beginReadArray(ShareSettingsKeys::blockPrefix);
	for(int i = 0; i < size; ++i)
	{
		sett.setArrayIndex(i);
		QSharedPointer<SShare> p(new SShare);
		p->name = sett.value(ShareSettingsKeys::name).toString();
		p->ip = sett.value(ShareSettingsKeys::ip).toString();
		p->path = sett.value(ShareSettingsKeys::path).toString();
		p->username = sett.value(ShareSettingsKeys::username).toString();
		p->password = sett.value(ShareSettingsKeys::password).toString();
		p->type = (SShare::Type) sett.value(ShareSettingsKeys::type).toInt();
		shares.push_back(p);
	}
	sett.endArray();
	return shares;
}

void CSharesListDialog::saveSettings(QSettings &sett, const QVector<QSharedPointer<SShare> > &shares)
{
	sett.beginGroup(ShareSettingsKeys::blockPrefix);
	sett.remove("");
	sett.endGroup();

	sett.beginWriteArray(ShareSettingsKeys::blockPrefix);
	QVector<QSharedPointer<SShare> >::ConstIterator it = shares.constBegin();
	for(int i = 0; it != shares.constEnd(); ++i, ++it)
	{
		sett.setArrayIndex(i);
		sett.setValue(ShareSettingsKeys::name, (*it)->name);
		sett.setValue(ShareSettingsKeys::ip, (*it)->ip);
		sett.setValue(ShareSettingsKeys::path, (*it)->path);
		sett.setValue(ShareSettingsKeys::username, (*it)->username);
		sett.setValue(ShareSettingsKeys::password, (*it)->password);
		sett.setValue(ShareSettingsKeys::type, (int) (*it)->type);
	}
	sett.endArray();
	sett.sync();
}

void CSharesListDialog::refreshList()
{
	ui->sharesList->clear();
	QVector<QSharedPointer<SShare> >::ConstIterator it;
	for(it = shares_.constBegin(); it != shares_.constEnd(); ++it)
		new QListWidgetItem((*it)->name, ui->sharesList);
}

void CSharesListDialog::keyPressEvent(QKeyEvent *event)
{
	switch(event->key())
	{
	case Qt::Key_Return:
		editShare();
		break;

	case Qt::Key_F9:
		removeShare();
		break;

	case Qt::Key_F10:
		addShare();
		break;

	case Qt::Key_Escape:
		this->close();
		break;

	default:
		break;
	}
}

void CSharesListDialog::addShare()
{
#ifndef QT_TEST
	CShareEditDialog d(0, shares_, tr("Add resource"));
	if(d.exec() == QDialog::Accepted)
	{
		shares_.push_back(d.getShare());
		manager_.addAutoShare(shares_.back());
		manager_.tryMountAutoShare(shares_.back());
		this->refreshList();
	}
#endif
}

void CSharesListDialog::editShare()
{
#ifndef QT_TEST
	if(ui->sharesList->currentItem())
	{
		int current = ui->sharesList->currentRow();
		CShareEditDialog d(0, shares_, tr("Edit resource"), shares_[current]);
		QSharedPointer<SShare> old(new SShare(*shares_[current]));
		if(d.exec() == QDialog::Accepted)
		{
			this->refreshList();
			ui->sharesList->setCurrentRow(current);
			manager_.removeAutoShare(old);
			manager_.removeShare(old);
			manager_.addAutoShare(shares_[current]);
			manager_.tryMountAutoShare(shares_[current]);
		}
	}
#endif
}

void CSharesListDialog::removeShare()
{
#ifndef QT_TEST
	if(ui->sharesList->currentItem())
	{
		CYesNoDialog d;
		d.setTitle(tr("Are you sure want to delete resource:"));
		d.setText("<b>" + ui->sharesList->currentItem()->text() + "</b>");
		if(d.exec() == QDialog::Accepted)
		{
			manager_.removeAutoShare(shares_.at(ui->sharesList->currentRow()));
			manager_.removeShare(shares_.at(ui->sharesList->currentRow()));
			shares_.remove(ui->sharesList->currentRow());
			this->refreshList();
		}
	}
#endif
}
