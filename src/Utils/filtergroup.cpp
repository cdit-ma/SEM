#include "filtergroup.h"
#include "../theme.h"

#include <QDebug>

#define FILTER_KEY "filterKey"
#define FILTER_GROUP "filterGroup"
#define FILTER_RESET_KEY "All"
#define DEFAULT_CHECKED_STATE true

/**
 * @brief FilterGroup::FilterGroup
 * @param groupKind
 * @param parent
 */
FilterGroup::FilterGroup(QString group, QObject* parent) : QObject(parent)
{
    filterGroupBox = 0;
    filterToolbar = 0;
    filterGroup = group;
    exclusive = false;

    resetAction = 0;
    showResetButton = true;

    // construct and add reset ("All") button - this is visible by default
    setupResetButton();
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

    filterGroupBox = new QGroupBox(filterGroup);
    filterGroupBox->setCheckable(true);
    //filterGroupBox->setFlat(true);
    filterGroupBox->setAlignment(Qt::AlignHCenter);

    filterToolbar = new QToolBar();
    filterToolbar->setOrientation(orientation);
    filterToolbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    foreach (QAbstractButton* button, filterButtons.values()) {
        addToGroupBox(button);
    }

    // set the groupbox's initial checked state - this sets the filter buttons initial visibilty state
    filterGroupBox->setChecked(DEFAULT_CHECKED_STATE);
    filterGroupBox->toggled(DEFAULT_CHECKED_STATE);

    QLayout* layout = new QVBoxLayout(filterGroupBox);
    layout->addWidget(filterToolbar);

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
 * @brief FilterGroup::setResetButtonVisible
 * @param visible
 */
void FilterGroup::setResetButtonVisible(bool visible)
{
    showResetButton = visible;
    updateResetButtonVisibility();
}


/**
 * @brief FilterGroup::addToFilterGroup
 * @param key
 * @param filterButton
 */
void FilterGroup::addToFilterGroup(QString key, QAbstractButton* filterButton)
{
    return;

    if (filters.contains(key)) {
        qWarning() << "FilterGroup::addToFilterGroup - A filter with key [" << key << "] already exists.";
        return;
    }

    if (filterButton) {

        filters[key] = filterButton;
        filterButton->setProperty(FILTER_KEY, key);
        filterButton->setProperty(FILTER_GROUP, filterGroup);
        filterButton->setCheckable(true);
        filterButton->setChecked(false);
        connect(filterButton, SIGNAL(clicked(bool)), this, SLOT(filterTriggered()));

        // initially have the reset ("All") button checked
        if (filterButton == resetFilterButton) {
            filterButton->setChecked(true);
            checkedKeys.append(key);
        }

        addToGroupBox(filterButton);
    }
}

void FilterGroup::addToFilterGroup(QVariant key, QAbstractButton* filterButton)
{
    uint variantKey = variantHasher.hash(key);

    if (filterButtons.contains(variantKey)) {
        qWarning() << "FilterGroup::addToFilterGroup - A filter with key [" << key.toString() << "] already exists.";
        return;
    }

    if (filterButton) {

        filterButtons[variantKey] = filterButton;
        filterButton->setProperty(FILTER_KEY, key);
        filterButton->setProperty(FILTER_GROUP, filterGroup);
        filterButton->setCheckable(true);
        filterButton->setChecked(false);
        connect(filterButton, SIGNAL(clicked(bool)), this, SLOT(filterTriggered()));

        // initially have the reset ("All") button checked
        if (filterButton == resetFilterButton) {
            filterButton->setChecked(true);
            checkedFilterKeys.append(key);
        }

        addToGroupBox(filterButton);
    }
}


/**
 * @brief FilterGroup::themeChanged
 */
void FilterGroup::themeChanged()
{
    Theme* theme = Theme::theme();

    if (filterGroupBox) {
        filterGroupBox->setStyleSheet("QGroupBox{ color:" + theme->getTextColorHex() + "; margin-top: 10px; margin-bottom: 0px; border: none; border-top: 4px solid" + theme->getAltBackgroundColorHex() + ";}"
                                      "QGroupBox::title{ subcontrol-origin: margin; subcontrol-position: top center; margin-top: 10px; padding: -2px 6px 2px -16px; }"
                                      "QGroupBox::title::hover{ color:" + theme->getHighlightColorHex() + ";}"
                                      "QGroupBox::indicator:checked{ image: none; }"
                                      "QGroupBox::indicator:unchecked{ image: none; }"
                                      /*
                                      "QGroupBox::title{ subcontrol-origin: margin; subcontrol-position: top center; padding: 0px 3px 0px 3px; }"
                                      "QGroupBox::indicator:checked{ image: url(:/Icons/arrowHeadDown); }"
                                      "QGroupBox::indicator:unchecked{ image: url(:/Icons/arrowHeadUp); }"
                                      */
                                      + theme->getToolBarStyleSheet() +
                                      "QToolButton{ border-radius:" + theme->getSharpCornerRadius() + ";}");
    }

    //QString iconPath = resetFilterButton->property("iconPath").toString();
    //QString iconName = resetFilterButton->property("iconName").toString();
    //resetFilterButton->setIcon(theme->getIcon(iconPath, iconName));

    foreach (QAbstractButton* button, filterButtons.values()) {
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

        QStringList prevCheckedKeys = checkedKeys;
        QString key = button->property(FILTER_KEY).toString();

        if (button->isChecked()) {
            if (exclusive || (key == FILTER_RESET_KEY)) {
                // if this group is exclusive or the reset button is checked, clear all the previously checked filter buttons
                clearFilters();
                button->setChecked(true);
            } else {
                // if any other button is checked, make sure the reset button is unchecked
                if (resetFilterButton && resetFilterButton->isChecked()) {
                    resetFilterButton->setChecked(false);
                    checkedKeys.removeAll(FILTER_RESET_KEY);
                }
            }
            checkedKeys.append(key);
        } else {
            // if un-checking the last checked button, check the reset ("All") button
            // there has to be one filter button checked at all times
            if (checkedKeys.size() == 1) {
                if (button == resetFilterButton) {
                    button->setChecked(true);
                    return;
                }
                checkedKeys.removeAll(key);
                checkedKeys.append(FILTER_RESET_KEY);
                resetFilterButton->setChecked(true);
            } else {
                // this case should never happen - the case above should catch it
                if (exclusive) {
                    qWarning() << "FilterGroup::filterTriggered - Exclusive filter group has more than one checked button.";
                    return;
                }
                checkedKeys.removeAll(key);
            }
        }

        if (prevCheckedKeys != checkedKeys) {
            updateFilterCheckedCount();
            emit filtersChanged(checkedKeys);
        }
    }
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
    } else if (resetFilterButton) {
        resetFilterButton->setVisible(visible);
    }
}


