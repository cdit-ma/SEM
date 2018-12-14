#include "theme.h"
#include <QDebug>
#include <QStringBuilder>
#include <QDirIterator>
#include <QDateTime>
#include <QStringBuilder>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent/QtConcurrentMap>
#include <QThreadPool>
#include <QAction>
#include <QApplication>
#include "../modelcontroller/entityfactory.h"
#include <QPainter>

Theme::Theme() : QObject(0)
{
    //Get the original settings from the settings.
    setupAliasIcons();
    setupToggledIcons();
    updateValid();

    //Preload images on a background thread.
    preloadImages();
}

Theme::~Theme()
{
    entity_icons_load_future.waitForFinished();
}

QColor Theme::white()
{
    return QColor(250,250,250);
}

QColor Theme::black()
{
    return QColor(50,50,50);
}

QColor Theme::getBackgroundColor()
{
    return backgroundColor;
}

QString Theme::getBackgroundColorHex()
{
    QColor color = getBackgroundColor();
    return Theme::QColorToHex(color);
}

QColor Theme::getDisabledBackgroundColor()
{
    return disabledBackgroundColor;
}

QString Theme::getDisabledBackgroundColorHex()
{
    QColor color = getDisabledBackgroundColor();
    return Theme::QColorToHex(color);
}

QColor Theme::getAltBackgroundColor()
{
    return altBackgroundColor;
}

QString Theme::getAltBackgroundColorHex()
{
    QColor color = getAltBackgroundColor();
    return Theme::QColorToHex(color);
}

QColor Theme::getHighlightColor()
{
    return highlightColor;
}

QString Theme::getHighlightColorHex()
{
    QColor color = getHighlightColor();
    return Theme::QColorToHex(color);
}

QColor Theme::getPressedColor()
{
    return highlightColor.darker(105);
}

QString Theme::getPressedColorHex()
{
    QColor color = getPressedColor();
    return Theme::QColorToHex(color);
}


QColor Theme::getActiveWidgetBorderColor()
{
    return selectedWidgetBorderColor;
}

QString Theme::getActiveWidgetBorderColorHex()
{
    return Theme::QColorToHex(getActiveWidgetBorderColor());
}

void Theme::setActiveWidgetBorderColor(QColor color)
{
    if(selectedWidgetBorderColor != color){
        selectedWidgetBorderColor = color;
        updateValid();
    }
}

inline uint qHash(const QSize &size, uint seed)
{

    return ::qHash(QPair<int,int>(size.width(), size.height()), seed);
}


void Theme::roundQSize(QSize& size)
{
    int factor = 1;
    //Bitshift round to power of 2
    while((factor <<= 1 ) <= size.width()>>1);

    //Scale the request image size to a width = factor
    size.setWidth(factor);
    size.setHeight(factor);
}

IconPair Theme::getIconPair(const QString& prefix, const QString& alias)
{
    return IconPair(prefix, alias);
}

void Theme::UpdateActionIcon(QAction* action, Theme* theme){
    if(!theme){
        theme = Theme::theme();
    }
    if(action){
        auto icon_path = action->property("icon_path").toString();
        auto icon_alias = action->property("icon_alias").toString();
        action->setIcon(theme->getIcon(icon_path, icon_alias));
    }   
}

void Theme::StoreActionIcon(QAction* action, QString icon_path, QString icon_alias){
    action->setProperty("icon_path", icon_path);
    action->setProperty("icon_alias", icon_alias);
    UpdateActionIcon(action);
}   

void Theme::StoreActionIcon(QAction* action, IconPair icon){
    StoreActionIcon(action, icon.first, icon.second);
}

void Theme::setBackgroundColor(QColor color)
{
    if(backgroundColor != color){
        backgroundColor = color;
        updateValid();
    }
}

void Theme::setInactiveEdgeOpacity(qreal opacity)
{
    if(inactive_opacity_ != opacity){
        inactive_opacity_ = opacity;
        updateValid();
    }
}

void Theme::setDisabledBackgroundColor(QColor color)
{
    if(disabledBackgroundColor != color){
        disabledBackgroundColor = color;
        updateValid();
    }
}

void Theme::setAltBackgroundColor(QColor color)
{
    if(altBackgroundColor != color){
        altBackgroundColor = color;
        updateValid();
    }
}

void Theme::setHighlightColor(QColor color)
{
    if(highlightColor != color){
        highlightColor = color;
        updateValid();
    }
}

QColor Theme::getTextColor(ColorRole role)
{
    if(textColor.contains(role)){
        return textColor[role];
    }
    return QColor();
}

QString Theme::getTextColorHex(ColorRole role)
{
    QColor color = getTextColor(role);
    return Theme::QColorToHex(color);
}

void Theme::setTextColor(ColorRole role, QColor color)
{
    if(textColor[role] != color){
        textColor[role] = color;
        updateValid();
    }
}

void Theme::setSeverityColor(Notification::Severity severity, QColor color){
    if(severityColor[severity] != color){
        severityColor[severity] = color;
        updateValid();
    }
}

QColor Theme::getSeverityColor(Notification::Severity severity){
    return severityColor.value(severity, QColor());
}

QColor Theme::getMenuIconColor(ColorRole role)
{
    if(menuIconColor.contains(role)){
        return menuIconColor[role];
    }
    return QColor();
}

QString Theme::getMenuIconColorHex(ColorRole role)
{
    QColor color = getMenuIconColor(role);
    return Theme::QColorToHex(color);
}

void Theme::setMenuIconColor(ColorRole role, QColor color)
{
     if(menuIconColor[role] != color){
        menuIconColor[role] = color;
        updateValid();
     }
}

void Theme::setAspectBackgroundColor(VIEW_ASPECT aspect, QColor color)
{
    if(aspectColor[aspect] != color){
        aspectColor[aspect] = color;
        updateValid();
    }
}



QColor Theme::getAspectBackgroundColor(VIEW_ASPECT aspect)
{
    if(aspectColor.contains(aspect)){
        return aspectColor[aspect];
    }
    return QColor();
}


void Theme::setWindowIcon(const QString& window_title, const QString& visible_icon_prefix, const QString& visible_icon_alias){
    setIconToggledImage("WindowIcon", window_title, visible_icon_prefix, visible_icon_alias, "Icons", "transparent");
}

void Theme::setIconToggledImage(const QString& prefix, const QString& alias, const QString& toggledOnPrefix, const QString& toggledOnAlias, const QString& toggledOffPrefix, const QString& toggleOffAlias, bool ignore_toggle_coloring){
    QString name = getResourceName(prefix, alias);
    
    //Construct pairs
    auto toggled_on = getIconPair(toggledOnPrefix, toggledOnAlias);
    auto toggled_off = getIconPair(toggledOffPrefix, toggleOffAlias);

    IconToggle icon_toggle;
    icon_toggle.on = getIconPair(toggledOnPrefix, toggledOnAlias);
    icon_toggle.off = getIconPair(toggledOffPrefix, toggleOffAlias);
    icon_toggle.got_toggle = true;

    if(ignore_toggle_coloring){
        icon_toggle.on_normal = icon_toggle.off_normal;
    }

    iconToggledLookup.insert(name, icon_toggle);
}

