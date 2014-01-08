#ifndef CSIZEMANAGER_H
#define CSIZEMANAGER_H

#include <QStyle>
#include <QStyleFactory>
class QString;
class QSize;
class QFont;
class QStyle;

class ProxyStyle : public QStyle
{
    private:
        QStyle *m_style;

    protected:
        const QStyle *style() const
        {
            return m_style;
        }

        virtual void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = 0) const
        {
            m_style->drawComplexControl(control, option, painter, widget);
        }

        virtual void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const
        {
            m_style->drawControl(element, option, painter, widget);
        }

        virtual void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const
        {
            m_style->drawPrimitive(element, option, painter, widget);
        }

        virtual QPixmap generatedIconPixmap (QIcon::Mode iconMode, const QPixmap &pixmap, const QStyleOption *option) const
        {
            return m_style->generatedIconPixmap(iconMode, pixmap, option);
        }

        virtual SubControl hitTestComplexControl(ComplexControl control, const QStyleOptionComplex *option, const QPoint &position, const QWidget *widget = 0) const
        {
            return m_style->hitTestComplexControl(control, option, position, widget);
        }

        virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0, const QWidget * widget = 0) const
        {
            return m_style->pixelMetric(metric, option, widget);
        }

        virtual QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &contentsSize, const QWidget *widget) const
        {
            return m_style->sizeFromContents(type, option, contentsSize, widget);
        }

        virtual int styleHint(StyleHint hint, const QStyleOption *option = 0, const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const
        {
            return m_style->styleHint(hint, option, widget, returnData);
        }

        virtual QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option, SubControl subControl, const QWidget *widget = 0) const
        {
            return m_style->subControlRect(control, option, subControl, widget);
        }

        virtual QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget = 0) const
        {
            return m_style->subElementRect(element, option, widget);
        }

        virtual QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *option = 0, const QWidget *widget = 0) const
        {
            return m_style->standardPixmap(standardPixmap, option, widget);
        }

    public:
        ProxyStyle(QWidget *widget);
 };

class NoFocusStyle : public ProxyStyle {
    protected:
        void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const
        {
            if (element != QStyle::PE_FrameFocusRect)
                style()->drawPrimitive(element, option, painter, widget);
        }
    public:
        NoFocusStyle(QWidget *widget) : ProxyStyle(widget)
        {
        }
};
#define _SZ_HELP_BAR_HEIGHT 100 // высота панели с подсказкой внизу
#define _SZ_ADD_DEL_BTN_BAR_HEIGHT 80

extern const int controlWidth_sd;
extern const int controlWidth_hd;
extern const int controlHeight;
extern const int controlEditX0_hd;
extern const int controlEditX0_sd;
extern const int controlEnableColor_hd;
extern const int controlDisableColor_hd;
extern const int controlEnableColor_sd;
extern const int controlDisableColor_sd;
extern const QFont controlFont_sd;
extern const QFont controlFont_hd;

class CSizeManager
{
	int video_output;
public:
	enum eGuiItemID {
		_ITEM_LABEL,
		_ITEM_LABEL_DISABLE,
		_ITEM_COMBO,
		_ITEM_COMBO_DISABLE,
		_ITEM_COMBO_DROPDOWN,
		_ITEM_EDIT,
		_ITEM_EDIT_DISABLE,
		_ITEM_TREEVIEW,
		_ITEM_VERT_SCROLL,
		_ITEM_VERT_SCROLL_SIMPLE,
		_ITEM_HORIZ_SCROLL,
		_ITEM_IPTV_CHANNEL,
		_ITEM_DIALOG,
		_ITEM_TABLEVIEW,
		_ITEM_TABLEVIEW_NOSELECT,
		_ITEM_LISTVIEW,
		_ITEM_CONTEXTMENU,
	};
	enum eFonts {
		_FONT_BIG,
		_FONT_MED,
		_FONT_SML,
	};
	enum eMode {
		SD_MODE,
		HD_MODE,
	};
	enum eColor {
		COLOR_NORMAL,
		COLOR_DISABLE,
	};
	CSizeManager();
//	unsigned int getColor (EColorID id);												// return color for ID
	QString getStyle (eGuiItemID item);
	int getFontSize ();
	eMode getMode () {return currentMode;}
	QFont getFont (eFonts);
	QColor getColor (eColor);
	void setDisplayMode (eMode mode) {currentMode = mode;}
private:
	eMode currentMode;
};


extern CSizeManager sizeM;

#endif // CSIZEMANAGER_H
