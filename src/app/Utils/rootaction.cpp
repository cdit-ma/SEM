#include "rootaction.h"

RootAction::RootAction(const QString& category, const QString& text, QObject *parent)
        : QAction(text, parent)
{
    this->category = category;
    connect(this, SIGNAL(changed()), this, SLOT(actionChanged()));
}

void RootAction::setIconPath(const QString& path, const QString& alias)
{
    iconPath = path;
    iconAlias = alias;
}

QPair<QString, QString> RootAction::getIconPair() const
{
    return QPair<QString, QString>(iconPath, iconAlias);
}

QString RootAction::getIconPath() const
{
    return iconPath;
}

QString RootAction::getIconAlias() const
{
    return iconAlias;
}

QString RootAction::getCategory() const
{
    return category;
}

QAction* RootAction::constructSubAction(bool stealth)
{
    auto action = new QAction(this);
    copyActionState(action, stealth);
    
    if (stealth) {
        stealthActions.append(action);
    } else {
        subActions.append(action);
    }
    
    connect(action, SIGNAL(toggled(bool)), this, SLOT(setChecked(bool)));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(trigger()));
    connect(action, SIGNAL(destroyed(QObject*)), this, SLOT(actionRemoved(QObject*)));
    return action;
}

void RootAction::actionChanged()
{
    for (QAction* action : subActions) {
        copyActionState(action, false);
    }
    for (QAction* action : stealthActions) {
        copyActionState(action, true);
    }
}

void RootAction::actionRemoved(QObject *obj)
{
    auto action = qobject_cast<QAction*>(obj);
    if (action) {
        //Remove.
        subActions.removeAll(action);
        stealthActions.removeAll(action);
    }
}

void RootAction::copyActionState(QAction* action, bool stealth)
{
    if (action != nullptr) {
    
        action->setEnabled(isEnabled());
        action->setCheckable(isCheckable());
        action->setChecked(isChecked());

        if (!icon().isNull()) {
            action->setIcon(icon());
        }
        if (iconText() != "") {
            action->setIconText(iconText());
        }
        if (text() != "") {
            action->setText(text());
        }
        if (stealth) {
            action->setVisible(isEnabled());
        }
    }
}