QColor Theme::getMainImageColor(const QString& prefix, const QString& alias)
{
    const auto& key = getResourceName(prefix, alias);
    return pixmapMainColorLookup.value(key, QColor());
}

QColor Theme::getMainImageColor(IconPair path)
{
    return getMainImageColor(path.first, path.second);
}


void Theme::applyTheme()
{
    themeChanged = false;
    emit theme_Changed();
}

bool Theme::isValid() const
{
    return valid;
}

bool Theme::gotImage(IconPair icon)
{
    return gotImage(icon.first, icon.second);
}

bool Theme::gotImage(const QString& path, const QString& alias)
{
    return gotImage(getResourceName(path, alias));
}

bool Theme::gotImage(const QString& resource_name){
    QReadLocker lock(&lock_);
    return image_names.contains(resource_name);
}


QString Theme::getCornerRadius()
{
    return "4px";
}

QString Theme::getSharpCornerRadius()
{
    return "2px";
}

QIcon Theme::getIcon(IconPair icon)
{
    return getIcon(icon.first, icon.second);
}

QIcon Theme::getIcon(const QString& prefix, const QString& alias)
{
    const auto resource_name = getResourceName(prefix, alias);
    {
        QReadLocker lock(&lock_);
        if(iconLookup.contains(resource_name)){
            return iconLookup.value(resource_name);
        }
    }

    QIcon icon;

    if(prefix.size() && alias.size()){
        IconToggle icon_toggle;

        //Check if we have a toggled Icon
        if(iconToggledLookup.contains(resource_name)){
            icon_toggle = iconToggledLookup.value(resource_name);
        }else{
            icon_toggle.off.first = prefix;
            icon_toggle.off.second = alias;
        }

        auto off_rn = getResourceName(icon_toggle.off);
        auto on_rn = getResourceName(icon_toggle.on);

        bool is_off_tinted = tintIcon(off_rn);
        bool is_on_tinted = tintIcon(on_rn);

        auto icon_size = getLargeIconSize() * 2;

        //Handle Off State
        {
            //Set the default states.
            icon.addPixmap(_getPixmap(off_rn, icon_size, getMenuIconColor(icon_toggle.off_normal)), QIcon::Normal, QIcon::Off);
            
            if(is_off_tinted){
                icon.addPixmap(_getPixmap(off_rn, icon_size, getMenuIconColor(icon_toggle.off_active)), QIcon::Active, QIcon::Off);
                icon.addPixmap(_getPixmap(off_rn, icon_size, getMenuIconColor(icon_toggle.off_disabled)), QIcon::Disabled, QIcon::Off);
            }
        }

            
        //Handle On State
        if(icon_toggle.got_toggle){
            icon.addPixmap(_getPixmap(on_rn, icon_size, getMenuIconColor(icon_toggle.on_normal)), QIcon::Normal, QIcon::On);

            if(is_on_tinted){
                icon.addPixmap(_getPixmap(on_rn, icon_size, getMenuIconColor(icon_toggle.on_active)), QIcon::Active, QIcon::On);
                icon.addPixmap(_getPixmap(on_rn, icon_size, getMenuIconColor(icon_toggle.on_disabled)), QIcon::Disabled, QIcon::On);
            }
        }else{
            icon.addPixmap(_getPixmap(off_rn, icon_size, getMenuIconColor(icon_toggle.on_normal)), QIcon::Normal, QIcon::On);
            icon.addPixmap(_getPixmap(off_rn, icon_size, getMenuIconColor(icon_toggle.on_disabled)), QIcon::Disabled, QIcon::On);
        }

        {
            QWriteLocker lock(&lock_);
            iconLookup.insert(resource_name, icon);
        }
    }
    return icon;
}

