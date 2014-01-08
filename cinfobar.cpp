#include "cinfobar.h"
#include "tools.h"
#include "csizemanager.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <QLabel>
#include <QTimer>


#ifndef GLOBAL_DEBUG_ENABLE
#define NDEBUG
#endif
#include "debugmacros.h"

CInfoBarButton::CInfoBarButton(QWidget *parent):QWidget(parent),descVisible_(true)
{
	img = new QLabel(this);
	img->move(0,0);
	img->show();
	desc = new QLabel(this);
	desc->show();
	updGeometry();
}

CInfoBarButton::CInfoBarButton(const QPixmap &pixmap, const QString &str, QWidget *parent)
	:QWidget(parent),descVisible_(true)
{
	img = new QLabel(this);
	img->setPixmap(pixmap);
	img->setFixedSize(img->sizeHint());
	img->show();
	desc = new QLabel(str,this);
	desc->setFixedSize(desc->sizeHint());
	desc->show();
	updGeometry();
}


void CInfoBarButton::updGeometry()
{
	desc->setFixedSize(0,0);		// without this it does not work
	desc->setFixedSize(desc->sizeHint());
	img->setFixedSize(0,0);		// without this it does not work
	img->setFixedSize(img->sizeHint());
	if(!descVisible_ || desc->text().isEmpty())
	{
		setFixedSize(img->size());
		img->move(0,0);
		return;
	}
	const int clearance = 5;
	if(img->size().height() >= desc->size().height())	{
		int center = img->size().height() / 2;
		img->move(0,0);
		desc->move(img->size().width() + clearance,center - desc->size().height()/2);
		setFixedSize(img->width() + clearance + desc->width(),img->height());
	}
	else	{
		int center = desc->size().height() / 2;
		img->move(0,center - img->size().height()/2);
		desc->move(img->size().width() + clearance,0);
		setFixedSize(img->width() + clearance + desc->width(),desc->height());
	}
}

void CInfoBarButton::setImg(const QPixmap & pixmap)
{
	img->setPixmap(pixmap);
	updGeometry();
}

void CInfoBarButton::setText(const QString & str)
{
	desc->setText(str);
	updGeometry();
}

void CInfoBarButton::setFont (QFont font)
{
	desc->setFont (font);
	updGeometry ();
}

void CInfoBarButton::setDescVisible(bool v)
{
	desc->setVisible(v);
	descVisible_ = v;
	updGeometry();
}

QString CInfoBarButton::text() const
{
	return desc->text();
}

CInfoBar::CInfoBar(QWidget * parent):QWidget(parent),mask(0),gap(60)
{

	std::list<QString> imgFiles,descrip;

	imgFiles.push_back(":/mpimg/buttons/up.png");
	descrip.push_back(tr("Move"));
	imgFiles.push_back(":/mpimg/buttons/down.png");
	descrip.push_back(tr("Move"));
	imgFiles.push_back(":/mpimg/buttons/left.png");
	descrip.push_back(tr("Move"));
	imgFiles.push_back(":/mpimg/buttons/right.png");
	descrip.push_back(tr("Move"));
	imgFiles.push_back(":/mpimg/buttons/ok.png");
	descrip.push_back(tr("Ok"));
	imgFiles.push_back(":/mpimg/buttons/return.png");
	descrip.push_back(tr("Return"));
	imgFiles.push_back(":/mpimg/buttons/red.png");
	descrip.push_back(tr("Delete"));
	imgFiles.push_back(":/mpimg/buttons/green.png");
	descrip.push_back(tr("Add"));
	imgFiles.push_back(":/mpimg/buttons/yellow.png");
	descrip.push_back("");
	imgFiles.push_back(":/mpimg/buttons/blue.png");
	descrip.push_back("");
	imgFiles.push_back(":/mpimg/buttons/menu.png");
	descrip.push_back(tr("Menu"));

	std::list<QString>::iterator it=imgFiles.begin(),it1 = descrip.begin();
	unsigned long i=0;
	for(;it!=imgFiles.end();++i,++it,++it1)
	{
		cont[EButton(1UL<<i)] = new CInfoBarButton(QPixmap(*it),*it1,this);
		cont[EButton(1UL<<i)] ->show();
		mask |=1UL<<i;
	}
	imgFiles.clear();
	descrip.clear();

	cont[B_UP]->setDescVisible(false);
	cont[B_DOWN]->setDescVisible(false);
	cont[B_LEFT]->setDescVisible(false);
	updGeometry();
}

