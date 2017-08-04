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
    setupResetButton();
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
    return checkedKeys.count() == optionsLookup.count();
}


/**
 * @brief updateIcon
 * @param button
 * @param theme
 */
void updateIcon(QToolButton* button, Theme* theme = Theme::theme())
{
    if (button) {
        auto icon_path = button->property("iconPath").toString();
        auto icon_name = button->property("iconName").toString();
        button->setIcon(theme->getIcon(icon_path, icon_name));
    }
}


/**
 * @brief OptionGroupBox::themeChanged
 */
void OptionGroupBox::themeChanged()
{
    auto theme = Theme::theme();    
    for (auto button : optionsLookup.values()) {
        updateIcon(button, theme);
    }
    updateIcon(resetButton, theme);
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
    if (resetAction) {
        resetAction->setVisible(visible);
    }
}


/**
 * @brief OptionGroupBox::setResetButtonIcon
 * @param path
 * @param name
 */
void OptionGroupBox::setResetButtonIcon(QString path, QString name)
{
    if (resetButton) {
        resetButton->setProperty("iconPath", path);
        resetButton->setProperty("iconName", name);
        resetButton->setIcon(Theme::theme()->getIcon(path, name));
    }
}


/**
 * @brief OptionGroupBox::setResetButtonText
 * @param text
 */
void OptionGroupBox::setResetButtonText(QString text)
{
    if (resetButton) {
        resetButton->setText(text);
    }
}


/**
 * @brief OptionGroupBox::addToFilterGroup
 * @param key
 * @param filterButton
 * @param addToGroupBox
 */
QAction* OptionGroupBox::addOptionButton(QToolButton* button, QVariant key)
{
    if (button) {
        if (!key.isNull()) {
            if (optionsLookup.contains(key)) {
                return 0;
            } else {
                optionsLookup[key] = button;
            }
        }

        //Add the Option key as a property
        button->setProperty(OPTION_KEY, key);
        button->setCheckable(true);
        button->setChecked(false);
        connect(button, &QToolButton::clicked, this, &OptionGroupBox::optionToggled);
        updateIcon(button);
        
        return addWidget(button);
    }
    return 0;
}


/**
 * @brief OptionGroupBox::removeOption
 * @param key
 */
void OptionGroupBox::removeOption(QVariant key)
{
    auto button = optionsLookup.value(key, 0);
    if (button) {
        optionsLookup.remove(key);
        checkedKeys.removeAll(key);
        delete button;
    }
    updateTitleCount();
}


/**
 * @brief OptionGroupBox::removeOptions
 */
void OptionGroupBox::removeOptions()
{
    hide();
    for (auto val : optionsLookup.values()) {
        delete val;
    }
    optionsLookup.clear();
    resetOptions();
    show();
}


/**
 * @brief OptionGroupBox::uncheckOptions
 */
void OptionGroupBox::uncheckOptions()
{
    for (auto button : optionsLookup.values()) {
        button->setChecked(false);
    }
    resetButton->setChecked(false);
    checkedKeys.clear();
}


/**
 * @brief OptionGroupBox::resetOptions
 */
void OptionGroupBox::resetOptions()
{
    uncheckOptions();
    resetButton->setChecked(true);
    checkedKeys = optionsLookup.keys();
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
    if (resetButton && !resetButton->isChecked()) {
        new_title += QString(" (%1)").arg(QString::number(checkedKeys.count()));
    }
    CustomGroupBox::setTitle(new_title);
}


/**
 * @brief OptionGroupBox::filterTriggered
 */
void OptionGroupBox::optionToggled()
{
    auto button = qobject_cast<QToolButton*>(sender());

    // handle "All" and exclusive case
    if (button == resetButton) {
        resetOptions();
    } else {
        bool checked = button->isChecked();
        auto key = button->property(OPTION_KEY);
        if (!checked) {
            checkedKeys.removeAll(key);
        } else {
            if (exclusive) {
                //Uncheck everything
                uncheckOptions();
                button->setChecked(true);
            } else {
                if (resetButton->isChecked()) {
                    uncheckOptions();
                }
                button->setChecked(true);
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
void OptionGroupBox::setupResetButton()
{
    resetButton = new QToolButton(this);
    resetButton->setText("All");
    resetButton->setProperty("iconPath", "Icons");
    resetButton->setProperty("iconName", "list");
    resetButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    resetButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    resetAction = addOptionButton(resetButton);
    resetButton->setChecked(true);
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
    if (key.isNull()) {
        qWarning() << "OptionGroupBox::addOption - The key is null.";
        return false;
    }

    auto button = new QToolButton(this);
    button->setText(label);
    button->setProperty("iconPath", icon_path);
    button->setProperty("iconName", icon_name);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    if (addOptionButton(button, key)) {
        resetOptions();
        return true;
    }
    return false;
}


inline uint qHash(QVariant key, uint seed)
{
    return ::qHash(key.toUInt(), seed);
}