QPixmap Theme::_getPixmap(const QString& resourceName, QSize size, QColor tintColor)
{
    //Get the name of the pixmap url
    auto pixmap_url = getPixmapResourceName(resourceName, size, tintColor);
    {
        QReadLocker lock(&lock_);
        if(pixmapLookup.contains(pixmap_url)){
            return pixmapLookup.value(pixmap_url);
        }
    }

    //Get the Image (Load it if it isn't loaded)
    QImage image = getImage(resourceName);
    QPixmap pixmap;

    if(!image.isNull()){
        //Get the size of the original Image
        QSize original_size = image.size();

        //If we have been given a size, which is different to the original size, round it
        if(size.isValid() && size != original_size){
            roundQSize(size);
            //Scale the image
            image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        bool tint_icon = tintIcon(original_size);

        if(tint_icon && !tintColor.isValid()){
            tintColor = getMenuIconColor();
        }

        //Tint the icon if we have to
        if(tint_icon && tintColor.isValid()){
            //Replace the image with it's alpha channel
            image = image.alphaChannel();

            //Replace the colors with the tinted color.
            for(int i = 0; i < image.colorCount(); ++i) {
                tintColor.setAlpha(qGray(image.color(i)));
                image.setColor(i, tintColor.rgba());
            }
        }

        //Construct a Pixmap from the image.
        pixmap = QPixmap::fromImage(image);

        if(current_theme == ThemePreset::XMAS_THEME && pixmap_url.endsWith("XMAS")){
            auto hat_pixmap = _getPixmap(getResourceName("Icons", "santaHat"), pixmap.size());
            QPainter painter(&pixmap);
            painter.drawPixmap(pixmap.rect(), hat_pixmap, hat_pixmap.rect());
        }

        {
            QWriteLocker lock(&lock_);
            if(!pixmapLookup.contains(pixmap_url)){
                pixmapLookup.insert(pixmap_url, pixmap);
            }
        }
    }
    return pixmap;
}

QPixmap Theme::getImage(const QString& prefix, const QString& alias, QSize size, QColor tintColor)
{
    return _getPixmap(getResourceName(prefix, alias), size, tintColor);
}


QString Theme::getPixmapResourceName(QString resource_name, QSize size, QColor tintColor)
{
    QSize original_size = pixmapSizeLookup.value(resource_name);

    if(size.isValid() && size != original_size){
        roundQSize(size);
        resource_name += QString("_%1_%2").arg(size.width()).arg(size.height());
    }

    bool tint_pixmap = tintIcon(original_size);

    if(tint_pixmap && !tintColor.isValid()){
        tintColor = getMenuIconColor();
    }

    if(tint_pixmap && tintColor.isValid()){
        resource_name += QColorToHex(tintColor);
    }

    if(current_theme == ThemePreset::XMAS_THEME){
        //Overlay some christmas shit
        const auto& hat_name = getResourceName("Icons", "santaHat");
        if(resource_name != hat_name && (resource_name.contains("EntityIcon") || resource_name.contains("medea")) && !resource_name.contains("Edge")){
            resource_name += "_XMAS";
        }
    }
    return resource_name;
}

QColor Theme::getAltTextColor()
{
    return altTextColor;
}

QString Theme::getAltTextColorHex()
{
    return QColorToHex(altTextColor);
}

void Theme::setAltTextColor(QColor color)
{
    altTextColor = color;
}

QString Theme::getWindowStyleSheet(bool show_background_image)
{
    QString str;
    str += "QMainWindow {";
    str += "background: " % getBackgroundColorHex() % ";";
    str += "color:" % getTextColorHex() % ";";
    if(show_background_image){
        str += "background-image: url(:/Images/Icons/medeaLogoTransparent); background-position: center; background-repeat: no-repeat;";
    }
    str += "}";
    return str;
}

QString Theme::getScrollBarStyleSheet()
{
    int marginSize = 1;
    int scrollSize = 14;
    int buttonSize = scrollSize - (2* marginSize);

    QString margin = QString::number(marginSize) % "px";
    QString size = QString::number(scrollSize) % "px";
    QString button = QString::number(buttonSize) % "px";

    return "QScrollBar {"
           "background: " % getDisabledBackgroundColorHex() % ";"
           "border: 0px;"
           "margin: 0px;"
           "}"
           "QScrollBar:vertical{width:" % size % "; padding-left: 1px;}"
           "QScrollBar:horizontal{height:" % size % "; padding-top: 1px;}"
           "QScrollBar::handle{background: " % getAltBackgroundColorHex() % " ;}"
           "QScrollBar::handle:active{background: " % getHighlightColorHex() % ";}"
           "QScrollBar::add-line, QScrollBar::sub-line{background:none;}" // Up/Down Button holders;
           "QScrollBar::add-page, QScrollBar::sub-page{background:none;}" // Space between Handle and Up/Down Buttons
           "QScrollBar::handle{margin:" % margin % ";}" // Allow nice space.
            ;
}

qreal Theme::getInactiveEdgeOpacity(){
    return inactive_opacity_;   
}

QString Theme::getSliderStyleSheet()
{
    return "QSlider {"
           "margin: 0px;"
           "}"
           "QSlider::groove:horizontal {"
           "background: " % getBackgroundColorHex() % ";"
           "border: 1px solid " % getAltBackgroundColorHex() % ";"
           "}"
           "QSlider::handle{background: " % getAltBackgroundColorHex() % ";"
           "width:10px;"
           "}"
           "QSlider::handle:active{background: " % getHighlightColorHex() % ";}"
           "QSlider::sub-page {background: " % getDisabledBackgroundColorHex() % ";border: 1px solid " % getAltBackgroundColorHex() % ";}"
           ;
}

QString Theme::getDialogStyleSheet()
{
    return "QDialog {"
           "background: " % getBackgroundColorHex() % ";"
           "color: " % getTextColorHex() % ";"
           "}";
}

QString Theme::getSplitterStyleSheet()
{
    return  "QSplitter {background:" % getBackgroundColorHex() % ";}"
            "QSplitter::handle{width:16px;height:16px;}"
            "QSplitter::handle:pressed {background:" % getAltBackgroundColorHex() % ";}"
            "QSplitter::handle:horizontal {image: url(:/Images/Icons/dotsVertical);}"
            "QSplitter::handle:vertical {image: url(:/Images/Icons/dotsHorizontal);}"
            ;
}
QSize Theme::getIconSize(){
    return icon_size;
}
QSize Theme::getLargeIconSize(){
    return icon_size * 2;
}
void Theme::setIconSize(int size){

    QSize new_size(size, size);

    roundQSize(new_size);
    if(new_size != icon_size){
        icon_size = new_size;
        updateValid();
    }

}

QString Theme::getWidgetStyleSheet(QString widgetName)
{
    return widgetName % " {"
           "background:" % getBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "border: 0px;"
           "}";
}

QMovie* Theme::getGif(const QString& path, const QString& name){
    auto lookupName = getResourceName(path, name);
    
    if(!gifLookup.contains(lookupName)){
        auto movie = new QMovie();
        movie->setCacheMode(QMovie::CacheAll);
        movie->setFileName(":/" % lookupName);
        movie->setScaledSize(QSize(16,16));
        movie->start();
        gifLookup[lookupName] = movie;
    }
    return gifLookup[lookupName];
}

QString Theme::getTabbedWidgetStyleSheet()
{
    return "QTabBar::tab{ background:" % getAltBackgroundColorHex() % "; color: " % getTextColorHex() % "; border-radius:" % getCornerRadius() % "; border: 1px solid " % getBackgroundColorHex() % ";}"
           "QTabBar::tab:top{ border-bottom-left-radius: 0px; border-bottom-right-radius: 0px; margin: 1px 0px; padding: 5px 10px; }"
           "QTabBar::tab:right{ border-top-right-radius: 0px; border-bottom-right-radius: 0px; margin: 0px 1px; padding: 10px 5px; }"
           "QTabBar::tab:selected{ background:" % getActiveWidgetBorderColorHex() % "; border-color:" % getDisabledBackgroundColorHex() % ";}"
           "QTabBar::tab:hover{ background:" % getHighlightColorHex() % "; color: " % getTextColorHex(ColorRole::SELECTED) % ";}"
           "QTabBar, QTabWidget::tab-bar{ alignment: center; qproperty-drawBase: 0; border: 0px; }"
           "QTabWidget::pane, QTabBar::pane{ background:" % getBackgroundColorHex() % ";}"
           ;
}

QString Theme::getNodeViewStyleSheet(bool isActive)
{
    QString activeBorder = "border: 2px solid " % getActiveWidgetBorderColorHex() % ";";
    QString inActiveBorder = "border: 2px solid " % getDisabledBackgroundColorHex() % ";";
    return "QGraphicsView {"
           "background:" % getBackgroundColorHex() % ";"
           % (isActive ? activeBorder : inActiveBorder) %
           "}";
}

QString Theme::getDockWidgetStyleSheet()
{
    return "QDockWidget {"
           "border: 0px;"
           "margin: 3px;"
           "background:" % getBackgroundColorHex() % ";"
           "}";
}

QString Theme::getToolDockWidgetTitleBarStyleSheet()
{
    return  getDockTitleBarStyleSheet(false) %
            "DockTitleBar QToolButton { padding: 0px; border-radius: " % getCornerRadius() % /*2px*/ "; }";
}

QString Theme::getViewDockWidgetStyleSheet(bool isActive)
{
    return  getDockWidgetStyleSheet() %
            getNodeViewStyleSheet(isActive);
}

QString Theme::getDockTitleBarStyleSheet(bool isActive, QString widgetName)
{
    QString bgColor = isActive ? getActiveWidgetBorderColorHex() : getAltBackgroundColorHex();
    QString borderColor = isActive ? getActiveWidgetBorderColorHex() : getDisabledBackgroundColorHex();
    return  widgetName % "{"
            "padding: 0px 2px;"
            "spacing: 1px;"
            "background:" % bgColor % ";"
            "border: 1px solid " % borderColor % ";"
            "}"
            % widgetName % " QLabel {"
            "color:" % getTextColorHex() % ";"
            "}"
            % widgetName % " QToolButton {"
            "border: 0px;"
            "padding: 2px;"
            "margin: 0px;"
            "}"
            % widgetName % " QToolButton::hover {"
            "background:" % getHighlightColorHex() % ";"
            "}"
            % widgetName % " QToolButton::!hover {"
            "background:" % bgColor % ";"
            "}"
            % widgetName % " QToolButton#WINDOW_ICON::hover{"
            "background:none;"
            "}";
            
}

QString Theme::getMenuBarStyleSheet()
{
    return "QMenuBar {"
           "padding: 2px;"
           "spacing: 2px;"
           "background:" % getBackgroundColorHex() % ";"
           "border-bottom: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "}"
           "QMenuBar::item {"
           "background:" % getBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "padding: 5px;"
           "}"
           "QMenuBar::item:selected {"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "background:" % getHighlightColorHex() % ";"
           "border-radius:" % getSharpCornerRadius() % ";"
           //"border-radius:" % getCornerRadius() % ";"
           "}";
}

