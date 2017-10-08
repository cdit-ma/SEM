#include "theme.h"
#include <QDebug>
#include <QStringBuilder>
#include <QDirIterator>
#include <QDateTime>
#include <QStringBuilder>
#include <QtConcurrent/QtConcurrentRun>
#include <QThreadPool>
#include <QAction>
#include <QApplication>



Theme* Theme::themeSingleton = 0;

Theme::Theme() : QObject(0)
  //,lock(QReadWriteLock::Recursive)
{
    //Set up the default colors
    iconColor = QColor(70,70,70);
    selectedItemBorderColor = Qt::blue;

    //Get the original settings from the settings.
    setupAliasIcons();
    setupToggledIcons();
    updateValid();

    
    //Preload images on a background thread.
    preloadThread = QtConcurrent::run(QThreadPool::globalInstance(), this, &Theme::preloadImages);
}

Theme::~Theme()
{
    //Wait for the preloading to finish
    preloadThread.waitForFinished();
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
    //return highlightColor.lighter(110);
    //return highlightColor.lighter(105);
    return highlightColor.darker(105);
}

QString Theme::getPressedColorHex()
{
    QColor color = getPressedColor();
    return Theme::QColorToHex(color);
}

QColor Theme::getSelectedItemBorderColor()
{
    return selectedItemBorderColor;
}

