#include "theme.h"
#include <QDebug>
#include <QStringBuilder>
#include <QDirIterator>
#include <QDateTime>
#include <QStringBuilder>
#include <QtConcurrent/QtConcurrentRun>
#include <QThreadPool>

Theme* Theme::themeSingleton = 0;

Theme::Theme():QObject(0)
{
    terminating = false;
    preloadedImages = false;
    themeChanged = false;
    slash = QString("/");

    //Cadet Blue
    deployColor = QColor(95, 158, 160);
    setDefaultImageTintColor(QColor(70,70,70));



    selectedItemBorderColor = Qt::blue;

    updateValid();
    setupIcons();


    preloadThread = QtConcurrent::run(QThreadPool::globalInstance(), this, &Theme::preloadImages);
}

Theme::~Theme()
{
    //Terminate!
    terminating = true;
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
    return highlightColor.lighter(110);
}

QString Theme::getPressedColorHex()
{
    QColor color = getPressedColor();
    return Theme::QColorToHex(color);
}

QColor Theme::getDeployColor()
{
    return deployColor;
}

QString Theme::getDeployColorHex()
{
    return Theme::QColorToHex(getDeployColor());
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

QPair<QString, QString> Theme::getIconPair(QString prefix, QString alias)
{
    QPair<QString, QString> pair;
    pair.first = prefix;
    pair.second = alias;
    return pair;
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

void Theme::setIconToggledImage(QString prefix, QString alias, QString toggledAlias, QString toggledImageName)
{
    QString name = prefix % slash % alias;
    QString toggledName = toggledAlias  % slash %  toggledImageName;
    iconToggledLookup[name] = toggledName;
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
        qCritical() << key << " Doesn't have pixel data?!";
    }
}

QColor Theme::getMainImageColor(QPair<QString, QString> path)
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

    QString longName = prefix % slash % alias;
    pixmapTintLookup[longName] = color;
}

void Theme::applyTheme()
{
    if(themeChanged && valid){
        //PRINT OUT MEMORY USAGE.
        iconLookup.clear();
        emit theme_Changed();
    }
    themeChanged = false;
}

bool Theme::isValid()
{
    return valid;
}

QString Theme::getCornerRadius()
{
    return "2px";
}

QIcon Theme::getIcon(QPair<QString, QString> icon)
{
    return getIcon(icon.first, icon.second);
}

QIcon Theme::getIcon(QString prefix, QString alias)
{
    //qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QString lookupName = prefix  % slash %  alias;

    if(iconLookup.contains(lookupName)){
        return iconLookup[lookupName];
    }else{
        if(!imageLookup.contains(lookupName)){
            //If we haven't loaded the original image we can't tell if it needs tinting!
            getImage(prefix, alias);
        }


        bool isTinted = tintIcon(prefix, alias);

        QIcon icon;

        //Set the default states.
        icon.addPixmap(getImage(prefix, alias, QSize(), getMenuIconColor(CR_NORMAL)), QIcon::Normal, QIcon::Off);
        //icon.addPixmap(getImage(prefix, alias, QSize(), getMenuIconColor(CR_NORMAL)), QIcon::Normal, QIcon::On);
        if(isTinted){
            icon.addPixmap(getImage(prefix, alias, QSize(), getMenuIconColor(CR_SELECTED)), QIcon::Active, QIcon::Off);
            icon.addPixmap(getImage(prefix, alias, QSize(), getMenuIconColor(CR_DISABLED)), QIcon::Disabled, QIcon::Off);
        }

        if(iconToggledLookup.contains(lookupName)){
            QString toggledName = iconToggledLookup[lookupName];
            int midSlash = toggledName.lastIndexOf("/");

            if(midSlash > 0){
                QString toggledPrefixName = toggledName.mid(0, midSlash);
                QString toggledAliasName = toggledName.mid(midSlash + 1);

                bool isToggledTinted = tintIcon(toggledPrefixName, toggledAliasName);

                //Set the toggled states.
                icon.addPixmap(getImage(toggledPrefixName, toggledAliasName, QSize(), getMenuIconColor(CR_NORMAL)), QIcon::Normal, QIcon::On);
                if(isToggledTinted){
                    icon.addPixmap(getImage(toggledPrefixName, toggledAliasName, QSize(), getMenuIconColor(CR_SELECTED)), QIcon::Active, QIcon::On);
                    icon.addPixmap(getImage(toggledPrefixName, toggledAliasName, QSize(), getMenuIconColor(CR_DISABLED)), QIcon::Disabled, QIcon::On);
                }
            }
        }

        iconLookup[lookupName] = icon;
        //qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
        //qCritical() <<  lookupName << " HARD TIME: " <<  timeFinish-timeStart;
        return icon;
    }
}

