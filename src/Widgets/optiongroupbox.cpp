#include "optiongroupbox.h"
#include "../theme.h"
#include <QDebug>

#define OPTION_KEY "OPTION_KEY"
#define ICON_SIZE 20


/**
 * @brief OptionGroupBox::OptionGroupBox
 * @param title
 * @param parent
 */
OptionGroupBox::OptionGroupBox(QString title, QWidget* parent) : CustomGroupBox(title, parent)
{
    setupResetAction();
    setTitle(title);
    setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &OptionGroupBox::themeChanged);
    themeChanged();
}


/**
 * @brief OptionGroupBox::getCheckedKeys
 * @return
 */
QList<QVariant> OptionGroupBox::getCheckedKeys()
{
    return checkedKeys;
}


/**
 * @brief OptionGroupBox::isAllChecked
 * @return
 */
bool OptionGroupBox::isAllChecked()
{
    return checkedKeys.count() == actions_lookup.count();
}


/**
 * @brief updateIcon
 * @param button
 * @param theme
 */
void updateIcon(QAction* action, Theme* theme = Theme::theme())
{
    if (action) {
        auto icon_path = action->property("iconPath").toString();
        auto icon_name = action->property("iconName").toString();
        action->setIcon(theme->getIcon(icon_path, icon_name));
    }
}


/**
 * @brief OptionGroupBox::themeChanged
 */
void OptionGroupBox::themeChanged()
{
    auto theme = Theme::theme();    
    for (auto action : actions_lookup.values()) {
        updateIcon(action, theme);
    }
    updateIcon(reset_action, theme);
}


/**
 * @brief OptionGroupBox::setExclusive
 * This sets whether this group is exclusive or not.
 * Being exclusive means that there is only ever one filter selected in this group at one time.
 * @param exclusive
 */
void OptionGroupBox::setExclusive(bool exclusive)
{
    this->exclusive = exclusive;
}


/**
 * @brief OptionGroupBox::setResetButtonVisible
 * @param visible
 */
void OptionGroupBox::setResetButtonVisible(bool visible)
{
    if (reset_action) {
        reset_action->setVisible(visible);
    }
}

void OptionGroupBox::setOptionVisible(QVariant key, bool visible)
{
    auto option_button = actions_lookup.value(key, 0);
    if(option_button){
        option_button->setVisible(visible);
    }
}



/**
 * @brief OptionGroupBox::setResetButtonIcon
 * @param path
 * @param name
 */
void OptionGroupBox::setResetButtonIcon(QString path, QString name)
{
    if (reset_action) {
        reset_action->setProperty("iconPath", path);
        reset_action->setProperty("iconName", name);
        updateIcon(reset_action);
    }
}


/**
 * @brief OptionGroupBox::setResetButtonText
 * @param text
 */
void OptionGroupBox::setResetButtonText(QString text)
{
    if (reset_action) {
        reset_action->setText(text);
    }
}


/**
 * @brief OptionGroupBox::removeOption
 * @param key
 */
void OptionGroupBox::removeOption(QVariant key)
{
    auto action = actions_lookup.value(key, 0);
    if (action) {
        actions_lookup.remove(key);
        checkedKeys.removeAll(key);
        delete action;
    }
    updateTitleCount();
}


/**
 * @brief OptionGroupBox::removeOptions
 */
void OptionGroupBox::removeOptions()
{
    hide();
    for (auto action : actions_lookup.values()) {
        delete action;
    }
    actions_lookup.clear();
    resetOptions();
    show();
}


void OptionGroupBox::reset(){
    resetOptions();
    emit checkedOptionsChanged();
}

/**
 * @brief OptionGroupBox::uncheckOptions
 */
void OptionGroupBox::uncheckOptions()
{
    for (auto action : actions_lookup.values()) {
        action->setChecked(false);
    }
    reset_action->setChecked(false);
    checkedKeys.clear();
}


/**
 * @brief OptionGroupBox::resetOptions
 */
void OptionGroupBox::resetOptions()
{
    uncheckOptions();
    reset_action->setChecked(true);
    checkedKeys = actions_lookup.keys();
    updateTitleCount();
    
}


/**
 * @brief OptionGroupBox::setTitle
 * @param title
 */
void OptionGroupBox::setTitle(QString title)
{
    this->title = title;
    updateTitleCount();
}


/**
 * @brief OptionGroupBox::updateTitleCount
 */
void OptionGroupBox::updateTitleCount()
{
    auto new_title = title;
    if (reset_action && !reset_action->isChecked()) {
        new_title += QString(" (%1)").arg(QString::number(checkedKeys.count()));
    }
    CustomGroupBox::setTitle(new_title);
}


/**
 * @brief OptionGroupBox::filterTriggered
 */
void OptionGroupBox::optionToggled()
{
    auto action = qobject_cast<QAction*>(sender());

    // handle "All" and exclusive case
    if (action == reset_action) {
        resetOptions();
    } else {
        bool checked = action->isChecked();
        auto key = action->property(OPTION_KEY);
        if (!checked) {
            checkedKeys.removeAll(key);
        } else {
            if (exclusive) {
                //Uncheck everything
                uncheckOptions();
                action->setChecked(true);
            } else {
                if (reset_action->isChecked()) {
                    uncheckOptions();
                }
                action->setChecked(true);
            }
            checkedKeys.append(key);
        }
        if (checkedKeys.isEmpty()) {
            resetOptions();
        }
    }

    updateTitleCount();
    emit checkedOptionsChanged();
}


/**
 * @brief OptionGroupBox::setupResetButton
 */
void OptionGroupBox::setupResetAction()
{
    if(!reset_action){
        reset_action = getNewOptionAction();
        reset_action->setText("All");
        reset_action->setProperty("iconPath", "Icons");
        reset_action->setProperty("iconName", "list");
        reset_action->setChecked(true);
    }
}


/**
 * @brief OptionGroupBox::addOption
 * @param key
 * @param label
 * @param icon_path
 * @param icon_name
 * @return
 */
bool OptionGroupBox::addOption(QVariant key, QString label, QString icon_path, QString icon_name)
{
    if (key.isNull() || actions_lookup.count(key)) {
        qWarning() << "OptionGroupBox::addOption - The key is null.";
        return false;
    }

    auto option_action = getNewOptionAction();
    option_action->setText(label);
    option_action->setProperty("iconPath", icon_path);
    option_action->setProperty("iconName", icon_name);
    option_action->setProperty(OPTION_KEY, key);
    actions_lookup.insert(key, option_action);
    updateIcon(option_action);
    resetOptions();
    return true;
}

QAction* OptionGroupBox::getNewOptionAction(){
    auto button = new QToolButton(this);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    auto action = addWidget(button);
    //Connect the button to it's action so we don't need to worry about QToolButton stuff
    button->setDefaultAction(action);
    action->setCheckable(true);
    action->setChecked(false);
    connect(action, &QAction::triggered, this, &OptionGroupBox::optionToggled);
    return action;
}


inline uint qHash(QVariant key, uint seed)
{
    return ::qHash(key.toUInt(), seed);
}
