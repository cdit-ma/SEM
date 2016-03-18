#include "theme.h"
#include <QDebug>
#include <QStringBuilder>
#include <QDirIterator>
#include <QDateTime>
Theme* Theme::themeSingleton = 0;
QThread* Theme::themeThread = 0;

Theme::Theme():QObject(0)
{
    slash = QString("/");
    updateValid();
    valid = false;
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
    return highlightColor;
}

QString Theme::getPressedColorHex()
{
    QColor color = getPressedColor();
    return Theme::QColorToHex(color);
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
    long long memSize=0;
    foreach(QPixmap pix, pixmapLookup){
        memSize += pix.width() * pix.height() * 4;
    }


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

QIcon Theme::getIcon(QString prefix, QString alias)
{
    //qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QString lookupName = prefix  % slash %  alias;

    if(iconLookup.contains(lookupName)){
        return iconLookup[lookupName];
    }else{
        QIcon icon;

        bool isTinted = tintIcon(prefix, alias);
        //Set the default states.
        icon.addPixmap(getImage(prefix, alias, QSize(), getMenuIconColor(CR_NORMAL)), QIcon::Normal, QIcon::Off);
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

                bool isTinted2 = tintIcon(toggledPrefixName, toggledAliasName);

                //Set the toggled states.
                icon.addPixmap(getImage(toggledPrefixName, toggledAliasName, QSize(), getMenuIconColor(CR_NORMAL)), QIcon::Normal, QIcon::On);
                if(isTinted2){
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
    //Calculate the name of the image.

    QString resourceName = prefix % slash % alias;
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
        int factor = 1;
        //Bitshift round to power of 2
        while((factor <<= 1 ) < size.width()>>1);

        //Scale the request image size to a width = factor
        int newWidth = factor;
        int newHeight = (double) factor * ((double)size.width() / (double)size.height());
        size.setWidth(newWidth);
        size.setHeight(newHeight);

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

        //Load the original Image
        QImage image(":/" % resourceName);

        if(image.isNull()){
            qCritical() << "NULL IMAGE " << lookupName;
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

void Theme::preloadImages()
{
    qint64 timeStart = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QStringList dirs;
    dirs << "Actions" << "Data" << "Functions" << "Items" << "Welcome";
    foreach(QString dir, dirs){
        QDirIterator it(":/" % dir, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            it.next();
            QString imageName = it.fileName();
            getImage(dir, imageName);
        }
    }
    //Only Allow once.
    disconnect(this, SIGNAL(initPreloadImages()), this, SLOT(preloadImages()));
    qint64 timeFinish = QDateTime::currentDateTime().toMSecsSinceEpoch();
    qCritical() << "PRELOADING IMAGES TIME: " <<  timeFinish-timeStart;
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

QString Theme::QColorToHex(const QColor color)
{
    return color.name(QColor::HexArgb);
}

Theme *Theme::theme()
{
    if(!themeSingleton){
        themeThread = new QThread();
        themeThread->start();

        themeSingleton = new Theme();
        connect(themeSingleton, SIGNAL(initPreloadImages()), themeSingleton, SLOT(preloadImages()));
        connect(themeSingleton, SIGNAL(destroyed(QObject*)), themeThread, SIGNAL(finished()));
        //Move the JenkinsRequest to the thread
        themeSingleton->moveToThread(themeThread);
    }
    return themeSingleton;
}

void Theme::teardownTheme()
{
    delete themeSingleton;
    themeSingleton = 0;
}

