#include "actionbutton.h"
#include <QDebug>
#include <QVBoxLayout>

ActionButton::ActionButton(QAction *action): QToolButton()
{
    if(action){
        setDefaultAction(action);
        this->action = action;
        connect(action, SIGNAL(changed()), this, SLOT(actionChanged()));
        this->setEnabled(action->isEnabled());
        this->setCheckable(action->isCheckable());
    }
 }

void ActionButton::actionChanged()
{
    setEnabled(action->isEnabled());
    setChecked(action->isChecked());
}
