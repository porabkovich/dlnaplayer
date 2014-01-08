#include "csizemanager.h"
#include <iostream>
#include <QCoreApplication>
#include <QStyleFactory>
#include <QSize>
#include <QPixmap>
#include <QFile>
#include <QFont>


#ifndef GLOBAL_DEBUG_ENABLE
#define NDEBUG
#endif
#include "debugmacros.h"


const int controlWidth_sd = 840;
const int controlWidth_hd = 1200;
const int controlHeight = 60;
const int controlEditX0_hd = 490;
const int controlEditX0_sd = 380;
const int controlEnableColor_hd = 0xFFFFFF;
const int controlDisableColor_hd = 0x808080;
const int controlEnableColor_sd = 0x909090;
const int controlDisableColor_sd = 0x404040;
const QFont controlFont_sd ("Century Gothic", 36, QFont::Bold, true);
const QFont controlFont_hd ("Century Gothic", 36, QFont::Normal);

ProxyStyle::ProxyStyle(QWidget *widget) : QStyle()
{
	Q_UNUSED(widget)
	m_style = QStyleFactory::create(/*widget->style()->objectName()*/"plastique");
				if (!m_style) {
					ERROR_INFO_ONE ("Style not created");
				}
}





CSizeManager::CSizeManager()
{
	currentMode = HD_MODE;
}

int CSizeManager::getFontSize (void)
{
	return 36;
}