QString Theme::getMenuStyleSheet(int icon_size_int)
{
    QString icon_size = QString::number(icon_size_int);
    return "QMenu {"
           "background:" % getAltBackgroundColorHex() % ";"
           "border-radius:" % getSharpCornerRadius() % ";"
           "margin: 2px;"
           "spacing: 2px;"
           "}"
           "QMenu::item {"
           "margin:2px;"
           "padding: 4px 8px 4px " % QString::number(icon_size_int + 8) % "px;"
           "background:" % getAltBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "border-radius: " % getSharpCornerRadius() % ";"
           "border: none;"
           "}"
           "QMenu::item:disabled {"
           "color:" % getAltTextColorHex() % ";"
           "}"
           "QMenu::item:selected:!disabled {"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "background: " % getHighlightColorHex() % ";"
           "border: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "}"
           "QMenu::icon {"
           /*"position: relative;"
           "top: 2px;"
           "right: 2px;"
           "bottom: 2px;"*/
           "left: 2px;"
           "}"
           "QMenu::right-arrow{"
               "width:4px;"
               "height:4px;"
               "margin:4px;"
               "border-radius:2px;"
               "background:" % getTextColorHex() % ";"
           "}"
           "QMenu::right-arrow:selected{"
               "background:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"
           "QMenu::separator {"
            "height: 1px;"
            "background: " % getTextColorHex(ColorRole::DISABLED) % ";"
            "margin:4px;"
            "margin-left: 8px;"
            "margin-right: 8px;"
            "}"
       ;
}

QString Theme::getToolBarStyleSheet()
{
    return "QToolBar {"
           "spacing: 3px;"
           "border: 0px;"
           "padding: 1px;"
           "margin: 0px;"
           //"background:" % getBackgroundColorHex() % ";"
           "background: rgba(0,0,0,0);"
           "}"
           "QToolBar::separator {"
           "width: 5px;"
           "background: rgba(0,0,0,0);"
           "}"

           "QToolButton {"
           "padding: 2px;"
           "border-radius: " % getCornerRadius() % ";"
           "border: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "background:" % getAltBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "}"
           "QToolButton::checked {"
           "background:" % getPressedColorHex() % ";"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"
           "QToolButton:hover {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"
           "QToolButton:pressed {"
           "background:" % getPressedColorHex() % ";"
           "}"
           "QToolButton:disabled {"
           "background:" % getDisabledBackgroundColorHex() % ";"
           "}"
           "QToolButton[popupMode=\"1\"] {"
           "padding-right: 15px;"
           "color:" % getTextColorHex() % ";"
           "}"
           "QToolButton[popupMode=\"1\"]:hover {"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"
           "QToolButton[popupMode=\"1\"]:pressed {"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"
           "QToolButton[popupMode=\"2\"] {"
           "padding-right: 15px;"
           "color:" % getTextColorHex() % ";"
           "}"
           "QToolButton[popupMode=\"2\"]:hover {"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"
           "QToolButton[popupMode=\"2\"]:pressed {"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"
           "QToolButton::menu-button {"
           "border-left: 1px solid rgb(150,150,150);"
           "border-top-right-radius: 10px;"
           "border-bottom-right-radius: 10px;"
           "width: 15px;"
           "}";
}

QString Theme::getAbstractItemViewStyleSheet()
{
    return "QAbstractItemView {"
           "background:" % getBackgroundColorHex() % ";"
           "color: "% getTextColorHex() % ";"
           "border: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "}"

           "QAbstractItemView::item {"
           "padding: 5px 0px;"
           "}"
           "QAbstractItemView::item::disabled {"
           "background:" % getBackgroundColorHex() % ";"
           "color:" % getAltTextColorHex() % ";"
           "}"
           "QAbstractItemView::item::selected {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"
           "QAbstractItemView::item::hover {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"

           "QHeaderView {"
           "background:" % getBackgroundColorHex() %";"
           "border: 0px;"
           "color:" % getTextColorHex() % ";"
           "}"
           "QHeaderView::section {"
           "background:" % getAltBackgroundColorHex() % ";"
           "border: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "border-width: 0px 0px 1px 0px;"
           "padding: 0px 0px 0px 3px;"
           "}"
           "QHeaderView::section:hover {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"
           "QHeaderView::section:selected {"
           "font-weight: normal;"
           "}"
            "QAbstractItemView QWidget {"
            "color:" % getTextColorHex() % ";"
            "border: none;"
            "background:" % getBackgroundColorHex() % ";"
            "selection-color:" % getTextColorHex(ColorRole::SELECTED) % ";"
            "selection-background-color:" % getHighlightColorHex() % ";"
            "}";
}


QString Theme::getGroupBoxStyleSheet()
{
    return
    "QGroupBox {border:0;padding:0px;margin:0px;background:transparent; color: " % getTextColorHex() % ";}"
    "QGroupBox {border: 1px solid " % getAltBackgroundColorHex() % ";border-radius: " % getCornerRadius() %"; padding:10px;margin-top:8px;}"
    "QGroupBox::title {subcontrol-position: top center;subcontrol-origin: margin; padding: 2px; font-weight:bold;}";
}

