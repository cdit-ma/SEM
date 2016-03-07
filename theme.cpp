#include "theme.h"
#include <QDebug>

Theme* Theme::themeSingleton = 0;

Theme::Theme():QObject(0)
{
}

QColor Theme::getBackgroundColor()
{
    return backgroundColor;
}

QColor Theme::getAltBackgroundColor()
{
    return altBackgroundColor;
}

QColor Theme::getHighlightColor()
{
    return highlightColor;
}

void Theme::setBackgroundColor(QColor color)
{
    backgroundColor = color;
}

void Theme::setAltBackgroundColor(QColor color)
{
    altBackgroundColor = color;
}

void Theme::setHighlightColor(QColor color)
{
    highlightColor = color;
}

QColor Theme::getTextColor(Theme::COLOR_ROLE role)
{
    if(textColor.contains(role)){
        return textColor[role];
    }
    return defaultTextColor;
}

void Theme::setTextColor(Theme::COLOR_ROLE role, QColor color)
{
    textColor[role] = color;
}

QColor Theme::getMenuIconColor(Theme::COLOR_ROLE role)
{
    if(menuIconColor.contains(role)){
        return menuIconColor[role];
    }
    return defaultMenuIconColor;
}

void Theme::setMenuIconColor(Theme::COLOR_ROLE role, QColor color)
{
    menuIconColor[role] = color;
}

void Theme::setIconToggledImage(QString prefix, QString alias, QString toggledAlias, QString toggledImageName)
{
    QString name = prefix + "/" + alias;
    QString toggledName = toggledAlias + "/" + toggledImageName;
    qCritical() << "LOOKUP: " << name << " POINTS TO " << toggledName;
    iconToggledLookup[name] = toggledName;
}

void Theme::setDefaultImageTintColor(QColor color)
{
    iconColor = color;
}

void Theme::setDefaultImageTintColor(QString prefix, QString alias, QColor color)
{
    QString longName = prefix + "/" + alias;
    pixmapTintLookup[longName] = color;
}

void Theme::applyTheme()
{
    pixmapLookup.clear();
    iconLookup.clear();
    emit theme_Changed();
}

QIcon Theme::getIcon(QString prefix, QString alias)
{
    QString lookupName = prefix + "/" + alias;



    if(iconLookup.contains(lookupName)){
        return iconLookup[lookupName];
    }else{
        QIcon icon;
        //Set the default states.
        icon.addPixmap(getImage(prefix, alias, getMenuIconColor(CR_NORMAL)), QIcon::Normal, QIcon::Off);
        icon.addPixmap(getImage(prefix, alias, getMenuIconColor(CR_SELECTED)), QIcon::Active, QIcon::Off);
        icon.addPixmap(getImage(prefix, alias, getMenuIconColor(CR_DISABLED)), QIcon::Disabled, QIcon::Off);

        if(iconToggledLookup.contains(lookupName)){
            QString toggledName = iconToggledLookup[lookupName];
            int midSlash = toggledName.lastIndexOf("/");

            if(midSlash > 0){
                QString toggledPrefixName = toggledName.mid(0, midSlash);
                QString toggledAliasName = toggledName.mid(midSlash + 1);

                //Set the toggled states.
                icon.addPixmap(getImage(toggledPrefixName, toggledAliasName, getMenuIconColor(CR_NORMAL)), QIcon::Normal, QIcon::On);
                icon.addPixmap(getImage(toggledPrefixName, toggledAliasName, getMenuIconColor(CR_SELECTED)), QIcon::Active, QIcon::On);
                icon.addPixmap(getImage(toggledPrefixName, toggledAliasName, getMenuIconColor(CR_DISABLED)), QIcon::Disabled, QIcon::On);
            }
        }

        iconLookup[lookupName] = icon;
        return icon;
    }
}

QPixmap Theme::getImage(QString prefix, QString alias, QColor tintColor)
{
    QString resourceName = prefix + "/" + alias;
    QString lookupName = resourceName;


    //If we have a tint color check for it.
    if(tintColor.isValid()){
        lookupName += "/" + QColorToHex(tintColor);
    }

    if(pixmapLookup.contains(lookupName)){
        return pixmapLookup[lookupName];
    }else{
        QImage image(":/" + resourceName);

        if(!tintColor.isValid()){
            //Check for a non-default color.
            if(pixmapTintLookup.contains(resourceName)){
                tintColor = pixmapTintLookup[resourceName];
            }else{
                //Return the default.
                tintColor = iconColor;
            }
        }

        QPixmap pixmap;
        //Tint the pixmap If it's 96x96
        if(image.size() == QSize(96,96)){
            //Replace the image with it's alphaChannel
            image = image.alphaChannel();

            //Replace the colors with the tinted color.
            for(int i = 0; i < image.colorCount(); ++i) {
                tintColor.setAlpha(qGray(image.color(i)));
                image.setColor(i, tintColor.rgba());
            }
            pixmap = QPixmap::fromImage(image);
        }else{
            pixmap = QPixmap::fromImage(image);
        }

        pixmapLookup[lookupName] = pixmap;
        return pixmap;
    }
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

