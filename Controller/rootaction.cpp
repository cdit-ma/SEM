#include "rootaction.h"

RootAction::RootAction(QString text, QObject *parent) : QAction(text, parent)
{
    stealthAction = 0;
}

void RootAction::setIconPath(QString path, QString alias)
{
    iconPath = path;
    iconAlias = alias;
}

QString RootAction::getIconPath()
{
    return iconPath;
}

QString RootAction::getIconAlias()
{
    return iconAlias;
}

QAction *RootAction::getStealthAction()
{
    if(!stealthAction){
        connect(this, SIGNAL(changed()), this, SLOT(actionChanged()));
        stealthAction = new QAction(this);
    }
    return stealthAction;
}

void RootAction::actionChanged()
{
    if(stealthAction){
        stealthAction->setEnabled(isEnabled());
        stealthAction->setCheckable(isCheckable());
        stealthAction->setChecked(isChecked());
        stealthAction->setIcon(icon());
        stealthAction->setIconText(iconText());
        stealthAction->setText(text());
        stealthAction->setVisible(isEnabled());
    }
}

