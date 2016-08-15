#include "rootaction.h"

RootAction::RootAction(QString text, QObject *parent) : QAction(text, parent)
{
    connect(this, SIGNAL(changed()), this, SLOT(actionChanged()));
}

void RootAction::setIconPath(QString path, QString alias)
{
    iconPath = path;
    iconAlias = alias;
}

QPair<QString, QString> RootAction::getIconPair()
{
    return QPair<QString, QString>(iconPath, iconAlias);
}

QString RootAction::getIconPath()
{
    return iconPath;
}

QString RootAction::getIconAlias()
{
    return iconAlias;
}

QAction* RootAction::constructSubAction(bool stealth)
{
    QAction* action = new QAction(this);

    if(action){
        copyActionState(action, stealth);

        if(stealth){
            stealthActions.append(action);
        }else{
            subActions.append(action);
        }

        connect(action, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(trigger()));

        connect(action, SIGNAL(destroyed(QObject*)), this, SLOT(actionRemoved(QObject*)));
    }
    return action;
}

void RootAction::actionChanged()
{
    foreach(QAction* action, subActions){
        copyActionState(action, false);
    }
    foreach(QAction* action, stealthActions){
        copyActionState(action, true);
    }
}

void RootAction::actionRemoved(QObject *obj)
{
    QAction* action = qobject_cast<QAction*>(obj);
    if(action){
        //Remove.
        subActions.removeAll(action);
        stealthActions.removeAll(action);
    }
}

void RootAction::copyActionState(QAction *action, bool stealth)
{
    if(action){
        action->setEnabled(isEnabled());
        action->setCheckable(isCheckable());
        action->setChecked(isChecked());

        if(!icon().isNull()){
            action->setIcon(icon());
        }
        if(iconText() != ""){
            action->setIconText(iconText());
        }
        if(text() != ""){
            action->setText(text());
        }
        if(stealth){
            action->setVisible(isEnabled());
        }else{
            //action->setVisible(isVisible());
        }
    }
}