//QString CInfoBar::makeImageFilename(const QString &str)
//{
//	QString tmpStr = dir + IS::imagesFolder + IS::buttonsSubfolder;
//	tmpStr += str + sizeM.getResAsString() + IS::png;
//	return tmpStr;
//}

void CInfoBar::updGeometry()
{
	int offset = 0;
	const int cursor_gap = 10;

	if(mask & B_LEFT)	{
		cont[B_LEFT]->show();
		cont[B_LEFT]->move(offset,getVertPos(height(),cont[B_LEFT]->size()));
		offset += cont[B_LEFT]->width() + cursor_gap;
	}
	else	{
		cont[B_LEFT]->hide();
	}

	if(mask & B_UP)	{
		cont[B_UP]->show();
		cont[B_UP]->move(offset,5);
	}
	else	{
		cont[B_UP]->hide();
	}
	if(mask & B_DOWN)	{
		cont[B_DOWN]->show();
		cont[B_DOWN]->move(offset,45);
		offset += cont[B_DOWN]->width() + cursor_gap;
	}
	else	{
		cont[B_DOWN]->hide();
		if(mask & B_UP)	{
			offset += cont[B_DOWN]->width() + cursor_gap;
		}
	}

	unsigned long i=3;

	while(i<cont.size())	{
		if(mask & (1UL << i))	{
			cont[EButton(1UL << i)]->show();
			cont[EButton(1UL << i)]->move(offset,getVertPos(height(),cont[EButton(1UL << i)]->size()));
			offset += cont [EButton(1UL << i)]->width() + gap;
			setFixedWidth (offset);
			if (height() < cont[EButton(1UL << i)]->height())
				setFixedHeight (cont[EButton(1UL << i)]->height());
		}
		else	{
			cont[EButton(1UL << i)]->hide();
		}
		++i;
	}
}

void CInfoBar::setFont (QFont font)
{
	unsigned int i = 0;
	while( i < cont.size() )	{
		cont[EButton(1UL << i)]->setFont (font);
		++i;
	}
}

void CInfoBar::setGeometry(int x,int y,int w,int h)
{
	QWidget::setGeometry(x,y,w,h);
	updGeometry();
}

void CInfoBar::setFixedSize(int w,int h)
{
	QWidget::setFixedSize(w,h);
	updGeometry();
}

int CInfoBar::getVertPos(int h, const QSize &size)
{
	return h/2 - size.height()/2;
}

void CInfoBar::setButtonsMask(unsigned long msk)
{
	unsigned long i=0;
	mask = 0;
	while( i < cont.size() )	{
		if(msk & (1UL << i))	{
			mask |= (1UL << i);
		}
		++i;
	}
	updGeometry();
}

void CInfoBar::setButtonDescription(EButton but,const QString &str)
{
	switch(but)	{
	case B_UP:
	case B_DOWN:
	case B_LEFT:
	case B_RIGHT:
	case B_OK:
	case B_RETURN:
	case B_RED:
	case B_GREEN:
	case B_YELLOW:
	case B_BLUE:
	case B_MENU:
		cont[but]->setText(str);
		updGeometry();
		break;

	default:
		ERROR_INFO_TWO("unknown button:",(unsigned long)but);
	}
}

void CInfoBar::setupView ()
{
	if (sizeM.getMode () == CSizeManager::HD_MODE) {
		setFont (QFont ("Century Gothic", 45, QFont::Normal));
		setStyleSheet ("QLabel {color:#FFFFFF}");
	} else {
		setFont (QFont ("Century Gothic", 45, QFont::Bold, true));
		setStyleSheet ("QLabel {color:#808080}");
	}
	updGeometry ();	
}

void CInfoBar::setChecked(EButton btn){
	unsigned int i = 0;
	while( i < cont.size() ){
		cont[EButton(1UL << i)]->setStyleSheet("QLabel {color:#969696}");
		++i;
	}
	cont[btn]->setStyleSheet("QLabel {color:#FFFFFF}");
}