QString Theme::getPushButtonStyleSheet()
{
    return "QPushButton {"
           "background:" % getAltBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "border-radius: " % getCornerRadius() % ";"
           "border: 1px solid " % getDisabledBackgroundColorHex() % ";"
           //"padding: 2px;"
           "padding: 3px;"
           "}"
           "QPushButton:hover {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"
            /*
           "QPushButton::checked {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(ColorRole::SELECTED) % ";"
           "}"
           */
           "QPushButton:disabled {"
           "background:" % getDisabledBackgroundColorHex() % ";"
           "}";
}


QString Theme::getLineEditStyleSheet(QString widget_name)
{
    return widget_name + " {"
           "margin: 0px;"
           "padding: 0px;"
           "background:" % getAltBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "border: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "border-radius: 0px;"
           "selection-background-color:" % getHighlightColorHex() % ";"
           "selection-color: " % getTextColorHex(ColorRole::SELECTED) % ";"
           "}" + 
           widget_name +":focus {"
           "border: 1px solid "% getHighlightColorHex() % ";"
           "}";
}

QString Theme::getProgressBarStyleSheet()
{
    return "QProgressBar {"
           "background:" % getAltBackgroundColorHex() % ";"
           "border-radius: 0px;"
           "border: none;"
           "text-align: center;"
           "color:" % getTextColorHex() % ";"
           "}"
           "QProgressBar::chunk {"
           "margin: 2px;"
           "border: none;"
           "background: " % getHighlightColorHex() % ";"
           "}";
}

QString Theme::getLabelStyleSheet()
{
    return "QLabel{ background: rgba(0,0,0,0); color:" % getTextColorHex() % ";}";
}


void Theme::preloadImages()
{   
    auto time_start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    
    //Recurse through all files in the image alias.
    QDirIterator it(":/Images", QDirIterator::Subdirectories);

    QList<QString> preload_Icons;
    QList<QString> preload_EntityIcons;

    while(it.hasNext()){
        it.next();
        if(it.fileInfo().isFile()){
            const auto& file_path = it.filePath().mid(2);
            image_names.insert(file_path);
            
            if(file_path.startsWith("Images/EntityIcons/")){
                preload_EntityIcons << file_path;
            }else{
                preload_Icons << file_path;
            }
        }
    }

    entity_icons_load_future = QtConcurrent::run([=](){
        auto time_start = QDateTime::currentDateTime().toMSecsSinceEpoch();
        auto results = QtConcurrent::blockingMapped(preload_EntityIcons, &Theme::LoadImage);
        
        for(const auto& i : results){
            QWriteLocker lock(&lock_);
            icon_prefix_lookup[i.image_path.first].insert(i.image_path.second);
            imageLookup[i.resource_name] = i.image;
            pixmapSizeLookup[i.resource_name] = i.image.size();
            pixmapMainColorLookup[i.resource_name] = i.color;
        }
        auto time_finish = QDateTime::currentDateTime().toMSecsSinceEpoch();
        qCritical() << "Preloaded EntityIcons #" << results.count() << " Images in: " <<  time_finish - time_start << "MS";
    });

    auto results = QtConcurrent::blockingMapped(preload_Icons, &Theme::LoadImage);
    for(const auto& i : results){
        icon_prefix_lookup[i.image_path.first].insert(i.image_path.second);
        imageLookup[i.resource_name] = i.image;
        pixmapSizeLookup[i.resource_name] = i.image.size();
        pixmapMainColorLookup[i.resource_name] = i.color;
    }

    auto time_finish = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qCritical() << "Preloaded #" << preload_Icons.count() << " Images in: " <<  time_finish - time_start << "MS";
    emit theme_Changed();
}

void Theme::settingChanged(SETTINGS setting, QVariant value)
{
    QColor color = value.value<QColor>();

    switch(setting){
    case SETTINGS::THEME_BG_COLOR:{
        setBackgroundColor(color);
        break;
    }
    case SETTINGS::THEME_INACTIVE_EDGE_OPACITY:{
        setInactiveEdgeOpacity(value.toInt() / 100.0);
        break;
    }
    case SETTINGS::THEME_BG_ALT_COLOR:{
        setAltBackgroundColor(color);
        break;
    }
    case SETTINGS::THEME_TEXT_COLOR:{
        setTextColor(ColorRole::NORMAL, color);
        break;
    }
    case SETTINGS::THEME_ALTERNATE_TEXT_COLOR:{
        setAltTextColor(color);
        break;
    }
    case SETTINGS::THEME_ICON_COLOR:{
        setMenuIconColor(ColorRole::NORMAL, color);
        break;
    }
    case SETTINGS::THEME_BG_DISABLED_COLOR:{
        setDisabledBackgroundColor(color);
        break;
    }
    case SETTINGS::THEME_TEXT_DISABLED_COLOR:{
        setTextColor(ColorRole::DISABLED, color);
        break;
    }
    case SETTINGS::THEME_ICON_DISABLED_COLOR:{
        setMenuIconColor(ColorRole::DISABLED, color);
        break;
    }
    case SETTINGS::THEME_BG_SELECTED_COLOR:{
        setHighlightColor(color);
        break;
    }
    case SETTINGS::THEME_TEXT_SELECTED_COLOR:{
        setTextColor(ColorRole::SELECTED, color);
        break;
    }
    case SETTINGS::THEME_ICON_SELECTED_COLOR:{
        setMenuIconColor(ColorRole::SELECTED, color);
        break;
    }
    case SETTINGS::THEME_VIEW_BORDER_SELECTED_COLOR:{
        setActiveWidgetBorderColor(color);
        break;
    }
    case SETTINGS::THEME_ASPECT_BG_INTERFACES_COLOR:{
        setAspectBackgroundColor(VIEW_ASPECT::INTERFACES, color);
        break;
    }
    case SETTINGS::THEME_ASPECT_BG_BEHAVIOUR_COLOR:{
        setAspectBackgroundColor(VIEW_ASPECT::BEHAVIOUR, color);
        break;
    }
    case SETTINGS::THEME_ASPECT_BG_ASSEMBLIES_COLOR:{
        setAspectBackgroundColor(VIEW_ASPECT::ASSEMBLIES, color);
        break;
    }
    case SETTINGS::THEME_ASPECT_BG_HARDWARE_COLOR:{
        setAspectBackgroundColor(VIEW_ASPECT::HARDWARE, color);
        break;
    }
    
    case SETTINGS::THEME_SETTHEME_XMASTHEME:{
        resetTheme(ThemePreset::XMAS_THEME);
        break;
    }
    case SETTINGS::THEME_SETTHEME_DARKTHEME:{
        resetTheme(ThemePreset::DARK_THEME);
        break;
    }
    case SETTINGS::THEME_SETTHEME_LIGHTHEME:{
        resetTheme(ThemePreset::LIGHT_THEME);
        break;
    }
    case SETTINGS::THEME_SETTHEME_SOLARIZEDDARKTHEME:{
        resetTheme(ThemePreset::SOLARIZED_DARK_THEME);
        break;
    }
    case SETTINGS::THEME_SETTHEME_SOLARIZEDLIGHTTHEME:{
        resetTheme(ThemePreset::SOLARIZED_LIGHT_THEME);
        break;
    }
    case SETTINGS::THEME_SETASPECT_CLASSIC:{
        resetAspectTheme(false);
        break;
    }
    case SETTINGS::THEME_SIZE_FONTSIZE:{
        setFont(value.value<QFont>());
        break;
    }
    case SETTINGS::THEME_SIZE_ICONSIZE:{
        setIconSize(value.toInt());
        break;
    }
    case SETTINGS::THEME_SEVERITY_RUNNING_COLOR:{
        setSeverityColor(Notification::Severity::RUNNING, color);
        break;
    }

    case SETTINGS::THEME_SEVERITY_INFO_COLOR:{
        setSeverityColor(Notification::Severity::INFO, color);
        break;
    }
    case SETTINGS::THEME_SEVERITY_WARNING_COLOR:{
        setSeverityColor(Notification::Severity::WARNING, color);
        break;
    }
    case SETTINGS::THEME_SEVERITY_ERROR_COLOR:{
        setSeverityColor(Notification::Severity::ERROR, color);
        break;
    }
    case SETTINGS::THEME_SEVERITY_SUCCESS_COLOR:{
        setSeverityColor(Notification::Severity::SUCCESS, color);
        break;
    }
    case SETTINGS::THEME_SETASPECT_COLORBLIND:{
        resetAspectTheme(true);
        break;
    }case SETTINGS::THEME_APPLY:{
        applyTheme();
        break;
    }
    default:
        break;
    }
}