QPixmap Theme::getImage(QString prefix, QString alias, QSize size, QColor tintColor)
{
    QString resourceName = getResourceName(prefix, alias);
    QString lookupName = resourceName;

    //If we have a tint color check for it.
    if(tintColor.isValid()){
        lookupName = lookupName % slash % QColorToHex(tintColor);
    }

    //If the size we have been provided isn't set, check for the original image size.
    if(!size.isValid()){
        if(pixmapSizeLookup.contains(resourceName)){
            size = pixmapSizeLookup[resourceName];
        }
    }

    //If we have a valid size
    if(size.isValid()){
        size = roundQSize(size);
        //Update the lookupName to include this new size information.
        lookupName = lookupName % slash % QString::number(size.width()) % slash % QString::number(size.height());
    }


    if(pixmapLookup.contains(lookupName)){
        return pixmapLookup[lookupName];
    }else{
        QSize originalSize;

        //Try and get original Size of the resource.
        if(pixmapSizeLookup.contains(resourceName)){
            originalSize = pixmapSizeLookup[resourceName];
        }


        if(originalSize.isValid()){
            //If the size we want is bigger than the original size, return the exact original image.
            if(size.width() > originalSize.width()){
                //Request the image at max size.
                return getImage(prefix, alias, originalSize, tintColor);
            }
        }



        QImage image;

        if(imageLookup.contains(resourceName)){
            image = imageLookup[resourceName];
        }else{
            //Load the original Image
            image =  QImage(":/" % resourceName);
            //Store it
            imageLookup[resourceName] = image;

            calculateImageColor(resourceName);
        }

        if(image.isNull()){
            //qCritical() << "Cannot find image: " << resourceName;
            pixmapLookup[lookupName] = QPixmap();
            return pixmapLookup[lookupName];
        }

        //If we haven't seen this Image before, we should store the original size of the Image
        if(!pixmapSizeLookup.contains(resourceName)){
            originalSize = image.size();
            pixmapSizeLookup[resourceName] = originalSize;

            //Update the lookupName to include the original size.
            lookupName = lookupName % slash % QString::number(size.width()) % slash % QString::number(size.height());
        }

        //Handle the tint color.
        if(!tintColor.isValid()){
            //Check for a non-default color.
            if(pixmapTintLookup.contains(resourceName)){
                tintColor = pixmapTintLookup[resourceName];
            }else{
                //Return the default color tint.
                tintColor = iconColor;
            }
        }

        //If the size we want is bigger than the original.
        if(size.width() > originalSize.width()){
            //Request the image at the original size.
            return getImage(prefix, alias, originalSize, tintColor);
        }else{
            if(size.isValid() && originalSize.isValid()){
                //Scale the image to the required size.
                image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
        }


        //Tint the pixmap If it's a multiple of 96
        if(tintIcon(originalSize)){
            //qCritical() << originalSize;
            //Replace the image with it's alphaChannel
            image = image.alphaChannel();


            //Replace the colors with the tinted color.
            for(int i = 0; i < image.colorCount(); ++i) {
                tintColor.setAlpha(qGray(image.color(i)));
                image.setColor(i, tintColor.rgba());
            }
        }
        //Construct a Pixmap from the image.
        QPixmap pixmap = QPixmap::fromImage(image);

        pixmapLookup[lookupName] = pixmap;
        return pixmap;
    }
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

QString Theme::getWindowStyleSheet()
{
    return "QMainWindow {"
           "background: " % getBackgroundColorHex() % ";"
           "color: " % getTextColorHex() % ";"
           "background-image: url(:/MEDEA_Watermark); background-position: center; background-repeat: no-repeat;"
            "}" ;
}

QString Theme::getScrollBarStyleSheet()
{
    int marginSize = 1;
    int scrollSize = 16;
    int buttonSize = scrollSize - (2* marginSize);

    QString margin = QString::number(marginSize) % "px";
    QString size = QString::number(scrollSize) % "px";
    QString button = QString::number(buttonSize) % "px";

    return   //"QScrollBar{background: " % getAltBackgroundColorHex() % ";border:none;margin:0px;}"
             "QScrollBar{background: " % getBackgroundColorHex() % "; border: 1px solid " % getDisabledBackgroundColorHex() % "; margin:0px;}"
             "QScrollBar:vertical{width:" % size % ";}"
             "QScrollBar:horizontal{height:" % size % ";}"
             "QScrollBar::handle{background: " % getAltBackgroundColorHex() % " ;}"
             "QScrollBar::handle:active{background: " % getHighlightColorHex() % ";}"
             "QScrollBar::add-line, QScrollBar::sub-line{background:none;}" // Up/Down Button holders;
             "QScrollBar::add-page, QScrollBar::sub-page{background:none;}" // Space between Handle and Up/Down Buttons
             "QScrollBar::handle{margin:" % margin % ";}" // Allow nice space.

            /* // Have buttons
             "QScrollBar::handle:vertical{margin:" % size % " " % margin %";}"
             "QScrollBar::handle:horizontal{margin:" % margin % " " % size %";}"
             "QScrollBar::up-arrow, QScrollBar::down-arrow{background: " % getBackgroundColorHex() % "; width: " % button %"; height:" % button % "; margin: " % margin % ";}"
             "QScrollBar::up-arrow:active, QScrollBar::down-arrow:active{background: " % getHighlightColorHex() % ";}"

             "QScrollBar::up-arrow{image: url(:/Actions/Arrow_Up);}"
             "QScrollBar::down-arrow{image: url(:/Actions/Arrow_Down);}"
            */
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
            "QSplitter::handle{width:12px;height:12px;}"
            "QSplitter::handle:pressed {background:" % getAltBackgroundColorHex() % "}"
            "QSplitter::handle:horizontal {image: url(:/Actions/Menu_Vertical);}"
            "QSplitter::handle:vertical {image: url(:/Actions/Menu_Horizontal);}"
            ;
}

QString Theme::getWidgetStyleSheet(QString widgetName)
{
    return widgetName % " {"
           "background:" % getBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "}";
}

QString Theme::getTabbedWidgetStyleSheet()
{
    return  "QTabBar::tab:top {padding: 5px 10px;;margin: 5px 1px;}"
            "QTabBar::tab:right {padding: 10px 5px;margin: 1px 5px;}"
            "QTabBar::tab {background:" % getAltBackgroundColorHex() % "; color: " % getTextColorHex() % ";border-radius:" % getCornerRadius() % ";}"
            "QTabBar::tab:selected {background:" % getHighlightColorHex() % "; color: " % getTextColorHex(CR_SELECTED) % ";}"
            "QTabBar, QTabWidget::tab-bar {alignment: center;}"
            "QTabWidget::pane,QTabBar::pane {border:none;background:" % getBackgroundColorHex() % "}"
            ;
}

QString Theme::getNodeViewStyleSheet(bool isActive)
{
    QString activeBorder = "border: 2px solid " % getActiveWidgetBorderColorHex() % ";";
    QString inActiveBorder = "border: 1px solid " % getDisabledBackgroundColorHex() % ";";
    return "QGraphicsView {"
           "background:" % getBackgroundColorHex() % ";"
           % (isActive ? activeBorder : inActiveBorder) %
           //"border: 1px solid " % (isActive ? getActiveWidgetBorderColorHex() : getDisabledBackgroundColorHex()) % ";"
           "}";
}

QString Theme::getDockWidgetStyleSheet()
{
    return "QDockWidget {"
           "margin: 5px;"
           "background:" % getBackgroundColorHex() % ";"
           "}";
}

QString Theme::getToolDockWidgetTitleBarStyleSheet()
{
    return  getDockTitleBarStyleSheet(false) %
            "DockTitleBarWidget QToolButton { padding: 0px; border-radius:2px; }";
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
            "}";
}

QString Theme::getMenuBarStyleSheet()
{
    return "QMenuBar {"
           "padding: 3px;"
           "background:" % getBackgroundColorHex() % ";"
           "border-bottom: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "border-radius:" % getCornerRadius() % ";"
           "}"
           "QMenuBar::item {"
           "background:" % getBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "padding: 5px;"
           "}"
           "QMenuBar::item:selected {"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "background:" % getHighlightColorHex() % ";"
           "border-radius:" % getCornerRadius() % ";"
           "}";
}

QString Theme::getMenuStyleSheet()
{
    return "QMenu {"
           "background:" % getAltBackgroundColorHex() % ";"
           "border-radius:" % getCornerRadius() % ";"
           "margin: 2px; "
           "}"
           "QMenu::item {"
           "padding: 2px 15px 2px 25px;"
           "background:" % getAltBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "border-radius: " % getCornerRadius() % ";"
           "border: 0px;"
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
           "position: absolute;"
           "top: 2px;"
           "right: 4px;"
           "bottom: 2px;"
           "left: 4px;"
           "}";
}

QString Theme::getToolBarStyleSheet()
{
    return "QToolBar {"
           "spacing: 3px;"
           "border: 0px;"
           "padding: 1px;"
           "margin: 0px;"
           "background: rgba(0,0,0,0);"
           //"background:" % getBackgroundColorHex() % ";"
           "}"
           "QToolBar::separator {"
           "width: 5px;"
           "background: rgba(0,0,0,0);"
           "}"

           "QToolButton {"
           "padding: 2px;"
           "border-radius: 5px;"
           "border: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "background:" % getAltBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "}"
           "QToolButton:hover {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           "QToolButton:pressed {"
           "background:" % getPressedColorHex() % ";"
           "}"
           /*
           "QToolButton::checked {"
           "background:" % getHighlightColorHex() % ";"
           "}"*/
           "QToolButton:disabled {"
           "background:" % getDisabledBackgroundColorHex() % ";"
           "border: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "}"
           "QToolButton[popupMode=\"1\"] {"
           "padding-right: 15px;"
           "color:" % getTextColorHex() % ";"
           "}"
           "QToolButton[popupMode=\"1\"]:hover {"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           "QToolButton::menu-indicator {"
           "image: none;"
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
           "padding:5px 0px;"
           "}"
           "QAbstractItemView::item::disabled {"
           "background:" % getBackgroundColorHex() % ";"
           "color:" % getTextColorHex(CR_DISABLED) % ";"
           "}"
           "QAbstractItemView::item::hover {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "}"
           "QAbstractItemView::item::selected {"
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
           "border: 1px solid " % getBackgroundColorHex() % ";"
           "padding: 0px 0px 0px 3px;"
           "}"
           "QHeaderView::section:hover {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "border: 0px;"
           "}"
           "QHeaderView::section:selected {"
           "font-weight: normal;"
           "}"

           "QAbstractItemView QLineEdit {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "border: 0px;"
           "}"
           "QAbstractItemView QComboBox {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "border: 0px;"
           "}"
           "QAbstractItemView QSpinBox {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "border: 0px;"
           "}"
           "QAbstractItemView QDoubleSpinBox {"
           "background:" % getHighlightColorHex() % ";"
           "color:" % getTextColorHex(CR_SELECTED) % ";"
           "border: 0px;"
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
           //"QComboBox::down-arrow{ image: url(:/Actions/Arrow_Down); }"
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
           "padding: 2px;"
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

QString Theme::getLineEditStyleSheet()
{
    return "QLineEdit {"
           "margin: 0px;"
           "padding: 0px;"
           "background:" % getAltBackgroundColorHex() % ";"
           "color:" % getTextColorHex() % ";"
           "border: 1px solid " % getDisabledBackgroundColorHex() % ";"
           "border-radius: 0px;"
           "}"
           "QLineEdit:focus {"
           "border: 1px solid " % getHighlightColorHex() % ";"
           "}";
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
            "}"
            "QMessageBox QLabel {"
            "color:" % getTextColorHex() % ";"
            "}";
}

QString Theme::getPopupWidgetStyleSheet()
{
    QColor bgColor = getBackgroundColor();
    bgColor.setAlphaF(0.75);

    return "QWidget {"
           "margin: 0px;"
           "padding: 0px;"
           "background:" % QColorToHex(bgColor) % ";"
           "border-radius:" % getCornerRadius() % ";"
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
    return "QLabel{ background: rgba(0,0,0,0); color:" % getTextColorHex() % ";}"
           "QLabel:hover{ background:" % getHighlightColorHex() + "; color:" + getTextColorHex(CR_SELECTED) + ";}";
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
    if(!preloadedImages){
        connect(this, &Theme::_preload, Theme::theme(), &Theme::preloadFinished, Qt::QueuedConnection);
        preloadedImages = true;
        qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
        QStringList dirs;
        dirs << "Actions" << "Data" << "Functions" << "Items" << "Welcome";
        int count = 0;
        foreach(QString dir, dirs){
            QDirIterator it(":/" % dir, QDirIterator::Subdirectories);
            while (it.hasNext() && !terminating) {
                it.next();
                QString imageName = it.fileName();
                getImage(dir, imageName);
                if(imageLookup.contains(getResourceName(dir, imageName))){
                    count++;
                }
            }
        }
        //Only Allow once.
        qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();

        if(!terminating){
            qCritical() << "Preloaded #" << count << "images in: " <<  timeFinish-timeStart << "MS";
            emit _preload();
        }else{
            qCritical() << "Preloading Cancelled #" << count << "images in: " <<  timeFinish-timeStart << "MS";
        }
    }
}

void Theme::settingChanged(SETTING_KEY setting, QVariant value)
{
    QColor color = value.value<QColor>();

    switch(setting){
    case SK_THEME_BG_COLOR:{
        setBackgroundColor(color);
        break;
    }
    case SK_THEME_BG_ALT_COLOR:{
        setAltBackgroundColor(color);
        break;
    }
    case SK_THEME_TEXT_COLOR:{
        setTextColor(CR_NORMAL, color);
        break;
    }
    case SK_THEME_ALTERNATE_TEXT_COLOR:{
        setAltTextColor(color);
        break;
    }
    case SK_THEME_ICON_COLOR:{
        setMenuIconColor(CR_NORMAL, color);
        break;
    }
    case SK_THEME_BG_DISABLED_COLOR:{
        setDisabledBackgroundColor(color);
        break;
    }
    case SK_THEME_TEXT_DISABLED_COLOR:{
        setTextColor(CR_DISABLED, color);
        break;
    }
    case SK_THEME_ICON_DISABLED_COLOR:{
        setMenuIconColor(CR_DISABLED, color);
        break;
    }
    case SK_THEME_BG_SELECTED_COLOR:{
        setHighlightColor(color);
        break;
    }
    case SK_THEME_TEXT_SELECTED_COLOR:{
        setTextColor(CR_SELECTED, color);
        break;
    }
    case SK_THEME_ICON_SELECTED_COLOR:{
        setMenuIconColor(CR_SELECTED, color);
        break;
    }
    case SK_THEME_VIEW_BORDER_SELECTED_COLOR:{
        setActiveWidgetBorderColor(color);
        break;
    }
    case SK_THEME_ASPECT_BG_INTERFACES_COLOR:{
        setAspectBackgroundColor(VA_INTERFACES, color);
        break;
    }
    case SK_THEME_ASPECT_BG_BEHAVIOUR_COLOR:{
        setAspectBackgroundColor(VA_BEHAVIOUR, color);
        break;
    }
    case SK_THEME_ASPECT_BG_ASSEMBLIES_COLOR:{
        setAspectBackgroundColor(VA_ASSEMBLIES, color);
        break;
    }
    case SK_THEME_ASPECT_BG_HARDWARE_COLOR:{
        setAspectBackgroundColor(VA_HARDWARE, color);
        break;
    }
    case SK_THEME_SETTHEME_DARKTHEME:{
        resetTheme(VT_DARK_THEME);
        break;
    }
    case SK_THEME_SETTHEME_LIGHTHEME:{
        resetTheme(VT_LIGHT_THEME);
        break;
    }
    case SK_THEME_SETASPECT_CLASSIC:{
        resetAspectTheme(false);
        break;
    }
    case SK_THEME_SETASPECT_COLORBLIND:{
        resetAspectTheme(true);
        break;
    }case SK_THEME_APPLY:{
        applyTheme();
        break;
    }
    default:
        break;
    }
}

void Theme::calculateImageColor(QString resourceName)
{
    if(imageLookup.contains(resourceName) && !pixmapMainColorLookup.contains(resourceName)){
        QImage image = imageLookup[resourceName];
        if(!image.isNull()){
            int size = 32;
            image = image.scaled(size, size);

            QHash<QRgb, int> colorCount;
            int max = 0;
            QRgb frequentColor;

            int f = 25;

            for(int i =0; i < size * size; i++){
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
            QColor c(frequentColor);
            if(colorCount.isEmpty()){
                c = iconColor;
            }
            pixmapMainColorLookup[resourceName] = c;
        }else{
            pixmapMainColorLookup[resourceName] = QColor(0,0,0,0);
        }
    }
}

QString Theme::getResourceName(QString prefix, QString alias)
{
    //UNCOMMENT FOR TROLLS
    //return ":/Actions/Cage.jpg";
    return prefix % slash % alias;
}

void Theme::setupIcons()
{
    setIconToggledImage("Actions", "Grid_On", "Actions", "Grid_Off");
    setIconToggledImage("Actions", "Fullscreen", "Actions", "Failure");
    setIconToggledImage("Actions", "Minimap", "Actions", "Invisible");
    setIconToggledImage("Actions", "Arrow_Down", "Actions", "Arrow_Up");
    setIconToggledImage("Actions", "SearchOptions", "Actions", "Arrow_Down");
    setIconToggledImage("Actions", "Maximize", "Actions", "Minimize");
    setIconToggledImage("Actions", "Lock_Open", "Actions", "Lock_Closed");
    setIconToggledImage("Actions", "Visible", "Actions", "Invisible");
}


void Theme::resetTheme(VIEW_THEME themePreset)
{
    if (themePreset == VT_DARK_THEME) {
        QColor bgColor = QColor(70,70,70);
        emit changeSetting(SK_THEME_BG_COLOR, bgColor);
        emit changeSetting(SK_THEME_BG_ALT_COLOR, bgColor.lighter());
        emit changeSetting(SK_THEME_TEXT_COLOR, white());
        emit changeSetting(SK_THEME_ICON_COLOR, white());
        emit changeSetting(SK_THEME_BG_DISABLED_COLOR, bgColor.lighter(120));
        emit changeSetting(SK_THEME_TEXT_DISABLED_COLOR, QColor(130,130,130));
        emit changeSetting(SK_THEME_ICON_DISABLED_COLOR, QColor(130,130,130));
        emit changeSetting(SK_THEME_BG_SELECTED_COLOR, QColor(255,165,70));
        emit changeSetting(SK_THEME_TEXT_SELECTED_COLOR, black());
        emit changeSetting(SK_THEME_ICON_SELECTED_COLOR, black());
        emit changeSetting(SK_THEME_VIEW_BORDER_SELECTED_COLOR, black());
        emit changeSetting(SK_THEME_ALTERNATE_TEXT_COLOR, white().darker(150));
    }else if(themePreset == VT_LIGHT_THEME){
        QColor bgColor = QColor(170,170,170);
        emit changeSetting(SK_THEME_BG_COLOR, bgColor);
        emit changeSetting(SK_THEME_BG_ALT_COLOR, bgColor.lighter(130));
        emit changeSetting(SK_THEME_TEXT_COLOR, black());
        emit changeSetting(SK_THEME_ICON_COLOR, black());
        emit changeSetting(SK_THEME_BG_DISABLED_COLOR, bgColor.lighter(110));
        emit changeSetting(SK_THEME_TEXT_DISABLED_COLOR, QColor(130,130,130));
        emit changeSetting(SK_THEME_ICON_DISABLED_COLOR, QColor(130,130,130));
        emit changeSetting(SK_THEME_BG_SELECTED_COLOR, QColor(75,110,175));
        emit changeSetting(SK_THEME_TEXT_SELECTED_COLOR, white());
        emit changeSetting(SK_THEME_ICON_SELECTED_COLOR, white());
        emit changeSetting(SK_THEME_VIEW_BORDER_SELECTED_COLOR, white());
        emit changeSetting(SK_THEME_ALTERNATE_TEXT_COLOR, black().lighter(180));
    }

}

void Theme::resetAspectTheme(bool colorBlind)
{
    if(colorBlind){
        emit changeSetting(SK_THEME_ASPECT_BG_INTERFACES_COLOR, QColor(24,148,184));
        emit changeSetting(SK_THEME_ASPECT_BG_BEHAVIOUR_COLOR, QColor(90,90,90));
        emit changeSetting(SK_THEME_ASPECT_BG_ASSEMBLIES_COLOR, QColor(175,175,175));
        emit changeSetting(SK_THEME_ASPECT_BG_HARDWARE_COLOR, QColor(207,107,100));
    }else{
        //LEGACY
        emit changeSetting(SK_THEME_ASPECT_BG_INTERFACES_COLOR,  QColor(110,210,210));
        emit changeSetting(SK_THEME_ASPECT_BG_BEHAVIOUR_COLOR, QColor(254,184,126));
        emit changeSetting(SK_THEME_ASPECT_BG_ASSEMBLIES_COLOR, QColor(255,160,160));
        emit changeSetting(SK_THEME_ASPECT_BG_HARDWARE_COLOR, QColor(110,170,220));
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
    }else{
        gotAllColors = false;
    }

    valid = gotAllColors;
    themeChanged = true;
}

bool Theme::tintIcon(QString prefix, QString alias)
{
    QString key = prefix % slash % alias;
    if(pixmapSizeLookup.contains(key)){
        return tintIcon(pixmapSizeLookup[key]);
    }
    return false;
}

bool Theme::tintIcon(QSize size)
{
    return size.width() % 96 == 0;
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



