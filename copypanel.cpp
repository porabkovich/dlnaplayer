#include "copypanel.h"

#include <QGroupBox>
#include <QProgressBar>
#include <QLabel>

#include "tools.h"

#define MODULE "mediaplayer"
#ifndef GLOBAL_DEBUG_ENABLE
#define NDEBUG
#endif
#include "debugmacros.h"
//#include <QDebug>
const QString groupBoxStyleSheet =
		"QGroupBox {"
		"	border: 3px solid rgb(37, 90, 225);"
		"	border-radius: 10px;"
"background-color:rgb(0,0,0, 255);"
		"}";

const QString progressBarStyleSheet =
		"QProgressBar {"
		"	border: 2px solid rgb(37, 90, 225,255);"
		"	border-radius: 10px;"
		"	text-align: center;"
		"}";

CCopyPanel::CCopyPanel(QWidget *parent):QWidget(parent)
{
	box = new QGroupBox(this);
	box->setFixedSize(818,110);
	box->setStyleSheet(groupBoxStyleSheet);

	QFont font;
	font.setPointSize(25);
	setFont(font);

	pBar = new QProgressBar(box);
	pBar->setGeometry(15,55,788,30);
//	pBar->setInvertedAppearance(true);
//    pBar->setMinimum(0);
//    pBar->setMaximum(1000);
//    pBar->setValue(33);
	pBar->setStyleSheet(progressBarStyleSheet);
	pBar->setFont(font);

	label = new QLabel(box);
	label->setGeometry(15,10,730,30);
	label->setFont(font);
	label->setText(tr("Copy: "));
}

void CCopyPanel::setFileName(const QString &str)
{
//	label->setText(tr("Copy: ") + str);
	label->setFont(QFont ("Century Gothic", 24, QFont::Normal));
	label->setText( "<font color=\"white\">"+tr("Copy: ") + str+"</font>" );
}
void CCopyPanel::setTitleText(const QString &str)
{
	label->setText( "<font color=\"white\">"+tr("Deleting: ") + str+"</font>" );
}
void CCopyPanel::setMaximum(int max)
{
	pBar->setMaximum(max);
}

void CCopyPanel::setMinimum(int min)
{
	pBar->setMinimum(min);
}

void CCopyPanel::setValue(int val)
{
	pBar->setValue(val);
}