QColor Theme::CalculateImageColor(const QImage& image)
{
    QColor c;
    if(!image.isNull()){
        int size = 32;
        auto scaled_image = image.scaled(size, size);

        QHash<QRgb, int> color_count;
        int max = 0;
        QRgb frequent_color = QColor(Qt::black).rgb();

        int f = 25;

        for(int i = 0; i < size * size; i++){
            int x = i % size;
            int y = i / size;
            QRgb pixel = scaled_image.pixel(x,y);

            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);

            //Ignore black
            if(r < f && g < f && b < f){
                continue;
            }

            if(color_count.contains(pixel)){
                color_count[pixel] ++;
            }else{
                color_count[pixel] = 1;
            }

            int count = color_count[pixel];
            if(count > max){
                frequent_color = pixel;
                max = count;
            }
        }
        c = QColor(frequent_color);
    }
    return c;
}

QString resourceName(QString prefix, QString alias){
    return "Images/" % prefix % '/' % alias;
}

QString Theme::getResourceName(QString prefix, QString alias) const
{
    //Uncomment for bounding rects
    //return "Images/Icons/TwoTone";
    auto resource_name = resourceName(prefix, alias);
    while(icon_alias_lookup.contains(resource_name)){
        resource_name = icon_alias_lookup[resource_name];
    }
    return resource_name;
}

QString Theme::getResourceName(IconPair icon) const
{
    return getResourceName(icon.first, icon.second);
}

void Theme::setupToggledIcons()
{
    setIconToggledImage("ToggleIcons", "arrowVertical", "Icons", "arrowHeadUp", "Icons", "arrowHeadDown");
    setIconToggledImage("ToggleIcons", "groupToggle", "Icons", "arrowHeadDown", "Icons", "arrowHeadRight");
    setIconToggledImage("ToggleIcons", "sort", "Icons", "arrowDown", "Icons", "arrowUp");
    setIconToggledImage("ToggleIcons", "slider", "Icons", "toggleOn", "Icons", "toggleOff");


    setIconToggledImage("ToggleIcons", "maximize", "Icons", "minimize", "Icons", "maximize");
    setIconToggledImage("ToggleIcons", "lock", "Icons", "lockOpened", "Icons", "lockClosed");
    setIconToggledImage("ToggleIcons", "visible", "Icons", "eye", "Icons", "transparent");
    setIconToggledImage("ToggleIcons", "newNotification", "Icons", "bell", "Icons", "clock", false);
}

void Theme::setupAliasIcons(){
    setIconAlias("EntityIcons", "WorkerDefinitions", "EntityIcons", "Workload");
    setIconAlias("Data", "label", "Icons", "label");
    setIconAlias("Data", "description", "Icons", "speechBubbleFilled");
    setIconAlias("Data", "kind", "Icons", "tiles");
    setIconAlias("Data", "namespace", "Icons", "letterA");
    setIconAlias("Data", "value", "Icons", "pencil");
    setIconAlias("Data", "type", "Icons", "gearDark");
    setIconAlias("Data", "ID", "Icons", "numberOne");

    setIconAlias("Data", "SRC ID", "Icons", "planeLaunch");
    setIconAlias("Data", "DST ID", "Icons", "planeLand");
}   

void Theme::setIconAlias(const QString& prefix, const QString& alias, const QString& icon_prefix, const QString& icon_alias){
    auto alias_rn = resourceName(prefix, alias);
    auto icon_rn = resourceName(icon_prefix, icon_alias);
    
    if(!icon_alias_lookup.contains(alias_rn)){
        icon_alias_lookup[alias_rn] = icon_rn;
    }
}


