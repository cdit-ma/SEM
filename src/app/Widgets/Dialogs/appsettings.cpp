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

#include "../dataeditwidget.h"
#include "../../theme.h"

#include "../../Controllers/SettingsController/settingscontroller.h"
#include "../../Controllers/SettingsController/setting.h"



#include <QApplication>

#include <QTabWidget>

AppSettings::AppSettings(QWidget *parent):QDialog(parent)
{
    setMinimumSize(600, 400);

    QString title = "Settings";
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

void AppSettings::settingChanged(SETTINGS key, QVariant data)
{
    DataEditWidget* widget = getDataWidget(key);
    if(widget){
        if(widget->getType() != SETTING_TYPE::BUTTON){
            widget->setValue(data);
        }
    }
}


void AppSettings::themeChanged()
{
    Theme* theme = Theme::theme();

    tabWidget->setStyleSheet(theme->getTabbedWidgetStyleSheet() +
                             "QTabBar::tab:selected{ background:" % theme->getPressedColorHex() % "; color:" % theme->getTextColorHex(ColorRole::SELECTED) % ";}"
                             "QTabBar::tab:hover{ background:" % theme->getHighlightColorHex() % ";}");


    toolbar->setStyleSheet(theme->getToolBarStyleSheet());
    warningLabel->setStyleSheet("color: " + theme->getHighlightColorHex() + "; font-weight:bold;");

    setStyleSheet(theme->getWidgetStyleSheet("AppSettings") % theme->getGroupBoxStyleSheet() % theme->getScrollBarStyleSheet() %
                  "#BACKGROUND_WIDGET {background: " % theme->getBackgroundColorHex() % ";}"
                  );
    
    clearSettingsAction->setIcon(theme->getIcon("Icons", "cross"));
    applySettingsAction->setIcon(theme->getIcon("Icons", "tick"));
    toolbar->setIconSize(theme->getIconSize());
    updateLabels();
}

void AppSettings::dataValueChanged(QVariant data)
{
    //Get the sender
    auto widget = qobject_cast<DataEditWidget*>(sender());
    
    if(widget){
        SETTINGS key = widget->property("SETTINGS_KEY").value<SETTINGS>();
        //Apply directly.
        if(widget->getType() == SETTING_TYPE::BUTTON){
            emit setSetting(key, true);
            //APPLY THEME
            if(SettingsController::settings()->isThemeSetting(key)){
                emit setSetting(SETTINGS::THEME_APPLY, true);
            }
        }else{
            QVariant currentValue = SettingsController::settings()->getSetting(key);

            if(currentValue != data){
                changedSettings[key] = data;
                widget->setHighlighted(true);
            }else{
                changedSettings.remove(key);
                widget->setHighlighted(false);
            }
            updateButtons();
        }
    }
}

void AppSettings::applySettings()
{
    bool themeChanged = false;
    bool settingsChanged = false;
    foreach(SETTINGS key, changedSettings.keys()){
        if(!themeChanged && SettingsController::settings()->isThemeSetting(key)){
            themeChanged = true;
        }
        emit setSetting(key, changedSettings[key]);
        settingsChanged = true;
    }

    if(themeChanged){
        emit setSetting(SETTINGS::THEME_APPLY, true);
    }

    if(settingsChanged){
        emit settingsApplied();
    }
}

void AppSettings::clearSettings()
{
    foreach(SETTINGS key, changedSettings.keys()){
        DataEditWidget* widget = getDataWidget(key);
        if(widget){
            widget->setValue(SettingsController::settings()->getSetting(key));
        }
    }
}

void AppSettings::updateButtons()
{
    int count = changedSettings.size();

    applySettingsAction->setEnabled(count > 0);
    clearSettingsAction->setEnabled(count > 0);

    QString prefix = "";
    if(count > 0){
        prefix += " [" + QString::number(count) % "]";
    }

    applySettingsAction->setText("Apply" % prefix);
    clearSettingsAction->setText("Clear" % prefix);
    warningAction->setVisible(SettingsController::settings()->isWriteProtected());
}

DataEditWidget *AppSettings::getDataWidget(SETTINGS key)
{
    return dataEditWidgets.value(key, 0);
}

QString AppSettings::settingKey(Setting* setting){
    return settingKey(setting->getCategory(), setting->getSection());
}

QString AppSettings::settingKey(QString category, QString section){
    return  category + "_" + section;
}   
void AppSettings::updateLabels(){
    auto settings = SettingsController::settings();

    QHash<QString, DataEditWidget*> section_lookups;
    QHash<QString, int> max_sizes;
    for(auto setting : settings->getSettings()){
        auto setting_id = setting->getID();

        auto edit_widget = dataEditWidgets.value(setting_id, 0);
        if(edit_widget){
            auto section_key = settingKey(setting);
            auto current_size = max_sizes[section_key];
            auto required_size = edit_widget->getMinimumLabelWidth();
            if(required_size > current_size){
                max_sizes[section_key] = required_size;
            }
            section_lookups.insertMulti(section_key, edit_widget);
        }
    }

    for(auto key : section_lookups.uniqueKeys()){
        auto size = max_sizes[key];
        for(auto widgets : section_lookups.values(key)){
            widgets->setLabelWidth(size);
        }
    }
}

void AppSettings::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(5);

    tabWidget = new QTabWidget(this);
    //tabWidget->setTabPosition(QTabWidget::West);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setContentsMargins(QMargins(5,5,5,5));

    warningLabel = new QLabel("settings.ini file is read-only! Settings changed won't persist!");
    layout->addWidget(tabWidget, 1);


    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    warningAction = toolbar->addWidget(warningLabel);
    clearSettingsAction = toolbar->addAction("Clear");
    applySettingsAction = toolbar->addAction("Apply");
    //layout->addSpacing(2);
    layout->addWidget(toolbar, 0, Qt::AlignRight);

    connect(applySettingsAction, &QAction::triggered, this, &AppSettings::applySettings);
    connect(clearSettingsAction, &QAction::triggered, this, &AppSettings::clearSettings);



    setupSettingsLayouts();
    updateButtons();
}


