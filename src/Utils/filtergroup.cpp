#include "filtergroup.h"
#include "../theme.h"

#include <QDebug>

#define FILTER_KEY "filterKey"
#define FILTER_GROUP "filterGroup"
#define FILTER_RESET_KEY "All"
#define DEFAULT_CHECKED_STATE true
#define ICON_SIZE 30

/*
 * NOTE:    When setting tool button's text or icon, apply it to the button.
 *          When hiding a tool buttonw, set the visibility of its action.
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

    filterGroupBox = 0;
    filterToolbar = 0;
    exclusive = false;

    resetAction = 0;
    resetButton = 0;
    resetKey = FILTER_RESET_KEY;
    showResetButton = true;

    // construct and add reset ("All") button - this is visible by default
    setupResetButton();
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
    return FILTER_RESET_KEY;
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
 * @brief FilterGroup::constructFilterGroupBox
 * @param orientation
 * @return
 */
QGroupBox* FilterGroup::constructFilterGroupBox(Qt::Orientation orientation)
{
    if (filterGroupBox) {
        return filterGroupBox;
    }

    filterGroupBox = new QGroupBox(filterGroupTitle);
    filterGroupBox->setCheckable(true);
    filterGroupBox->setAlignment(Qt::AlignHCenter);

    filterToolbar = new QToolBar();
    filterToolbar->setOrientation(orientation);
    filterToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    filterToolbar->setIconSize(QSize(ICON_SIZE, ICON_SIZE));

    foreach (QAbstractButton* button, filterButtonsHash.values()) {
        addToGroupBox(button);
    }

    // set the groupbox's initial checked state - this sets the filter buttons initial visibilty state
    filterGroupBox->setChecked(DEFAULT_CHECKED_STATE);
    filterGroupBox->toggled(DEFAULT_CHECKED_STATE);

    QLayout* layout = new QVBoxLayout(filterGroupBox);
    layout->setMargin(0);
    layout->setContentsMargins(0,10,0,7);
    layout->addWidget(filterToolbar);

    connect(filterGroupBox, SIGNAL(toggled(bool)), filterToolbar, SLOT(setVisible(bool)));
    connect(Theme::theme(), &Theme::theme_Changed, this, &FilterGroup::themeChanged);
    themeChanged();

    return filterGroupBox;
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
    if (filterGroupBox) {
        filterGroupBox->setVisible(visible);
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
    showResetButton = visible;
    updateResetButtonVisibility();
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
    // remove reset button from hash
    filterButtonsHash.remove(key);

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
        qWarning() << "FilterGroup::addFilterToolButton - A filter with key [" << key.toString() << "] already exists.";
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
        qWarning() << "FilterGroup::addFilterPushButton - A filter with key [" << key.toString() << "] already exists.";
        return;
    }
    QPushButton* button = new QPushButton();
    addFilterButton(button, key, label, iconPath, iconName);
}


/**
 * @brief FilterGroup::addToFilterGroup
 * @param key
 * @param filterButton
 */
void FilterGroup::addToFilterGroup(QVariant key, QAbstractButton* filterButton)
{
    if (filterButtonsHash.contains(key)) {
        qWarning() << "FilterGroup::addToFilterGroup - A filter with key [" << key.toString() << "] already exists.";
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

        addToGroupBox(filterButton);
    }
}


/**
 * @brief FilterGroup::removeFilter
 * @param key
 */
void FilterGroup::removeFilter(QVariant key)
{
    if (filterButtonsHash.contains(key)) {
        // don't delete the reset button
        if (key == FILTER_RESET_KEY) {
            return;
        }
        // remove the button from the hash and the checked list
        QAbstractButton* button = filterButtonsHash.take(key);
        checkedKeys.removeAll(key);
        delete button;
    }
}


/**
 * @brief FilterGroup::themeChanged
 */