/**
 * @brief FilterGroup::setupResetButton
 */
void FilterGroup::setupResetButton()
{
    resetFilterButton = new QToolButton();
    resetFilterButton->setText("All");
    resetFilterButton->setProperty("iconName", "list");
    resetFilterButton->setProperty("iconPath", "Icons");
    resetFilterButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    resetFilterButton->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    addToFilterGroup(QVariant(FILTER_RESET_KEY), resetFilterButton);
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
        if (button == resetFilterButton) {
            QAction* topAction = filterToolbar->actions().at(0);
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
        qDebug() << "Added button - " << button->text();
        connect(filterGroupBox, SIGNAL(toggled(bool)), action, SLOT(setVisible(bool)));
        return action;
    }
    return 0;
}


/**
 * @brief FilterGroup::clearFilters
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
        if (resetFilterButton->isChecked()) {
            filterGroupBox->setTitle(filterGroup);
        } else {
            filterGroupBox->setTitle(filterGroup + " (" + QString::number(checkedKeys.count()) + ")");
        }
    }
}




/*
class QVariantHasher {
  public:
    QVariantHasher() : buff(&bb), ds(&buff) {
      bb.reserve(1000);
      buff.open(QIODevice::WriteOnly);
    }
    uint hash(const QVariant & v) {
      buff.seek(0);
      ds << v;
      return qHashBits(bb.constData(), buff.pos());
    }
  private:
    QByteArray bb;
    QBuffer buff;
    QDataStream ds;
};
*/