void Theme::resetTheme(ThemePreset themePreset){
    current_theme = themePreset;
    //Solarized
    QColor base03("#002b36");
    QColor base02("#073642");
    QColor base01("#586e75");
    QColor base00("#657b83");
    QColor base0("#839496");
    QColor base1("#93a1a1");
    QColor base2("#eee8d5");
    QColor base3("#fdf6e3");
    QColor yellow("#b58900");
    QColor orange("#cb4b16");
    QColor red("#dc322f");
    QColor magenta("#d33682");
    QColor violet("#6c71c4");
    QColor blue("#268bd2");
    QColor cyan("#2aa198");
    QColor green("#859900");

    
    iconLookup.clear();

    emit changeSetting(SETTINGS::THEME_INACTIVE_EDGE_OPACITY, 50);

    switch(themePreset){
        case ThemePreset::XMAS_THEME:
        case ThemePreset::DARK_THEME:{
            QColor bgColor = QColor(70,70,70);
            emit changeSetting(SETTINGS::THEME_BG_COLOR, bgColor);
            emit changeSetting(SETTINGS::THEME_BG_ALT_COLOR, bgColor.lighter());
            emit changeSetting(SETTINGS::THEME_TEXT_COLOR, white());
            emit changeSetting(SETTINGS::THEME_ICON_COLOR, white());
            emit changeSetting(SETTINGS::THEME_BG_DISABLED_COLOR, bgColor.lighter(120));
            emit changeSetting(SETTINGS::THEME_TEXT_DISABLED_COLOR, QColor(130,130,130));
            emit changeSetting(SETTINGS::THEME_ICON_DISABLED_COLOR, QColor(130,130,130));
            emit changeSetting(SETTINGS::THEME_BG_SELECTED_COLOR, QColor(255,165,70));
            emit changeSetting(SETTINGS::THEME_TEXT_SELECTED_COLOR, black());
            emit changeSetting(SETTINGS::THEME_ICON_SELECTED_COLOR, black());
            emit changeSetting(SETTINGS::THEME_VIEW_BORDER_SELECTED_COLOR, black());
            emit changeSetting(SETTINGS::THEME_ALTERNATE_TEXT_COLOR, white().darker(150));


            emit changeSetting(SETTINGS::THEME_SEVERITY_RUNNING_COLOR, white());
            emit changeSetting(SETTINGS::THEME_SEVERITY_INFO_COLOR, white());
            emit changeSetting(SETTINGS::THEME_SEVERITY_WARNING_COLOR, QColor(255,255,10));
            emit changeSetting(SETTINGS::THEME_SEVERITY_ERROR_COLOR, QColor(255,50,50));
            emit changeSetting(SETTINGS::THEME_SEVERITY_SUCCESS_COLOR, QColor(50,205,50));
            resetAspectTheme(true);
            break;
        }
        case ThemePreset::LIGHT_THEME:{
            QColor bgColor = QColor(170,170,170);
            emit changeSetting(SETTINGS::THEME_BG_COLOR, bgColor);
            emit changeSetting(SETTINGS::THEME_BG_ALT_COLOR, bgColor.lighter(130));
            emit changeSetting(SETTINGS::THEME_TEXT_COLOR, black());
            emit changeSetting(SETTINGS::THEME_ICON_COLOR, black());
            emit changeSetting(SETTINGS::THEME_BG_DISABLED_COLOR, bgColor.lighter(110));
            emit changeSetting(SETTINGS::THEME_TEXT_DISABLED_COLOR, QColor(130,130,130));
            emit changeSetting(SETTINGS::THEME_ICON_DISABLED_COLOR, QColor(130,130,130));
            emit changeSetting(SETTINGS::THEME_BG_SELECTED_COLOR, QColor(75,110,175));
            emit changeSetting(SETTINGS::THEME_TEXT_SELECTED_COLOR, white());
            emit changeSetting(SETTINGS::THEME_ICON_SELECTED_COLOR, white());
            emit changeSetting(SETTINGS::THEME_VIEW_BORDER_SELECTED_COLOR, white());
            emit changeSetting(SETTINGS::THEME_ALTERNATE_TEXT_COLOR, black().lighter(180));

            emit changeSetting(SETTINGS::THEME_SEVERITY_RUNNING_COLOR, black());
            emit changeSetting(SETTINGS::THEME_SEVERITY_INFO_COLOR, black());
            emit changeSetting(SETTINGS::THEME_SEVERITY_WARNING_COLOR, QColor(255,200,0));
            emit changeSetting(SETTINGS::THEME_SEVERITY_ERROR_COLOR, QColor(255,50,50));
            emit changeSetting(SETTINGS::THEME_SEVERITY_SUCCESS_COLOR, QColor(0,128,0));
            resetAspectTheme(true);
            break;
        }
        case ThemePreset::SOLARIZED_DARK_THEME:{
            emit changeSetting(SETTINGS::THEME_BG_COLOR, base02);
            emit changeSetting(SETTINGS::THEME_BG_ALT_COLOR, base03);
            emit changeSetting(SETTINGS::THEME_TEXT_COLOR, base2);
            emit changeSetting(SETTINGS::THEME_ALTERNATE_TEXT_COLOR, base01);
            emit changeSetting(SETTINGS::THEME_ICON_COLOR, base2);

            emit changeSetting(SETTINGS::THEME_BG_DISABLED_COLOR, base02);
            emit changeSetting(SETTINGS::THEME_TEXT_DISABLED_COLOR, base01);
            emit changeSetting(SETTINGS::THEME_ICON_DISABLED_COLOR, base01);

            emit changeSetting(SETTINGS::THEME_BG_SELECTED_COLOR, base2);
            emit changeSetting(SETTINGS::THEME_TEXT_SELECTED_COLOR, base02);
            emit changeSetting(SETTINGS::THEME_ICON_SELECTED_COLOR, base02);
            emit changeSetting(SETTINGS::THEME_VIEW_BORDER_SELECTED_COLOR, base03);

            emit changeSetting(SETTINGS::THEME_SEVERITY_INFO_COLOR, base2);
            emit changeSetting(SETTINGS::THEME_SEVERITY_WARNING_COLOR, orange);
            emit changeSetting(SETTINGS::THEME_SEVERITY_ERROR_COLOR, red);
            emit changeSetting(SETTINGS::THEME_SEVERITY_SUCCESS_COLOR, green);

            emit changeSetting(SETTINGS::THEME_ASPECT_BG_INTERFACES_COLOR, base03);
            emit changeSetting(SETTINGS::THEME_ASPECT_BG_BEHAVIOUR_COLOR, base03);
            emit changeSetting(SETTINGS::THEME_ASPECT_BG_ASSEMBLIES_COLOR, base03);
            emit changeSetting(SETTINGS::THEME_ASPECT_BG_HARDWARE_COLOR, base03);
            break;
        }
        case ThemePreset::SOLARIZED_LIGHT_THEME:{
            emit changeSetting(SETTINGS::THEME_BG_COLOR, base3);
            emit changeSetting(SETTINGS::THEME_BG_ALT_COLOR, base2);
            emit changeSetting(SETTINGS::THEME_TEXT_COLOR, base00);
            emit changeSetting(SETTINGS::THEME_ALTERNATE_TEXT_COLOR, base01);
            emit changeSetting(SETTINGS::THEME_ICON_COLOR, base00);

            emit changeSetting(SETTINGS::THEME_BG_DISABLED_COLOR, base02);
            emit changeSetting(SETTINGS::THEME_TEXT_DISABLED_COLOR, base01);
            emit changeSetting(SETTINGS::THEME_ICON_DISABLED_COLOR, base01);

            emit changeSetting(SETTINGS::THEME_BG_SELECTED_COLOR, base2);
            emit changeSetting(SETTINGS::THEME_TEXT_SELECTED_COLOR, base02);
            emit changeSetting(SETTINGS::THEME_ICON_SELECTED_COLOR, base02);
            emit changeSetting(SETTINGS::THEME_VIEW_BORDER_SELECTED_COLOR, base02);

            emit changeSetting(SETTINGS::THEME_SEVERITY_INFO_COLOR, base2);
            emit changeSetting(SETTINGS::THEME_SEVERITY_WARNING_COLOR, orange);
            emit changeSetting(SETTINGS::THEME_SEVERITY_ERROR_COLOR, red);
            emit changeSetting(SETTINGS::THEME_SEVERITY_SUCCESS_COLOR, green);

            emit changeSetting(SETTINGS::THEME_ASPECT_BG_INTERFACES_COLOR, base03);
            emit changeSetting(SETTINGS::THEME_ASPECT_BG_BEHAVIOUR_COLOR, base03);
            emit changeSetting(SETTINGS::THEME_ASPECT_BG_ASSEMBLIES_COLOR, base03);
            emit changeSetting(SETTINGS::THEME_ASPECT_BG_HARDWARE_COLOR, base03);
            break;
        }
        default:
            break;
    }
}


