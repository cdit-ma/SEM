#include "filtergroup.h"

#include <QDebug>

#define FILTER_KEY "filterKey"
#define FILTER_GROUP "filterGroup"
#define FILTER_RESET_KEY "All"

/**
 * @brief FilterGroup::FilterGroup
 * @param groupKind
 * @param parent
 */
FilterGroup::FilterGroup(QString group, QObject* parent) : QObject(parent)
{
    resetFilterButton = 0;
    filterGroup = group;
    exclusive = false;
}


/**
 * @brief FilterGroup::getFilterGroup
 * @return
 */
QString FilterGroup::getFilterGroup()
{
    return filterGroup;
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
 * @brief FilterGroup::addToFilterGroup
 * @param key
 * @param filterButton
 */
void FilterGroup::addToFilterGroup(QString key, QAbstractButton* filterButton, bool resetButton)
{
    if (filters.contains(key)) {
        qWarning() << "FilterGroup::addToFilterGroup - A filter with key [" << key << "] already exists.";
        return;
    }
    if (filterButton) {

        if (resetButton) {
            if (resetFilterButton) {
                qWarning() << "FilterGroup::addToFilterGroup - There is already a reset filter button.";
                return;
            }
            // this forces the reset filter button to have the correct key
            filterButton->setProperty(FILTER_KEY, FILTER_RESET_KEY);
            resetFilterButton = filterButton;
        } else {
            filterButton->setProperty(FILTER_KEY, key);
        }

        filterButton->setProperty(FILTER_GROUP, filterGroup);
        filterButton->setCheckable(true);
        filters[key] = filterButton;

        //connect(filterButton, SIGNAL(clicked(bool)), this, SLOT(filterTriggered()));
        connect(filterButton, SIGNAL(clicked(bool)), this, SLOT(filterTriggered()));
    }
}


/**
 * @brief FilterGroup::filterTriggered
 */
void FilterGroup::filterTriggered()
{
    QAbstractButton* button = qobject_cast<QAbstractButton*>(sender());
    if (button) {

        QString key = button->property(FILTER_KEY).toString();

        if (button->isChecked()) {
            // if this group is exclusive or the reset button is checked, clear all the previously checked filter buttons
            if (exclusive || (key == FILTER_RESET_KEY)) {
                clearFilters();
                button->setChecked(true);
            } else {
                if (resetFilterButton && resetFilterButton->isChecked()) {
                    resetFilterButton->setChecked(false);
                }
            }
            checkedKeys.append(key);
        } else {
            //TODO
            // if this group is exclusive, there has to be one filter button selected at all times
            if (checkedKeys.size() == 1) {
                if (exclusive || (button == resetFilterButton)) {
                    button->setChecked(true);
                    return;
                }
            } else {
                checkedKeys.removeAll(key);
            }
        }

        emit filtersChanged(checkedKeys);
    }
}


/**
 * @brief FilterGroup::clearFilters
 * @param exceptButton
 */
void FilterGroup::clearFilters()
{
    foreach (QString key, checkedKeys) {
        QAbstractButton* button = filters.value(key, 0);
        if (button) {
            button->setChecked(false);
        }
    }
    checkedKeys.clear();
}
