#include "filtergroup.h"
#include "theme.h"

#include <QDebug>
#include <QToolBar>
#include <QToolButton>

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
    filterGroupBox = 0;
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
 * @brief FilterGroup::constructFilterGroupBox
 * @param alignment
 * @return
 */
QGroupBox* FilterGroup::constructFilterGroupBox(Qt::Orientation orientation)
{
    if (filterGroupBox) {
        return filterGroupBox;
    }

    bool initialCheckedState = false;
    filterGroupBox = new QGroupBox(filterGroup);
    filterGroupBox->setCheckable(true);
    filterGroupBox->setChecked(initialCheckedState);

    QToolBar* toolbar = new QToolBar();
    toolbar->setOrientation(orientation);
    toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    //toolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    foreach (QAbstractButton* button, filters.values()) {
        QToolButton* tb = qobject_cast<QToolButton*>(button);
        if (tb) {
            QAction* action = toolbar->addWidget(tb);
            action->setVisible(initialCheckedState);
            connect(filterGroupBox, SIGNAL(toggled(bool)), action, SLOT(setVisible(bool)));
        }
    }

    QLayout* layout = new QVBoxLayout(filterGroupBox);
    layout->addWidget(toolbar);

    /*
    QLayout* layout = 0;
    if (orientation == Qt::Vertical) {
        layout = new QVBoxLayout(filterGroupBox);
    } else {
        layout = new QHBoxLayout(filterGroupBox);
    }

    if (layout) {
        foreach (QAbstractButton* button, filters.values()) {
            button->setVisible(initialCheckedState);
            connect(filterGroupBox, SIGNAL(toggled(bool)), button, SLOT(setVisible(bool)));
            layout->addWidget(button);
        }
    }
    */

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
            key = FILTER_RESET_KEY;
            resetFilterButton = filterButton;
        }

        filters[key] = filterButton;
        filterButton->setProperty(FILTER_KEY, key);
        filterButton->setProperty(FILTER_GROUP, filterGroup);
        filterButton->setCheckable(true);
        connect(filterButton, SIGNAL(clicked(bool)), this, SLOT(filterTriggered()));

        // initially have either the "ALL" button or if there is no "ALL" button, the very first added button checked
        if (resetButton || (!resetButton && filters.size() == 1)) {
            clearFilters();
            filterButton->setChecked(true);
            checkedKeys.append(key);
        }

        if (filterGroupBox && filterGroupBox->layout()) {
            filterGroupBox->layout()->addWidget(filterButton);
            connect(filterGroupBox, SIGNAL(toggled(bool)), filterButton, SLOT(setVisible(bool)));
        }
    }
}


/**
 * @brief FilterGroup::themeChanged
 */
void FilterGroup::themeChanged()
{
    Theme* theme = Theme::theme();
    if (filterGroupBox) {
        filterGroupBox->setStyleSheet("QGroupBox{ color:" + theme->getTextColorHex() + "; margin-top: 6px; border: none; border-top: 2px solid" + theme->getAltBackgroundColorHex() + ";}"
                                      "QGroupBox::title{ subcontrol-origin: margin; subcontrol-position: top center; padding: 0px 3px 0px 3px; }"
                                      "QGroupBox::title::hover{ color:" + theme->getHighlightColorHex() + ";}"
                                      ///*
                                      "QGroupBox::indicator:checked{ image: url(:/Actions/Arrow_Down); }"
                                      "QGroupBox::indicator:unchecked{ image: url(:/Actions/Arrow_Up); }"
                                      //*/
                                      "QToolButton{ border-radius:" + theme->getSharpCornerRadius() + ";}");
    }
}


/**
 * @brief FilterGroup::filterTriggered
 */
void FilterGroup::filterTriggered()
{
    QAbstractButton* button = qobject_cast<QAbstractButton*>(sender());
    if (button) {

        QStringList prevCheckedKeys = checkedKeys;
        QString key = button->property(FILTER_KEY).toString();
        bool sendUpdateSignal = false;

        if (button->isChecked()) {
            // if this group is exclusive or the reset button is checked, clear all the previously checked filter buttons
            if (exclusive || (key == FILTER_RESET_KEY)) {
                clearFilters();
                button->setChecked(true);
            } else {
                // if any other button is checked, make sure the reset button is unchecked
                if (resetFilterButton && resetFilterButton->isChecked()) {
                    resetFilterButton->setChecked(false);
                    checkedKeys.removeAll(FILTER_RESET_KEY);
                }
            }
            sendUpdateSignal = true;
            checkedKeys.append(key);
        } else {
            // if this group is exclusive, there has to be one filter button selected at all times
            if (checkedKeys.size() == 1) {
                if (exclusive) {
                    button->setChecked(true);
                    return;
                }
                if (resetFilterButton) {
                    if (button == resetFilterButton) {
                        button->setChecked(true);
                        return;
                    } else {
                        checkedKeys.removeAll(key);
                        checkedKeys.append(FILTER_RESET_KEY);
                        resetFilterButton->setChecked(true);
                        key = FILTER_RESET_KEY;
                    }
                }
            } else {
                checkedKeys.removeAll(key);
            }
        }

        if (prevCheckedKeys != checkedKeys) {
            updateFilterCheckedCount(key);
            emit filtersChanged(checkedKeys);
        }
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


/**
 * @brief FilterGroup::updateFilterCheckedCount
 * This is called whenever the filters checked list is changed.
 * It updates the checked filters count in the group box title.
 */
void FilterGroup::updateFilterCheckedCount()
{
    if (filterGroupBox) {
        int checkedKeysCount = checkedKeys.count();
        if (checkedKeysCount == 0) {
            filterGroupBox->setTitle(filterGroup);
        } else {
            filterGroupBox->setTitle(filterGroup + " (" + QString::number(checkedKeysCount) + ")");
        }
        /*
        if (key == FILTER_RESET_KEY) {
            filterGroupBox->setTitle(filterGroup);
        } else {
            filterGroupBox->setTitle(filterGroup + " (" + QString::number(checkedKeys.count()) + ")");
        }
        */
    }
}
