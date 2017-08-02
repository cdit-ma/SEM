#include "filtergroup.h"

#include <QDebug>

#define FILTER_KEY "filterKey"
#define FILTER_GROUP "filterGroup"

#define DEFAULT_CHECKED_STATE true
#define ICON_SIZE 30

/*
 * NOTE:    When setting tool button's text or icon, apply it to the button.
 *          When hiding a tool button, set the visibility of its action.
 */

/**
 * @brief FilterGroup::FilterGroup
 * @param title
 * @param groupKey
 * @param parent
 */
FilterGroup::FilterGroup(QString title, QVariant groupKey, QObject* parent) : QObject(parent)
{
    if (!groupKey.isValid()) {
        groupKey = title;
    }

    filterGroupTitle = title;
    filterGroupKey = groupKey;
    setProperty(FILTER_GROUP, filterGroupKey);

    customFilterBox = 0;
    resetAction = 0;
    resetButton = 0;

    resetKey = "All";

    showResetButton = true;
    exclusive = false;

    // construct and add reset ("All") button
    setupResetButton();
    //setResetButtonVisible(false);
}


/**
 * @brief FilterGroup::constructFilterBox
 * @return
 */
CustomGroupBox* FilterGroup::constructFilterBox()
{
    if (!customFilterBox) {
        customFilterBox = new CustomGroupBox(filterGroupTitle);
    }
    foreach (QAbstractButton* button, filterButtonsHash.values()) {
        addToFilterBox(button);
    }
    resetFilters();
    return customFilterBox;
}


/**
 * @brief FilterGroup::getFilterGroupKey
 * @return
 */
QVariant FilterGroup::getFilterGroupKey()
{
    return filterGroupKey;
}


/**
 * @brief FilterGroup::getResetKey
 * @return
 */
QVariant FilterGroup::getResetKey()
{
    return resetKey;
}


/**
 * @brief FilterGroup::getFilterKeys
 * @return
 */
QList<QVariant> FilterGroup::getFilterKeys()
{
    return filterButtonsHash.keys();
}


/**
 * @brief FilterGroup::getCheckedFilterKeys
 * @return
 */
QList<QVariant> FilterGroup::getCheckedFilterKeys()
{
    return checkedKeys;
}


/**
 * @brief FilterGroup::setExclusive
 * This sets whether this group is exclusive or not.
 * Being exclusive means that there is only ever one filter selected in this group at one time.
 * @param exclusive
 */
void FilterGroup::setExclusive(bool exclusive)
{
    this->exclusive = exclusive;
}


/**
 * @brief FilterGroup::setVisible
 * @param visible
 */
void FilterGroup::setVisible(bool visible)
{
    if (customFilterBox) {
        customFilterBox->setVisible(visible);
    } else {
        foreach (QAbstractButton* button, filterButtonsHash.values()) {
            button->setVisible(visible);
        }
    }
}


/**
 * @brief FilterGroup::setResetButtonVisible
 * @param visible
 */
void FilterGroup::setResetButtonVisible(bool visible)
{
    if (visible == showResetButton) {
        return;
    }

    bool showButton = visible;
    if (customFilterBox) {
        showButton = showButton && customFilterBox->isChecked();
    }
    if (resetAction) {
        resetAction->setVisible(showButton);
    } else if (resetButton) {
        resetButton->setVisible(showButton);
    }

    showResetButton = showButton;
}


/**
 * @brief FilterGroup::setResetButtonIcon
 * @param path
 * @param name
 */
void FilterGroup::setResetButtonIcon(QString path, QString name)
{
    if (resetButton) {
        resetButton->setProperty("iconPath", path);
        resetButton->setProperty("iconName", name);
        resetButton->setIcon(Theme::theme()->getIcon(path, name));
    }
}


/**
 * @brief FilterGroup::setResetButtonText
 * @param text
 */
void FilterGroup::setResetButtonText(QString text)
{
    if (resetButton) {
        resetButton->setText(text);
    }
}


/**
 * @brief FilterGroup::setResetButtonKey
 * @param key
 */
void FilterGroup::setResetButtonKey(QVariant key)
{
    // remove reset key from hash
    filterButtonsHash.remove(resetKey);

    // re-store reset button with the new key
    filterButtonsHash[key] = resetButton;

    // update reset button's filter key property
    resetKey = key;
    resetButton->setProperty(FILTER_KEY, key);
}


/**
 * @brief FilterGroup::addFilterToolButton
 * @param key
 * @param label
 * @param iconPath
 * @param iconName
 */
void FilterGroup::addFilterToolButton(QVariant key, QString label, QString iconPath, QString iconName)
{
    if (filterButtonsHash.contains(key)) {
        return;
    }
    QToolButton* button = new QToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    addFilterButton(button, key, label, iconPath, iconName);
}


/**
 * @brief FilterGroup::addFilterPushButton
 * @param key
 * @param label
 * @param iconPath
 * @param iconName
 */
void FilterGroup::addFilterPushButton(QVariant key, QString label, QString iconPath, QString iconName)
{
    if (filterButtonsHash.contains(key)) {
        return;
    }
    QPushButton* button = new QPushButton();
    addFilterButton(button, key, label, iconPath, iconName);
}


/**
 * @brief FilterGroup::addToFilterGroup
 * @param key
 * @param filterButton
 * @param addToGroupBox
 */
