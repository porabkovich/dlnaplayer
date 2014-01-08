#ifndef CSHARESLISTDIALOG_H
#define CSHARESLISTDIALOG_H

#include "dialogs.h"
#include "cinfobar.h"
#include "csharemounterinterface.h"
#include "csharemanager.h"
#include <QSettings>

namespace ShareSettingsKeys
{
	extern const QString blockPrefix;
	extern const QString name;
	extern const QString ip;
	extern const QString path;
	extern const QString username;
	extern const QString password;
	extern const QString type;
}

namespace Ui
{
	class CSharesListDialog;
}

class CSharesListDialog : public QDialog
{
	Q_OBJECT

	Ui::CSharesListDialog *ui;
	CInfoBar *infoBar_;
	QVector<QSharedPointer<SShare> > shares_;
	CShareManager &manager_;

	void keyPressEvent(QKeyEvent *event);
	void refreshList();
	void addShare();
	void editShare();
	void removeShare();

public:
	explicit CSharesListDialog(QWidget *parent, CShareManager &manager);
	~CSharesListDialog();

	static QVector<QSharedPointer<SShare> > loadSettings(QSettings &sett);
	static void saveSettings(QSettings &sett, const QVector<QSharedPointer<SShare> > &shares);


};

#endif // CSHARESLISTDIALOG_H