QString CSizeManager::getStyle (eGuiItemID item)
{
	QString colorEnable;
	QString colorDisable;

	if (getMode() == SD_MODE ) {
		colorEnable = "#999999";
		colorDisable = "#666666";
	} else {
		colorEnable = "#ffffff";
		colorDisable = "#888888";
	}

	switch (item) {
		case _ITEM_TABLEVIEW:
			return " QTableView {"
			"	border: 0px;"
			"	border-radius: 10px;"
			"	background-color: black;"
			"}"
			"QTableView::item {"
			"	color: white;"
			" background: black;"
			" border: 0px;"
			"}" 
			"QTableView::item:selected {"
			"	background-image: url(%1);"
			" background-color: black;"
			"}"
			" QTableView::item:selected:active{"
			"	background-image: url(%1);"
			"			background-color: black;"
			"}";
		case _ITEM_TABLEVIEW_NOSELECT:
			return " QTableView {"
			"	border: 0px;"
			"	border-radius: 10px;"
			"	background-color: black;"
			"}"
			"QTableView::item {"
			"	color: white;"
			" background: black;"
			" border: 0px;"
			"}"; 
		case _ITEM_LISTVIEW:
			return " QListView {"
			"	border: 0px;"
			"	border-radius: 10px;"
			"	background-color: black"
			"}"
			"QListView::item {"
			"	color: white;"
			" background: black;"
			" border: 0px;"
			"}" 
			"QListView::item:selected {"
			" background-color: #222740;"
			"}"

			" QListView::item:selected:active{"
			"			background-color: #222740;"
			"}";

		case _ITEM_CONTEXTMENU:
		return " QMenu {"
		"	border: 8px solid #1B799E;"
		"	border-radius: 15px;"
		"	background-color: black;"
		"	margin: 2px;"
		" icon-size: 48px;" 
		"}"
		"QMenu::item {"
		" icon-size: 48px;" 
		"	color: #999999;"
		"	padding: 2px 25px 2px 40px;"
		"	border: 0px solid transparent;"
		"}"
		"QMenu::item:selected {"
		"	color: #000000;"
		" border-color: "+colorEnable+";"
		" background: #1B799E;"
		"	border-radius: 4px;"
		"}"
		"QMenu::item:icon {"
		" position: absolute;"
		" top: 1px;"
		" right: 100px;"
		" bottom: 1px;"
		" left: 1px;"
		" icon-size: 48px;" 
		"}";

		case _ITEM_DIALOG:
			return
		" QDialog {"
			" background-color: black;"
			" background: black;"
			" border: 6px solid #666666;"
			"}";
		case _ITEM_VERT_SCROLL_SIMPLE:
			return
			"QScrollBar:vertical {"
			" border: 2px solid #606060;"
			" background: transparent;"
			" width: 20px;"
			" margin: 10px 0 10px 0;"
			"}"
			"QScrollBar::handle:vertical {"
			" background: #000090;"
			" border-radius: 9px;"
			" min-height: 30px;"
			" width: 20px;"
			"}"
			"QScrollBar::add-line:vertical {"
			" border: 0px;"
			" border-radius: 0px;"
			" background: transparent;"
			" height: 0px;"
			" subcontrol-position: bottom;"
			" subcontrol-origin: margin;"
			"}"
			"QScrollBar::sub-line:vertical {"
			" border: 0px;"
			" border-radius: 0px;"
			" background: transparent;"
			" height: 0px;"
			" subcontrol-position: top;"
			" subcontrol-origin: margin;"
			"}"
			"QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {"
			" border: 0px;"
			" width: 0px;"
			" height: 0px;"
			" background: transparent;"
			"}"
			"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
			" background: none;"
			"}";
		case _ITEM_VERT_SCROLL:
			return 
			"QScrollBar:vertical {"
			"	border: 0px solid white;"
			"	background: transparent;"
			"	width: 0px;"
			"	margin: 0px 0 0px 0;"
			"}"
			"QScrollBar::handle:vertical {"
			"	background: #3E98CC;"
			"	border-radius: 0px;"
			"	min-height: 0px;"
			"}"
			"QScrollBar::add-line:vertical {"
			"	border: 0px solid #808080;"
			"	border-radius: 0px;"
			"	background: #F2F9FF;"
			"	height: 0px;"
			"	subcontrol-position: bottom;"
			"	subcontrol-origin: margin;"
			"}"
			"QScrollBar::sub-line:vertical {"
			"	border: 0px solid #808080;"
			"	border-radius: 0px;"
			"	background: #F2F9FF;"
			"	height: 0px;"
			"	subcontrol-position: top;"
			"	subcontrol-origin: margin;"
			"}"
			"QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {"
			"	border: 0px solid blue;"
			"	width: 0px;"
			"	height: 0px;"
			"	background: blue;"
			"}"
			"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
			"	background: none;"
			"}";
		case _ITEM_HORIZ_SCROLL:
			return "QScrollBar:horizontal {"
			"	border: 0px solid white;"
			"	background: transparent;"
			"	height: 0px;"
			"	margin: 0px 0px 0px 0px;"
			"}"
			"QScrollBar::handle:horizontal {"
			"	background: #3E98CC;"
			"	border-radius: 0px;"
			"	min-width: 0px;"
			"}"
			"QScrollBar::add-line:horizontal {"
			"	border: 0px solid #808080;"
			"	border-radius: 0px;"
			"	background: #F2F9FF;"
			"	width: 0px;"
			"	subcontrol-position: right;"
			"	subcontrol-origin: margin;"
			"}"
			"QScrollBar::sub-line:horizontal {"
			"	border: 0px solid #808080;"
			"	border-radius: 0px;"
			"	background: #F2F9FF;"
			"	width: 0px;"
			"	subcontrol-position: left;"
			"	subcontrol-origin: margin;"
			"}"
			"QScrollBar::left-arrow:horizontal, QScrollBar::right-arrow:horizontal {"
			"	border: 0px solid blue;"
			"	width: 0px;"
			"	height: 0px;"
			"	background: blue;"
			"}"
			"QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
			"	background: none;"
			"}";

		case _ITEM_TREEVIEW:
		if (getMode() == HD_MODE) {
			return " QTreeView {"
			"	border: 0px;"
			"	border-radius: 10px;"
			"	background-color: rgba(0,0,0,0);"
			" selection-background-color : rgba(0,0,0,0);"
			"}"
			"QTreeView::item {"
			"	color: "+colorEnable+";"
			"	background: rgba(0,0,0,0);"
			" border: 0px;"
			" selection-background-color : rgba(0,0,0,0);"
			"}" 
			"QTreeView::item:selected {"
			"	background-color: rgba(0,0,0,0);"
			"}"
			"QTreeView::item:hover {"
			"	background-color: rgba(0,0,0,0);"
			"}"
			"QTreeView::item:selected:active{"
			"	background-color: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch {"
			"	background: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch:has-siblings:!adjoins-item {"
			" background: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch:has-siblings:adjoins-item {"
			" background: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch:!has-children:!has-siblings:adjoins-item {"
			" background: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch:has-children:!has-siblings:closed,"
			"QTreeView::branch:closed:has-children:has-siblings {"
			"	image: url(:/mpimg/branch-closed.png);"
			" background: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch:open:has-children:!has-siblings,"
			"QTreeView::branch:open:has-children:has-siblings  {"
			"	image: url(:/mpimg/branch-open.png);"
			" background: rgba(0,0,0,0);"
			"}";
		} else {
			return " QTreeView {"
			"	border: 0px;"
			"	border-radius: 10px;"
			"	background-color: rgba(0,0,0,0);"
			" selection-background-color : rgba(0,0,0,0);"
			"}"
			"QTreeView::item {"
			"	color: "+colorEnable+";"
			"	background: rgba(0,0,0,0);"
			" border: 0px;"
			" selection-background-color : rgba(0,0,0,0);"
			"}" 
			"QTreeView::item:selected {"
			"	background-color: rgba(0,0,0,0);"
			"}"
			"QTreeView::item:hover {"
			"	background-color: rgba(0,0,0,0);"
			"}"
			"QTreeView::item:selected:active{"
			"	background-color: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch {"
			"	background: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch:has-siblings:!adjoins-item {"
			" background: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch:has-siblings:adjoins-item {"
			" background: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch:!has-children:!has-siblings:adjoins-item {"
			" background: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch:has-children:!has-siblings:closed,"
			"QTreeView::branch:closed:has-children:has-siblings {"
			"	image: url(:/mpimg/branch-closed.png);"
			" background: rgba(0,0,0,0);"
			"}"
			"QTreeView::branch:open:has-children:!has-siblings,"
			"QTreeView::branch:open:has-children:has-siblings  {"
			"	image: url(:/mpimg/branch-open.png);"
			" background: rgba(0,0,0,0);"
			"}";
			/*return " QTreeView {"
			"	border: 0px;"
			"	border-radius: 10px;"
			"	background-color: black"
			"}"
			"QTreeView::item {"
			"	color: #999999;"
			" background: black;"
			" border: 0px;"
			"}" 
			"QTreeView::item:selected {"
			" background-color: black;"
			"}"

			" QTreeView::item:selected:active{"
//			"	background-image: black;"
			"			background-color: black;"
			" }"
			" QTreeView::branch {"
				"background: black;"
			"}"
			"QTreeView::branch:has-siblings:!adjoins-item {"
			"	border-image: url(:mpimg/vline.png) 0;"
			" background-color: black;"
			"}"
			"QTreeView::branch:has-siblings:adjoins-item {"
			" background-color: black;"
			"}"
			"QTreeView::branch:!has-children:!has-siblings:adjoins-item {"
			" background-color: black;"
			"}"
			"QTreeView::branch:has-children:!has-siblings:closed,"
			"QTreeView::branch:closed:has-children:has-siblings {"
			" background-color: black;"
			"	image: url(:/mpimg/branch-closed.png);"
			"}"
			"QTreeView::branch:open:has-children:!has-siblings,"
			"QTreeView::branch:open:has-children:has-siblings  {"
			" background-color: black;"
			"	image: url(:/mpimg/branch-open.png);"
			"}";*/
		}

		case _ITEM_EDIT:
			return "QLineEdit {background-color: rgba(0,0,0,0); color:"+colorEnable+"; selection-background-color:#004d79}";
		case _ITEM_EDIT_DISABLE:
			return "QLineEdit {background-color: rgba(0,0,0,0); color:"+colorDisable+"}";
		case _ITEM_COMBO_DROPDOWN:
			return "QAbstractItemView{background-color: #222222; color:"+colorEnable+"; selection-background-color:#555555; selection-color:#000000; border:0px}" /*"QAbstractItemView::selection {background-image: url(/sdk/pix.png);} QAbstractItemView::item:selected {background-image: url(/sdk/pix.png);}"*/;
		case _ITEM_COMBO:
			return "QComboBox {background-color:rgba(0,0,0,0); color:"+colorEnable+"; border:0px;} QComboBox::drop-down {border-width: 0px;} QComboBox::down-arrow {image: url(noimg); border-width: 0px;}";
		case _ITEM_COMBO_DISABLE:
			return "QComboBox {background-color:rgba(0,0,0,0); color:"+colorDisable+"; border:0px;} QComboBox::drop-down {border-width: 0px;} QComboBox::down-arrow {image: url(noimg); border-width: 0px;}"; 
		case _ITEM_LABEL_DISABLE:
			return "QLabel {color:"+colorDisable+"};";
		case _ITEM_LABEL:
		default:
			return "QLabel {color:"+colorEnable+"};";
	}
}