QString Theme::getSelectedItemBorderColorHex()
{
    return Theme::QColorToHex(getSelectedItemBorderColor());
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

QSize Theme::roundQSize(QSize size)
{
    int factor = 1;
    //Bitshift round to power of 2
    while((factor <<= 1 ) <= size.width()>>1);

    //Scale the request image size to a width = factor
    int newWidth = factor;
    int newHeight = (double) factor * ((double)size.width() / (double)size.height());
    size.setWidth(newWidth);
    size.setHeight(newHeight);
    return size;
}

IconPair Theme::getIconPair(QString prefix, QString alias)
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

QColor Theme::getTextColor(Theme::COLOR_ROLE role)
{
    if(textColor.contains(role)){
        return textColor[role];
    }
    return QColor();
}

QString Theme::getTextColorHex(Theme::COLOR_ROLE role)
{
    QColor color = getTextColor(role);
    return Theme::QColorToHex(color);
}

void Theme::setTextColor(Theme::COLOR_ROLE role, QColor color)
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

QColor Theme::getMenuIconColor(Theme::COLOR_ROLE role)
{
    if(menuIconColor.contains(role)){
        return menuIconColor[role];
    }
    return QColor();
}

QString Theme::getMenuIconColorHex(Theme::COLOR_ROLE role)
{
    QColor color = getMenuIconColor(role);
    return Theme::QColorToHex(color);
}

void Theme::setMenuIconColor(Theme::COLOR_ROLE role, QColor color)
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

QString Theme::getAspectBackgroundColorHex(VIEW_ASPECT aspect)
{
    QColor color = getAspectBackgroundColor(aspect);
    return Theme::QColorToHex(color);
}

void Theme::setWindowIcon(QString window_title, QString visible_icon_prefix, QString visible_icon_alias){
    setIconToggledImage("WindowIcon", window_title, visible_icon_prefix, visible_icon_alias, "Icons", "transparent");
}

void Theme::setIconToggledImage(QString prefix, QString alias, QString toggledOnPrefix, QString toggledOnAlias, QString toggledOffPrefix, QString toggleOffAlias, bool ignore_toggle_coloring){
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

    iconToggledLookup[name] = QPair<IconPair, IconPair>(toggled_on, toggled_off);
    iconToggledLookup2[name] = icon_toggle;
}

QColor Theme::getDefaultImageTintColor()
{
    return iconColor;
}

QString Theme::getDefaultImageTintColorHex()
{
    return Theme::QColorToHex(iconColor);
}

QColor Theme::getMainImageColor(QString prefix, QString alias)
{
    QString key = getResourceName(prefix, alias);
    if(pixmapTintLookup.contains(key)){
        return pixmapTintLookup[key];
    }else if(pixmapMainColorLookup.contains(key)){
        return pixmapMainColorLookup[key];
    }else{
        //qCritical() << key << " Doesn't have pixel data?!";
        return QColor();
    }
}

QColor Theme::getMainImageColor(IconPair path)
{
    return getMainImageColor(path.first, path.second);
}

void Theme::setDefaultImageTintColor(QColor color)
{
    if(iconColor != color){
        iconColor = color;
        updateValid();
    }
}

void Theme::setDefaultImageTintColor(QString prefix, QString alias, QColor color)
{
    QString longName = getResourceName(prefix, alias);
    pixmapTintLookup[longName] = color;
}

void Theme::applyTheme()
{
    clearIconMap();
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

bool Theme::gotImage(QString path, QString alias)
{
    auto resource_name = getResourceName(path, alias);
    QReadLocker lock(&lock_);
    return image_names.contains(resource_name);
}

QString Theme::getBorderWidth()
{
    return "1px";
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

QIcon Theme::getIcon(QString prefix, QString alias)//{//, bool ignore_checked_colors)
{
    bool ignore_checked_colors = false;
    //qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QString lookupName = getResourceName(prefix, alias);

    if(iconLookup.contains(lookupName)){
        return iconLookup[lookupName];
    }else{
        
        QIcon icon;

        IconToggle icon_toggle;


        //Check if we have a toggled Icon
        if(iconToggledLookup2.contains(lookupName)){
            icon_toggle = iconToggledLookup2.value(lookupName);
        }else{
            icon_toggle.off.first = prefix;
            icon_toggle.off.second = alias;
        }

        auto off_rn = getResourceName(icon_toggle.off);
        auto on_rn = getResourceName(icon_toggle.on);

        bool is_off_tinted = tintIcon(off_rn);
        bool is_on_tinted = tintIcon(on_rn);

        QSize blank_size;

        //Handle Off State
        {
            //Set the default states.
            icon.addPixmap(_getPixmap(off_rn, blank_size, getMenuIconColor(icon_toggle.off_normal)), QIcon::Normal, QIcon::Off);
            
            if(is_off_tinted){
                icon.addPixmap(_getPixmap(off_rn, blank_size, getMenuIconColor(icon_toggle.off_active)), QIcon::Active, QIcon::Off);
                icon.addPixmap(_getPixmap(off_rn, blank_size, getMenuIconColor(icon_toggle.off_disabled)), QIcon::Disabled, QIcon::Off);
            }
        }

        
        //Handle On State
        if(icon_toggle.got_toggle){
            icon.addPixmap(_getPixmap(on_rn, blank_size, getMenuIconColor(icon_toggle.on_normal)), QIcon::Normal, QIcon::On);

            if(is_on_tinted){
                icon.addPixmap(_getPixmap(on_rn, blank_size, getMenuIconColor(icon_toggle.on_active)), QIcon::Active, QIcon::On);
                icon.addPixmap(_getPixmap(on_rn, blank_size, getMenuIconColor(icon_toggle.on_disabled)), QIcon::Disabled, QIcon::On);
            }
        }else{
            icon.addPixmap(_getPixmap(off_rn, blank_size, getMenuIconColor(icon_toggle.on_normal)), QIcon::Normal, QIcon::On);
            icon.addPixmap(_getPixmap(off_rn, blank_size, getMenuIconColor(icon_toggle.on_disabled)), QIcon::Disabled, QIcon::On);
        }

        iconLookup[lookupName] = icon;
        return icon;
    }
}

QPixmap Theme::_getPixmap(QString resourceName, QSize size, QColor tintColor)
{
    QPixmap pixmap;
    //Get the name of the pixmap url
    QString pixmap_url = getPixmapResourceName(resourceName, size, tintColor);

    if(pixmapLookup.contains(pixmap_url)){
        //Get the pixmap from the lookup
        pixmap = pixmapLookup.value(pixmap_url);
    }else{
        //Get the Image (Load it if it isn't loaded)
        QImage image = getImage(resourceName);
        //Get the size of the original Image
        QSize original_size = image.size();

        if(!image.isNull()){

            //If we haven't got a tint color, get a default tint color (If Any)
            if(!tintColor.isValid()){
                tintColor = getTintColor(resourceName);
            }

            //If we have been given a size, which is different to the original size, round it
            if(size.isValid() && size != original_size){
                size = roundQSize(size);
                //Scale the image
                image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            //Tint the icon if we have to
            if(tintColor.isValid() && tintIcon(original_size)){
                //Replace the image with it's alphaChannel
                image = image.alphaChannel();

                //Replace the colors with the tinted color.
                for(int i = 0; i < image.colorCount(); ++i) {
                    tintColor.setAlpha(qGray(image.color(i)));
                    image.setColor(i, tintColor.rgba());
                }
            }

            //Construct a Pixmap from the image.
            pixmap = QPixmap::fromImage(image);

            //Store it in the pixmap map
            pixmapLookup[pixmap_url] = pixmap;
        }
    }
    return pixmap;
}

QPixmap Theme::getImage(QString prefix, QString alias, QSize size, QColor tintColor)
{
    return _getPixmap(getResourceName(prefix, alias), size, tintColor);
}

QString Theme::getPixmapResourceName(QString resource_name, QSize size, QColor tintColor)
{
    QSize original_size = pixmapSizeLookup.value(resource_name);

    if(size.isValid() && size != original_size){
        size = roundQSize(size);
        resource_name = resource_name % '_' % QString::number(size.width()) % '_' % QString::number(size.height());
    }

    if(tintColor.isValid()){
        resource_name = resource_name % '_' % QColorToHex(tintColor);
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
QSize Theme::getSmallIconSize(){
    return icon_size / 2;
}

void Theme::setIconSize(int size){

    auto new_size = roundQSize(QSize(size, size));
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

QMovie* Theme::getGif(QString path, QString name){
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
    /*
    return "QTabBar::tab{ background:" % getAltBackgroundColorHex() % "; color: " % getTextColorHex() % "; border-radius:" % getSharpCornerRadius() % "; border: 1px solid " % getDisabledBackgroundColorHex() % ";}"
           "QTabBar::tab:top{ padding: 5px 10px; margin: 5px 1px; border-bottom-left-radius: 0px; border-bottom-right-radius: 0px; }"
           "QTabBar::tab:right{ padding: 10px 5px; margin: 1px 5px; border-top-right-radius: 0px; border-bottom-right-radius: 0px; }"
           //"QTabBar::tab:selected{ background:" % getPressedColorHex() % "; color: " % getTextColorHex(CR_SELECTED) % ";}"
           "QTabBar::tab:selected{ border-width: 2px; background:" % getBackgroundColorHex() % "; font-weight: bold; padding: 5px; margin: 1px; }"
           "QTabBar::tab:hover{ background:" % getHighlightColorHex() % "; color: " % getTextColorHex(CR_SELECTED) % ";}"
           "QTabBar, QTabWidget::tab-bar{ alignment: center; qproperty-drawBase: 0; border: 0px; }"
           "QTabWidget::pane, QTabBar::pane{ border-bottom: 2px solid red; background:" % getBackgroundColorHex() % ";}"
           ;
           */

    return "QTabBar::tab{ background:" % getAltBackgroundColorHex() % "; color: " % getTextColorHex() % "; border-radius:" % getCornerRadius() % "; border: 1px solid " % getBackgroundColorHex() % ";}"
           "QTabBar::tab:top{ border-bottom-left-radius: 0px; border-bottom-right-radius: 0px; margin: 1px 0px; padding: 5px 10px; }"
           "QTabBar::tab:right{ border-top-right-radius: 0px; border-bottom-right-radius: 0px; margin: 0px 1px; padding: 10px 5px; }"
           "QTabBar::tab:selected{ background:" % getActiveWidgetBorderColorHex() % "; border-color:" % getDisabledBackgroundColorHex() % ";}"
           "QTabBar::tab:hover{ background:" % getHighlightColorHex() % "; color: " % getTextColorHex(CR_SELECTED) % ";}"
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
           "color:" % getTextColorHex(CR_SELECTED) % ";"
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
           "color:" % getTextColorHex(CR_DISABLED) % ";"
           "}"
           "QMenu::item:selected:!disabled {"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
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
               "background:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           "QMenu::separator {"
            "height: 1px;"
            "background: " % getTextColorHex(CR_DISABLED) % ";"
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
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           "QToolButton:hover {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
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
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           "QToolButton[popupMode=\"1\"]:pressed {"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           "QToolButton[popupMode=\"2\"] {"
           "padding-right: 15px;"
           "color:" % getTextColorHex() % ";"
           "}"
           "QToolButton[popupMode=\"2\"]:hover {"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           "QToolButton[popupMode=\"2\"]:pressed {"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
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
           "color:" % getTextColorHex(CR_DISABLED) % ";"
           "}"
           "QAbstractItemView::item::selected {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           "QAbstractItemView::item::hover {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
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
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           "QHeaderView::section:selected {"
           "font-weight: normal;"
           "}"
            "QAbstractItemView QWidget {"
            "color:" % getTextColorHex() % ";"
            "border: none;"
            "background:" % getBackgroundColorHex() % ";"
            "selection-color:" % getTextColorHex(CR_SELECTED) % ";"
            "selection-background-color:" % getHighlightColorHex() % ";"
            "}";
}

QString Theme::getAltAbstractItemViewStyleSheet()
{
    QString borderWidth = getBorderWidth();
    return "QAbstractItemView {"
           "border:" % borderWidth % " solid " % getDisabledBackgroundColorHex() % ";"
           "background:" % getBackgroundColorHex() % ";"
           "}"
           "QAbstractItemView::item {"
           "border-style: solid;"
           "border-width: 0px 0px " % borderWidth % " 0px;"
           "border-color:" % getDisabledBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "padding: 2px 2px 3px 2px;"
           "}"
           "QAbstractItemView::item:selected {"
           "background:" % getAltBackgroundColorHex() % ";"
           "}"
           "QAbstractItemView::item:hover {"
           "background:" % getDisabledBackgroundColorHex() % ";"
           "}"

           "QHeaderView {"
           "background:" % getBackgroundColorHex() % ";"
           "border: 0px;"
           "color:" % getTextColorHex() % ";"
           "}"
           "QHeaderView::section {"
           "border-style: solid;"
           "border-width: 0px " % borderWidth % " " % borderWidth % " 0px;"
           "border-color:" % getDisabledBackgroundColorHex() % ";"
           "background:" % getAltBackgroundColorHex() % ";"
           "padding: 0px 5px;"
           "}";
}

QString Theme::getComboBoxStyleSheet()
{
    return "QComboBox {"
           "background:" % getAltBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "selection-background-color:" % getHighlightColorHex() % ";"
           "selection-color:" % getTextColorHex(CR_SELECTED) % ";"
           "border: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "padding: 0px 5px;"
           "margin: 0px;"
           "}"
           "QComboBox:hover {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           "QComboBox::drop-down {"
           "subcontrol-position: top right;"
           "padding: 0px;"
           "width: 20px;"
           "}"
           "QComboBox QAbstractItemView {"
           "color:" % getTextColorHex() % ";"
           "background:" % getAltBackgroundColorHex() % ";"
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
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
            /*
           "QPushButton::checked {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           */
           "QPushButton:disabled {"
           "background:" % getDisabledBackgroundColorHex() % ";"
           "}";
}

QString Theme::getToolButtonStyleSheet()
{
    return "QToolButton {"
            "text-align: center;"
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
           "selection-color: " % getTextColorHex(CR_SELECTED) % ";"
           "}" + 
           widget_name +":focus {"
           "border: 1px solid "% getHighlightColorHex() % ";"
           "}";
}

QString Theme::getTextEditStyleSheet()
{
    return getLineEditStyleSheet("QPlainTextEdit");
}

QString Theme::getRadioButtonStyleSheet()
{
    return "QRadioButton {"
           "padding: 8px 10px 8px 8px;"
           "color:" % getTextColorHex() % ";"
           "}"
           "QRadioButton::checked {"
           "font-weight: bold; "
           "color:" % getHighlightColorHex() % ";"
           "}";
}

QString Theme::getMessageBoxStyleSheet()
{
    return  "QMessageBox {"
            "background:" % getAltBackgroundColorHex() % ";"
            "padding: 2px;"
            "}"
            "QMessageBox QLabel {"
            "color:" % getTextColorHex() % ";"
            "}";
}

QString Theme::getPopupWidgetStyleSheet()
{
    QColor bgColor = getBackgroundColor();
    bgColor.setAlphaF(0.85);

    return "QWidget {"
           "margin: 0px;"
           "padding: 0px;"
           "background:" % QColorToHex(bgColor) % ";"
           "border-radius:" % getCornerRadius() % ";    "
           "border: 1px outset " % getAltBackgroundColorHex() % ";"
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

QString Theme::getTitleLabelStyleSheet()
{
    return "color: " % getTextColorHex() % ";";
}

QString Theme::getAspectButtonStyleSheet(VIEW_ASPECT aspect)
{
    QColor color = getAspectBackgroundColor(aspect).darker(110);
    QString gradientColor1 = QColorToHex(color.lighter(275));
    QString gradientColor2 = QColorToHex(color.darker(110));

    return "QAbstractButton {"
           "color:" % QColorToHex(black()) % ";"
           "border: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "background:" % QColorToHex(color) % ";"
           "}"
           "QAbstractButton:hover {"
           "color:" % QColorToHex(white()) % ";"
           "border: 1px solid " % QColorToHex(white()) % ";"
           "}"
           "QAbstractButton:checked {"
           "background:"
           "qlineargradient(x1:0, y1:0, x2:0, y2:1.0,"
           "stop:0 " % gradientColor1 % ", stop:1.0 " % gradientColor2 + ");"
           "}"
           "QAbstractButton:disabled {"
           "background:" % getDisabledBackgroundColorHex() % ";"
           "}";
}

void Theme::preloadImages()
{   
    auto time_start = QDateTime::currentDateTime().toMSecsSinceEpoch();
    int load_count = 0;

    //Recurse through all files in the image alias.
    QDirIterator it(":/Images/", QDirIterator::Subdirectories);

    QList<QString> files;
    {
        QWriteLocker lock(&lock_);
        while (it.hasNext()){
            it.next();
            if(it.fileInfo().isFile()){
                //Trim the :/ from the path1
                auto file_path = it.filePath().mid(2);
                image_names.insert(file_path);
                files.append(file_path);
            }
        }
    }
    for(auto file_path : files){
         //load the image
         if(getImage(file_path).isNull()){
            qCritical() << "Image: " << file_path << " Is an null image";
        }else{
            load_count ++;
        }
    }

    auto time_finish = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qCritical() << "Preloaded #" << load_count << " Images in: " <<  time_finish - time_start << "MS";
    clearIconMap();
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
    case SETTINGS::THEME_BG_ALT_COLOR:{
        setAltBackgroundColor(color);
        break;
    }
    case SETTINGS::THEME_TEXT_COLOR:{
        setTextColor(CR_NORMAL, color);
        break;
    }
    case SETTINGS::THEME_ALTERNATE_TEXT_COLOR:{
        setAltTextColor(color);
        break;
    }
    case SETTINGS::THEME_ICON_COLOR:{
        setMenuIconColor(CR_NORMAL, color);
        break;
    }
    case SETTINGS::THEME_BG_DISABLED_COLOR:{
        setDisabledBackgroundColor(color);
        break;
    }
    case SETTINGS::THEME_TEXT_DISABLED_COLOR:{
        setTextColor(CR_DISABLED, color);
        break;
    }
    case SETTINGS::THEME_ICON_DISABLED_COLOR:{
        setMenuIconColor(CR_DISABLED, color);
        break;
    }
    case SETTINGS::THEME_BG_SELECTED_COLOR:{
        setHighlightColor(color);
        break;
    }
    case SETTINGS::THEME_TEXT_SELECTED_COLOR:{
        setTextColor(CR_SELECTED, color);
        break;
    }
    case SETTINGS::THEME_ICON_SELECTED_COLOR:{
        setMenuIconColor(CR_SELECTED, color);
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
    case SETTINGS::THEME_SETTHEME_DARKTHEME:{
        resetTheme(VT_DARK_THEME);
        break;
    }
    case SETTINGS::THEME_SETTHEME_LIGHTHEME:{
        resetTheme(VT_LIGHT_THEME);
        break;
    }
    case SETTINGS::THEME_SETTHEME_SOLARIZEDDARKTHEME:{
        resetTheme(VT_SOLARIZED_DARK_THEME);
        break;
    }
    case SETTINGS::THEME_SETTHEME_SOLARIZEDLIGHTTHEME:{
        resetTheme(VT_SOLARIZED_LIGHT_THEME);
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

void Theme::clearIconMap()
{
    QWriteLocker lock(&lock_);
    iconLookup.clear();
}

QColor Theme::calculateImageColor(QImage image)
{
    QColor c;
    if(!image.isNull()){
        int size = 32;
        image = image.scaled(size, size);

        QHash<QRgb, int> colorCount;
        int max = 0;
        QRgb frequentColor = QColor(Qt::black).rgb();

        int f = 25;

        for(int i = 0; i < size * size; i++){
            int x = i % size;
            int y = i / size;
            QRgb pixel = image.pixel(x,y);

            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);

            //Ignore black
            if(r < f && g < f && b < f){
                continue;
            }

            if(colorCount.contains(pixel)){
                colorCount[pixel] ++;
            }else{
                colorCount[pixel] = 1;
            }

            int count = colorCount[pixel];
            if(count > max){
                frequentColor = pixel;
                max = count;
            }
        }
        c = QColor(frequentColor);
        if(colorCount.isEmpty()){
            c = iconColor;
        }
    }
    return c;
}

QString resourceName(QString prefix, QString alias){
    return "Images/" % prefix % '/' % alias;
}

QString Theme::getResourceName(QString prefix, QString alias) const
{
    //Uncomment for bounding rects
    //return "Images/Icons/square";
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


    setIconToggledImage("ToggleIcons", "maximize", "Icons", "minimize", "Icons", "maximize");
    setIconToggledImage("ToggleIcons", "lock", "Icons", "lockOpened", "Icons", "lockClosed");
    setIconToggledImage("ToggleIcons", "visible", "Icons", "eye", "Icons", "transparent");

    setIconToggledImage("ToggleIcons", "newNotification", "Icons", "exclamation", "Icons", "clock", false);

    
}

void Theme::setupAliasIcons(){
    setIconAlias("Data", "label", "Icons", "label");
    setIconAlias("Data", "description", "Icons", "speechBubbleFilled");
    setIconAlias("Data", "kind", "Icons", "tiles");
    setIconAlias("Data", "namespace", "Icons", "letterA");
    setIconAlias("Data", "value", "Icons", "pencil");
    setIconAlias("Data", "type", "Icons", "gearDark");
    setIconAlias("Data", "ID", "Icons", "numberOne");
}   

void Theme::setIconAlias(QString prefix, QString alias, QString icon_prefix, QString icon_alias){
    auto alias_rn = resourceName(prefix, alias);
    auto icon_rn = resourceName(icon_prefix, icon_alias);
    
    if(!icon_alias_lookup.contains(alias_rn)){
        icon_alias_lookup[alias_rn] = icon_rn;
    }
}


void Theme::resetTheme(VIEW_THEME themePreset){
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

    switch(themePreset){
        case VT_DARK_THEME:{
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
            emit changeSetting(SETTINGS::THEME_SEVERITY_WARNING_COLOR, QColor(255,200,0));
            emit changeSetting(SETTINGS::THEME_SEVERITY_ERROR_COLOR, QColor(255,50,50));
            emit changeSetting(SETTINGS::THEME_SEVERITY_SUCCESS_COLOR, QColor(50,205,50));
            resetAspectTheme(true);
            break;
        }
        case VT_LIGHT_THEME:{
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
        case VT_SOLARIZED_DARK_THEME:{
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
        case VT_SOLARIZED_LIGHT_THEME:{
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
        
        QApplication::setFont(font);
        updateValid();
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
    bool gotAllColors = true;
    if(highlightColor.isValid() && backgroundColor.isValid() && altBackgroundColor.isValid() && iconColor.isValid() && disabledBackgroundColor.isValid()){
        QList<COLOR_ROLE> states;
        states << CR_NORMAL << CR_DISABLED << CR_SELECTED;

        foreach(COLOR_ROLE state, states){
            QColor tColor = textColor[state];
            QColor iColor = menuIconColor[state];

            if(tColor.isValid() && iColor.isValid()){
               continue;
            }else{
                gotAllColors = false;
                break;
            }
        }

        foreach(VIEW_ASPECT aspect, GET_VIEW_ASPECTS()){
            QColor color = aspectColor[aspect];

            if(color.isValid()){
               continue;
            }else{
                gotAllColors = false;
                break;
            }
        }
        for(auto severity : Notification::getSeverities()){
            if(!severityColor.contains(severity)){
                gotAllColors = false;
                break;
            }
        }
    }else{
        gotAllColors = false;
    }

    valid = gotAllColors;
    themeChanged = true;
}

QImage Theme::getImage(QString resource_name)
{
    {
        //Try load the image
        QReadLocker lock(&lock_);
        if(imageLookup.contains(resource_name)){
            return imageLookup[resource_name];
        }
    }
    //Else load the image
    QImage image(":/" % resource_name);
    //qCritical() << resource_name;
    auto color = calculateImageColor(image);

    QWriteLocker lock(&lock_);
    //Gain lock to write to the lookups
    imageLookup[resource_name] = image;
    pixmapSizeLookup[resource_name] = image.size();
    pixmapMainColorLookup[resource_name] = color;
    return image;
}

QColor Theme::getTintColor(QString resource_name)
{
    QReadLocker lock(&lock_);
    return pixmapTintLookup.value(resource_name, iconColor);
}

QSize Theme::getOriginalSize(QString resource_name)
{
    QReadLocker lock(&lock_);
    return pixmapSizeLookup.value(resource_name);
}

IconPair Theme::splitImagePath(QString path)
{
    int midSlash = path.lastIndexOf('/');

    IconPair pair;
    if(midSlash > 0){
        pair.first = path.mid(0, midSlash);
        pair.second = path.mid(midSlash + 1);
    }
    return pair;
}

bool Theme::tintIcon(IconPair pair){
    return tintIcon(getResourceName(pair));
}

bool Theme::tintIcon(QString resource_name){
    return tintIcon(pixmapSizeLookup.value(resource_name, QSize(0, 0)));
}
bool Theme::tintIcon(QSize size)
{
    return size.width() > 0 && size.width() % 96 == 0;
}

QColor Theme::blendColors(const QColor color1, const QColor color2, qreal blendRatio)
{
    QColor resultingColor;
    qreal color1Ratio = blendRatio;
    qreal color2Ratio = 1 - blendRatio;
    resultingColor.setBlue(color1Ratio * color1.blue() + color2Ratio * color2.blue());
    resultingColor.setRed(color1Ratio * color1.red() + color2Ratio * color2.red());
    resultingColor.setGreen(color1Ratio * color1.green() + color2Ratio * color2.green());
    return resultingColor;
}

QString Theme::QColorToHex(const QColor color)
{
    return color.name(QColor::HexArgb);
}

Theme *Theme::theme()
{
    if(!themeSingleton){
        themeSingleton = new Theme();
    }
    return themeSingleton;
}

void Theme::teardownTheme()
{
    delete themeSingleton;
    themeSingleton = 0;
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
