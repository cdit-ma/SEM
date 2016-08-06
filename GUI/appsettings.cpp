#include "appsettings.h"

#include <QWidget>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>
#include <QPushButton>
#include <QLineEdit>
#include <QDialog>
#include <QMessageBox>
#include <QStringBuilder>
#include "../Widgets/New/dataeditwidget.h"

#define SETTINGS_WIDTH 600
#define SETTINGS_HEIGHT 400

#include "../enumerations.h"
#include "../Controller/settingscontroller.h"
#include "../View/theme.h"

#include <QApplication>

#include <QTabWidget>

AppSettings::AppSettings(QWidget *parent):QDialog(parent)
{
    setMinimumWidth(SETTINGS_WIDTH);
    setMinimumHeight(SETTINGS_HEIGHT);
    setModal(true);

    QString title = "App Settings";
    setWindowTitle(title);
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));
    setModal(true);

    setupLayout();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}


AppSettings::~AppSettings()
{
}

QVariant AppSettings::getSetting(QString)
{
    return QVariant();
}

void AppSettings::setSetting(QString, QVariant)
{

}

void AppSettings::themeChanged()
{
    Theme* theme = Theme::theme();
    tabWidget->setStyleSheet(theme->getTabbedWidgetStyleSheet());

    setStyleSheet(theme->getWidgetStyleSheet() %
                  "QGroupBox {background: " % theme->getBackgroundColorHex() % "; color: " % theme->getTextColorHex() + "; border:0;padding:0px;margin:0px;}"
                  "QGroupBox > QGroupBox {border: 1px solid " % theme->getAltBackgroundColorHex() % ";border-radius: 4px; padding:10px;margin-top:8px;}"
                  "QGroupBox::title {subcontrol-position: top center;subcontrol-origin: margin; padding: 2px; font-weight:bold;}"
                  "KeyEditWidget {color: " % theme->getTextColorHex() % ";}"
                  );


    toolbar->setStyleSheet(theme->getToolBarStyleSheet());

    warningLabel->setStyleSheet("color: " + theme->getHighlightColorHex() + "; font-weight:bold;");

    setWindowIcon(theme->getImage("Actions", "Settings"));
}

void AppSettings::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->setContentsMargins(0,0,0,0);


    tabWidget = new QTabWidget(this);
    tabWidget->setContentsMargins(QMargins(0,0,0,0));

    warningLabel = new QLabel("settings.ini file is read-only! Settings changed won't persist!");
    layout->addWidget(warningLabel, 0, Qt::AlignCenter);
    layout->addWidget(tabWidget, 1);


    toolbar = new QToolBar(this);

    clearSettings = toolbar->addAction("Clear");
    applySettings = toolbar->addAction("Apply");
    layout->addWidget(toolbar, 0, Qt::AlignRight);

    setupSettingsLayouts();
}


void AppSettings::setupSettingsLayouts()
{
    foreach(Setting* setting, SettingsController::settings()->getSettings()){
        QString category = setting->getCategory();
        QString section = setting->getSection();

        QVBoxLayout* layout = getSectionLayout(category, section);

        QString customType = "";
        if(setting->getType() == ST_COLOR){
            customType = "Color";
        }else if(setting->getType() == ST_FILE){
            customType = "File";
        }else if(setting->getType() == ST_FILE){
            customType = "Path";
        }

        DataEditWidget* dataEditWidget = new DataEditWidget(setting->getSettingString(), setting->getName(), setting->getType(), setting->getValue(),this);
        //KeyEditWidget* keyEditWidget = new KeyEditWidget(category, setting->getSettingString(), setting->getName(), setting->getValue(),"", customType);
        layout->addWidget(dataEditWidget);
    }
    foreach(QString category, categoryLayouts.keys()){
        getCategoryLayout(category)->addStretch(1);
    }
}

QVBoxLayout *AppSettings::getCategoryLayout(QString category)
{
    QVBoxLayout* layout = 0;

    if(categoryLayouts.contains(category)){
        layout = categoryLayouts[category];
    }else{
        QScrollArea* area = new QScrollArea(tabWidget);
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        QGroupBox* widget = new QGroupBox(area);
        area->setWidgetResizable(true);
        area->setWidget(widget);

        layout = new QVBoxLayout();
        widget->setLayout(layout);
        tabWidget->addTab(area, category);
        categoryLayouts[category] = layout;
    }
    return layout;
}

QVBoxLayout *AppSettings::getSectionLayout(QString category, QString section)
{
    QString key = category + "_" + section;

    QVBoxLayout* layout = 0;
    if(sectionLayouts.contains(key)){
        layout = sectionLayouts[key];
    }else{
        QVBoxLayout* cLayout = getCategoryLayout(category);

        QGroupBox* groupBox = new QGroupBox(section, cLayout->parentWidget());
        cLayout->addWidget(groupBox);
        layout = new QVBoxLayout();
        groupBox->setLayout(layout);
        sectionLayouts[key] = layout;
    }
    return layout;
}