//*****************************************************************************
QFont CSizeManager::getFont (eFonts tfont)
{
  QFont font ("Century Gothic");
  switch (tfont) {

    case _FONT_BIG:
    if (getMode() == SD_MODE ) {
      font.setBold (true);
      font.setItalic (true);
    }
    font.setPointSize (getFontSize() + 13);
    font.setLetterSpacing (QFont::PercentageSpacing, 95);
    break;

    case _FONT_SML:
    if (getMode() == SD_MODE ) {
      font.setBold (true);
      font.setItalic (true);
    }
    font.setPointSize (getFontSize() - 10);
    break;

    case _FONT_MED:
    default:
    if (getMode() == SD_MODE ) {
      font.setBold (true);
      font.setItalic (true);
    }
    font.setPointSize (getFontSize());
    break;
  }
  return font;
}

//*****************************************************************************
QColor CSizeManager::getColor (eColor col)
{
	switch (col) {
		case COLOR_DISABLE:
			if (getMode () == HD_MODE)
				return QColor (controlDisableColor_hd);
			else
				return QColor (controlDisableColor_sd);
		case COLOR_NORMAL:
		default:
			if (getMode () == HD_MODE)
				return QColor (controlEnableColor_hd);
			else
				return QColor (controlEnableColor_sd);
	}
}