void AppSettings::setupSettingsLayouts()
{
    for(auto setting : SettingsController::settings()->getSettings()){
        //Ignore invisible settings.
        if(setting->getType() == SETTING_TYPE::NONE || setting->getType() == SETTING_TYPE::STRINGLIST || setting->getType() == SETTING_TYPE::BYTEARRAY){
            continue;
        }
        auto key = setting->getID();
        auto box = getSectionBox(setting->getCategory(), setting->getSection());
        

        if(!dataEditWidgets.contains(key)){
            DataEditWidget* widget = new DataEditWidget(setting->getName(), setting->getType(), setting->getValue(), this);
            widget->setProperty("SETTINGS_KEY", (uint)key);
            box->addWidget(widget);

            if(setting->gotIcon()){
                auto icon = setting->getIcon();
                widget->setIcon(icon.first, icon.second);
            }

            connect(widget, &DataEditWidget::valueChanged, this, &AppSettings::dataValueChanged);
            dataEditWidgets[key] = widget;
        }
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

        QWidget* widget = new QWidget(area);
        widget->setObjectName("BACKGROUND_WIDGET");
        area->setWidgetResizable(true);
        area->setWidget(widget);

        layout = new QVBoxLayout();
        layout->setSpacing(0);
        layout->setContentsMargins(10,10,10,10);
        widget->setLayout(layout);
        tabWidget->addTab(area, category);
        categoryLayouts[category] = layout;
    }
    return layout;
}


CustomGroupBox *AppSettings::getSectionBox(QString category, QString section){
    auto key = settingKey(category, section);
    CustomGroupBox* box = sectionBoxes.value(key, 0);
    if(!box){
        auto category_layout = getCategoryLayout(category);
        box = new CustomGroupBox(section, category_layout->parentWidget());
        box->setCheckable(false);
        category_layout->addWidget(box);
        sectionBoxes[key] = box;
    }
    return box;
}

