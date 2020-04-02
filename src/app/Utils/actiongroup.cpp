#include "actiongroup.h"

/**
 * @brief ActionGroup::ActionGroup
 * @param parent
 */
ActionGroup::ActionGroup(QObject* parent)
	: QActionGroup(parent)
{
    setExclusive(false);
}

/**
 * @brief ActionGroup::addSeparator
 */
void ActionGroup::addSeparator()
{
    auto separator = new QAction(this);
    separator->setSeparator(true);
    addAction(separator);
}

/**
 * @brief ActionGroup::updateSpacers
 */
void ActionGroup::updateSpacers()
{
    QList<QAction*> actions = this->actions();
    QList<int> visibleActionsCount;
    QList<int> spacerActionsIndexes;

    int currentVisibleCount = 0;

    for (int i = 0; i < actions.size(); i++) {
        QAction* action = actions.at(i);
        if (action->isSeparator()) {
            spacerActionsIndexes.append(i);
            visibleActionsCount.append(currentVisibleCount);
            currentVisibleCount = 0;
        } else if (action->isVisible()) {
            currentVisibleCount++;
        }
    }

    visibleActionsCount.append(currentVisibleCount);

    for (int i = 0; i < spacerActionsIndexes.size(); i++) {
        int separatorIndex = spacerActionsIndexes.at(i);
        int leftOfSeparator = visibleActionsCount.at(i);
        bool show_separator = leftOfSeparator > 0 && ((i + 1) != visibleActionsCount.size());
        auto separatorAction = actions.at(separatorIndex);
        separatorAction->setVisible(show_separator);
    }
}