void FilterGroup::addToFilterGroup(QVariant key, QAbstractButton* filterButton, bool addToGroupBox)
{
    if (filterButtonsHash.contains(key)) {
        return;
    }

    if (filterButton) {

        filterButtonsHash[key] = filterButton;
        filterButton->setProperty(FILTER_KEY, key);
        filterButton->setProperty(FILTER_GROUP, filterGroupKey);
        filterButton->setCheckable(true);
        filterButton->setChecked(false);
        connect(filterButton, SIGNAL(clicked(bool)), this, SLOT(filterTriggered()));

        // initially have the reset ("All") button checked
        if (filterButton == resetButton) {
            filterButton->setChecked(true);
            checkedKeys.append(key);
        }

        if (addToGroupBox) {
            //addToGroupBox(filterButton);
            addToFilterBox(filterButton);
        }
    }
}


/**
 * @brief FilterGroup::clearFilterGroup
 */
void FilterGroup::clearFilterGroup()
{
    QList<QVariant> keys = filterButtonsHash.keys();
    foreach (QVariant key, keys) {
        // don't delete the reset button
        if (key == resetKey) {
            continue;
        }
        if (filterButtonsHash.contains(key)) {
            checkedKeys.removeAll(key);
            delete filterButtonsHash.take(key);
        }
    }
    resetFilters();
}


/**
 * @brief FilterGroup::filterTriggered
 */
void FilterGroup::filterTriggered()
{
    QAbstractButton* button = qobject_cast<QAbstractButton*>(sender());
    if (button) {

        QList<QVariant> prevCheckedKeys = checkedKeys;
        QVariant key = button->property(FILTER_KEY);

        if (button->isChecked()) {

            // reset this filter group
            if (key == resetKey) {
                resetFilters();
                return;
            }

            // if this filter group is exclusive, only one button can be checked at any time
            // clear previously checked buttons then re-check the triggered button
            if (exclusive) {
                clearFilters();
                button->setChecked(true);
            } else {
                // if any other button is checked, make sure the reset button is unchecked
                if (resetButton && resetButton->isChecked()) {
                    resetButton->setChecked(false);
                    checkedKeys.removeAll(resetKey);
                }
            }

            // add the button's key to the checked keys list
            checkedKeys.append(key);

        } else {

            // the reset ("All") button cannot be unchecked by clicking on it
            if (key == resetKey) {
                button->setChecked(true);
                return;
            }

            // if un-checking the last checked button, check the reset ("All") button
            // there has to be one filter button checked at all times
            if (checkedKeys.size() == 1) {
                resetFilters();
                return;
            }

            // remove the button's key from the checked keys list
            checkedKeys.removeAll(key);
        }

        // send a signal if the checked keys list has changed
        if (prevCheckedKeys != checkedKeys) {
            updateFilterCheckedCount();
            emit filtersChanged(checkedKeys);
        }
    }
}


/**
 * @brief FilterGroup::resetFilters
 * This un-checks all currently checked buttons then re-checks the reset ("All") button.
 * It updates the checked count in the title and sends a signal with a clear checked list.
 */
void FilterGroup::resetFilters()
{
    // clear the filters then re-check the "All" button
    clearFilters();

    if (resetButton) {
        resetButton->setChecked(true);
    } else {
    }

    // update the checked count and send signals that the filters have changed
    updateFilterCheckedCount();
    emit filtersChanged(checkedKeys);
    emit filtersCleared();
}


/**
 * @brief FilterGroup::setupResetButton
 */
void FilterGroup::setupResetButton()
{
    resetButton = new QToolButton();
    resetButton->setText("All");
    resetButton->setProperty("iconPath", "Icons");
    resetButton->setProperty("iconName", "list");
    resetButton->setIcon(Theme::theme()->getIcon("Icons", "list"));
    resetButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    resetButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    addToFilterGroup(resetKey, resetButton, false);
}


/**
 * @brief FilterGroup::addFilterButton
 * @param button
 * @param key
 * @param label
 * @param iconPath
 * @param iconName
 */
void FilterGroup::addFilterButton(QAbstractButton* button, QVariant key, QString label, QString iconPath, QString iconName)
{
    // set default icon
    if (iconPath.isEmpty() || iconName.isEmpty()) {
        iconPath = "Icons";
        iconName = "circleQuestion";
    }

    button->setText(label);
    button->setProperty("iconPath", iconPath);
    button->setProperty("iconName", iconName);
    button->setIcon(Theme::theme()->getIcon(iconPath, iconName));
    button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    addToFilterGroup(key, button);
}


/**
 * @brief FilterGroup::addToFilterBox
 * This adds/inserts the provided filter button to the toolbar in the filter group box.
 * It makes sure that the reset filter button is at the top of the toolbar.
 * @param button
 */
void FilterGroup::addToFilterBox(QAbstractButton* button)
{
    if (customFilterBox) {
        if (button == resetButton) {
            QAction* topAction = customFilterBox->getTopAction();
            QAction* action = 0;
            if (topAction) {
                action = customFilterBox->insertWidget(topAction, button);
            } else {
                action = customFilterBox->addWidget(button);
            }
            resetAction = action;
            resetAction->setVisible(showResetButton);
        } else {
            customFilterBox->addWidget(button);
        }
    }
}


/**
 * @brief FilterGroup::clearFilters
 * This un-checks all currently checked buttons.
 */
void FilterGroup::clearFilters()
{
    foreach (QVariant key, checkedKeys) {
        QAbstractButton* button = filterButtonsHash.value(key, 0);
        if (button) {
            button->setChecked(false);
        }
    }
    checkedKeys.clear();
}


/**
 * @brief FilterGroup::updateFilterCheckedCount
 * This is called whenever the filters checked list is changed.
 * It updates the checked filters count in the group box title.
 */
void FilterGroup::updateFilterCheckedCount()
{
    if (customFilterBox && resetButton) {
        QString newTitle = filterGroupTitle;
        if (!resetButton->isChecked()) {
            newTitle = filterGroupTitle + " (" + QString::number(checkedKeys.count()) + ")";
        }
        customFilterBox->setTitle(newTitle);
    }
}