QFont Theme::getFont() const{
    return font;
}

QFont Theme::getLargeFont() const{
    auto large_font = font;
    large_font.setPointSizeF(font.pointSizeF() * 1.2);
    return large_font;
}

void Theme::setFont(QFont font){
    if(this->font != font){
        this->font = font;
        updateValid();
    }
    
    if(QApplication::font() != font){
        QApplication::setFont(font);
    }
}

void Theme::resetAspectTheme(bool colorBlind)
{
    if(colorBlind){
        emit changeSetting(SETTINGS::THEME_ASPECT_BG_INTERFACES_COLOR, QColor(24,148,184));
        emit changeSetting(SETTINGS::THEME_ASPECT_BG_BEHAVIOUR_COLOR, QColor(90,90,90));
        emit changeSetting(SETTINGS::THEME_ASPECT_BG_ASSEMBLIES_COLOR, QColor(175,175,175));
        emit changeSetting(SETTINGS::THEME_ASPECT_BG_HARDWARE_COLOR, QColor(207,107,100));
    }else{
        //LEGACY
        emit changeSetting(SETTINGS::THEME_ASPECT_BG_INTERFACES_COLOR,  QColor(110,210,210));
        emit changeSetting(SETTINGS::THEME_ASPECT_BG_BEHAVIOUR_COLOR, QColor(254,184,126));
        emit changeSetting(SETTINGS::THEME_ASPECT_BG_ASSEMBLIES_COLOR, QColor(255,160,160));
        emit changeSetting(SETTINGS::THEME_ASPECT_BG_HARDWARE_COLOR, QColor(110,170,220));
    }
}

void Theme::updateValid()
{
    const static QList<ColorRole> states({ColorRole::NORMAL,ColorRole::DISABLED,ColorRole::SELECTED});
    themeChanged = true;

    if(!highlightColor.isValid()){
        valid = false;
        return;
    }

    if(!backgroundColor.isValid()){
        valid = false;
        return;
    }

    if(!altBackgroundColor.isValid()){
        valid = false;
        return;
    }

    if(!disabledBackgroundColor.isValid()){
        valid = false;
        return;
    }
        
    for(const auto& state : states){
        auto text_color = textColor.value(state, QColor());
        auto icon_color = menuIconColor.value(state, QColor());

        if(!text_color.isValid() || !icon_color.isValid()){
            valid = false;
            return;
        }
    }

    for(const auto& aspect : EntityFactory::getViewAspects()){
        auto aspect_color = aspectColor.value(aspect, QColor());

        if(!aspect_color.isValid()){
            valid = false;
            return;
        }
    }

    for(const auto&  severity : Notification::getSeverities()){
        auto severity_color = severityColor.value(severity, QColor());

        if(!severity_color.isValid()){
            valid = false;
            return;
        }
    }
    valid = true;
}

Theme::ImageLoad Theme::LoadImage(const QString& resource_name){
    ImageLoad i;
    i.resource_name = resource_name;
    i.image = QImage(":/" % resource_name);
    if(!i.image.isNull()){
        i.color = CalculateImageColor(i.image);
        i.image_path = SplitImagePath(resource_name);
    }else{
        qCritical() << "Theme Loading Null Image: " << resource_name;
    }
    return i;
}

QImage Theme::getImage(const QString& resource_name)
{
    {
        //Try load the image
        QReadLocker lock(&lock_);
        if(imageLookup.contains(resource_name)){
            return imageLookup[resource_name];
        }
    }

    if(!gotImage(resource_name)){
        return QImage();
    }

    auto i = LoadImage(resource_name);
    if(!i.image.isNull()){
        QWriteLocker lock(&lock_);
        icon_prefix_lookup[i.image_path.first].insert(i.image_path.second);
        imageLookup[i.resource_name] = i.image;
        pixmapSizeLookup[i.resource_name] = i.image.size();
        pixmapMainColorLookup[i.resource_name] = i.color;
    }

    return i.image;
}


IconPair Theme::SplitImagePath(const QString& path)
{
    auto origin = 0;
    const QString image_prefix("Images/");
    if(path.startsWith(image_prefix)){
        origin = image_prefix.length();
    }
    int midSlash = path.lastIndexOf('/');

    IconPair pair;
    if(midSlash > 0){
        pair.first = path.mid(origin, midSlash - origin);
        pair.second = path.mid(midSlash + 1);
    }
    return pair;
}

bool Theme::tintIcon(IconPair pair){
    return tintIcon(getResourceName(pair));
}

bool Theme::tintIcon(const QString& resource_name){
    return tintIcon(pixmapSizeLookup.value(resource_name, QSize(0, 0)));
}

bool Theme::tintIcon(QSize size)
{
    return size.width() > 0 && size.width() % 96 == 0;
}

QString Theme::QColorToHex(const QColor& color)
{
    return color.name(QColor::HexArgb);
}

Theme *Theme::theme()
{
    static Theme theme;
    return &theme;
}


CustomMenuStyle::CustomMenuStyle(int icon_size) : QProxyStyle("Windows"){
    this->icon_size = icon_size;
}

int CustomMenuStyle::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const{
    auto s = QProxyStyle::pixelMetric(metric, option, widget);
    if (metric == QStyle::PM_SmallIconSize) {
        s = icon_size;
    }
    return s;
}


QStringList Theme::getIconPrefixs(){
    return icon_prefix_lookup.keys();
}

QStringList Theme::getIcons(const QString& icon_prefix){
    QStringList icons;
    if(icon_prefix_lookup.contains(icon_prefix)){
        icons = icon_prefix_lookup[icon_prefix].toList();
    }
    return icons;
}