void FilterGroup::themeChanged()
{
    Theme* theme = Theme::theme();

    if (filterGroupBox) {
        filterGroupBox->setStyleSheet("QGroupBox{ color:" + theme->getTextColorHex() + "; margin-top: 5px; border: none; border-top: 4px solid " + theme->getAltBackgroundColorHex() + ";}"
                                      "QGroupBox::title{ subcontrol-origin: margin; subcontrol-position: top center; margin-top: 0px; padding: -2px 6px 2px -16px; }"
                                      "QGroupBox::title::hover{ color:" + theme->getHighlightColorHex() + ";}"
                                      "QGroupBox::indicator:checked{ image: none; }"
                                      "QGroupBox::indicator:unchecked{ image: none; }"
                                      /*
                                      "QGroupBox::title{ subcontrol-origin: margin; subcontrol-position: top center; padding: 0px 3px 0px 3px; }"
                                      "QGroupBox::indicator:checked{ image: url(:/Icons/arrowHeadDown); }"
                                      "QGroupBox::indicator:unchecked{ image: url(:/Icons/arrowHeadUp); }"
                                      */
                                      );
    }

    if (filterToolbar) {
        filterToolbar->setStyleSheet(theme->getToolBarStyleSheet() + "QToolButton{ border-radius:" + theme->getSharpCornerRadius() + ";}");
    }

    foreach (QAbstractButton* button, filterButtonsHash.values()) {
        QString iconPath = button->property("iconPath").toString();
        QString iconName = button->property("iconName").toString();
        button->setIcon(theme->getIcon(iconPath, iconName));
    }
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

            // if this filter group is exclusive, only one button can be checked at any time
            // clear previously checked buttons then re-check the triggered button
            if (exclusive || (key == FILTER_RESET_KEY)) {
                clearFilters();
                button->setChecked(true);
                if (key == FILTER_RESET_KEY) {
                    emit filtersCleared();
                }
            } else {
                // if any other button is checked, make sure the reset button is unchecked
                if (resetButton && resetButton->isChecked()) {
                    resetButton->setChecked(false);
                    checkedKeys.removeAll(FILTER_RESET_KEY);
                }
            }

            // add the button's key to the checked keys list
            checkedKeys.append(key);

        } else {

            // the reset ("All") button cannot be unchecked by clicking on it
            if (key == FILTER_RESET_KEY) {
                button->setChecked(true);
                return;
            }

            // if un-checking the last checked button, check the reset ("All") button
            // there has to be one filter button checked at all times
            if (checkedKeys.size() == 1) {
                checkedKeys.append(FILTER_RESET_KEY);
                resetButton->setChecked(true);
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
 */
void FilterGroup::resetFilters()
{
    // clear the filter then re-check the "All" button
    clearFilters();
    if (resetButton) {
        resetButton->setChecked(true);
    }

    // update the checked count and send signals that the filters have changed
    updateFilterCheckedCount();
    emit filtersCleared();
    emit filtersChanged(checkedKeys);
}


/**
 * @brief FilterGroup::updateResetButtonVisibility
 */
void FilterGroup::updateResetButtonVisibility()
{
    bool visible = showResetButton;
    if (filterGroupBox) {
        visible = visible && filterGroupBox->isChecked();
    }
    if (resetAction) {
        resetAction->setVisible(visible);
    } else if (resetButton) {
        resetButton->setVisible(visible);
    }
}


/**
 * @brief FilterGroup::setupResetButton
 */
void FilterGroup::setupResetButton()
{
    resetButton = new QToolButton();
    resetButton->setText("All");
    resetButton->setProperty("iconName", "list");
    resetButton->setProperty("iconPath", "Icons");
    resetButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    resetButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    addToFilterGroup(resetKey, resetButton);
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
 * @brief FilterGroup::addToGroupBox
 * This adds/inserts the provided filter button to the toolbar in the filter group box.
 * It makes sure that the reset filter button is at the top of the toolbar.
 * @param button
 */
QAction* FilterGroup::addToGroupBox(QAbstractButton* button)
{
    if (filterGroupBox && filterToolbar) {
        QAction* action = 0;
        if (button == resetButton) {
            QAction* topAction = 0;
            if (!filterToolbar->actions().isEmpty()) {
                topAction = filterToolbar->actions().at(0);
            }
            if (topAction) {
                action = filterToolbar->insertWidget(topAction, button);
            } else {
                action = filterToolbar->addWidget(button);
            }
            resetAction = action;
            resetAction->setVisible(showResetButton);
        } else {
            action = filterToolbar->addWidget(button);
        }
        return action;
    }
    return 0;
}


/**
 * @brief FilterGroup::clearFilters
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
    if (filterGroupBox) {
        QString newTitle = filterGroupTitle;
        if (!resetButton->isChecked()) {
            newTitle = filterGroupTitle + " (" + QString::number(checkedKeys.count()) + ")";
        }
        filterGroupBox->setTitle(newTitle);
    }
}
