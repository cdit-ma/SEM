#include "actiongroup.h"
#include <QDebug>
ActionGroup::ActionGroup(QObject* parent) : QActionGroup(parent)
{
    setExclusive(false);
}

void ActionGroup::addSeperator()
{
    QAction* seperator = new QAction(this);
    seperator->setSeparator(true);
    addAction(seperator);
}

void ActionGroup::updateSpacers()
{

    QList<QAction*> actions = this->actions();

    QList<int> visibleActionsCount;
    QList<int> spacerActionsIndexes;

    int currentVisibleCount = 0;

    for(int i=0; i < actions.size(); i++){
        QAction* action = actions.at(i);

        if(action->isSeparator()){
            spacerActionsIndexes.append(i);
            visibleActionsCount.append(currentVisibleCount);
            currentVisibleCount = 0;
        }else if(action->isVisible()){
            currentVisibleCount ++;
        }
    }

    visibleActionsCount.append(currentVisibleCount);

    for(int i=0; i < spacerActionsIndexes.size(); i++){
        int seperatorIndex = spacerActionsIndexes.at(i);

        int leftOfSeperator = visibleActionsCount.at(i);
        int rightOfSeperator = visibleActionsCount.at(i + 1);

        QAction* seperatorAction = actions.at(seperatorIndex);
        seperatorAction->setVisible(leftOfSeperator > 0 && rightOfSeperator > 0);
    }
}
