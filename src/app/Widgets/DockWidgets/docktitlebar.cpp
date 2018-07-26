#include "docktitlebar.h"
#include "../../theme.h"

#include <QDebug>

#include <QToolButton>

DockTitleBar::DockTitleBar(QWidget* parent) : QToolBar(parent)
{
    _isActive = false;

    //This sets to the parent that everything is okay.
    setFocusPolicy(Qt::ClickFocus);
    setContentsMargins(0,0,0,0);

    //Setting as Custom Context Menu so the parent can catch this signal.
    setContextMenuPolicy(Qt::CustomContextMenu);
    setupToolBar();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

DockTitleBar::~DockTitleBar()
{
}

void DockTitleBar::setActive(bool active)
{
    if(_isActive != active){
        _isActive = active;
        updateActiveStyle();
    }
}

QList<QAction *> DockTitleBar::getToolActions()
{
    return actions;
}

void DockTitleBar::setIcon(QString iconPath, QString iconName){
    icon_path.first = iconPath;
    icon_path.second = iconName;
    updateIcon(iconAction, icon_path.first, icon_path.second);    
}

/*
QPixmap DockTitleBar::getIcon()
{
    return *iconLabel->pixmap();
}*/

void DockTitleBar::setTitle(QString title, Qt::Alignment alignment)
{
    if(titleLabel){
        titleLabel->setText(title);
        titleLabel->setAlignment(alignment | Qt::AlignVCenter);
    }
    
}

QString DockTitleBar::getTitle()
{
    if(titleLabel){
        return titleLabel->text();
    }
    return "";
}

QAction *DockTitleBar::getAction(DockTitleBar::DOCK_ACTION action)
{
    switch(action){
    case DA_CLOSE:
        return closeAction;
    case DA_MAXIMIZE:
        return maximizeAction;
    case DA_POPOUT:
        return popOutAction;
    case DA_PROTECT:
        return protectAction;
    case DA_HIDE:
        return hideAction;
    case DA_ICON:
        return iconAction;
    default:
        return 0;
    }
}

void DockTitleBar::updateIcon(QAction* action, QString iconPath, QString iconName){
    if(action){
        action->setIcon(Theme::theme()->getIcon(iconPath, iconName));//, true));
    }
}
void DockTitleBar::updateIcon(QLabel* action, QString iconPath, QString iconName){
    if(action){
        action->setPixmap(Theme::theme()->getIcon(iconPath, iconName).pixmap(QSize(16,16), QIcon::Normal, QIcon::On));
    }
}

void DockTitleBar::themeChanged()
{
    updateActiveStyle();
    Theme* theme = Theme::theme();
    auto icon_size = theme->getIconSize();
    setIconSize(icon_size);

    updateIcon(closeAction, "Icons", "cross");
    updateIcon(popOutAction, "Icons", "popOut");
    updateIcon(maximizeAction, "ToggleIcons", "maximize");
    updateIcon(protectAction, "ToggleIcons", "lock");
    updateIcon(hideAction, "ToggleIcons", "visible");
    updateIcon(iconAction, icon_path.first, icon_path.second);
}

void DockTitleBar::updateActiveStyle()
{
    setStyleSheet(Theme::theme()->getDockTitleBarStyleSheet(isActive()));
}

void DockTitleBar::setupToolBar()
{
    iconAction = addAction("");
    iconAction->setCheckable(true);
    iconAction->setChecked(true);
    auto button = (QToolButton*) widgetForAction(iconAction);
    button->setObjectName("WINDOW_ICON");
    button->setAutoRaise(false);
    connect(iconAction, &QAction::triggered, [=](){iconAction->setChecked(true);});

    titleLabel = new QLabel(this);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //iconAction->setVisible(false);
    addWidget(titleLabel);

    popOutAction = addAction("Pop Out");
    actions.append(popOutAction);
    popOutAction->setVisible(false);
    popOutAction->setShortcutContext(Qt::WindowShortcut);
    popOutAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));

    maximizeAction = addAction("Maximise/Minimise");
    //actions.append(maximizeAction);
    maximizeAction->setCheckable(true);
    maximizeAction->setVisible(false);

    protectAction = addAction("Protect Window");
    actions.append(protectAction);
    protectAction->setCheckable(true);
    protectAction->setVisible(false);

    hideAction = addAction("Show/Hide Window");
    addAction(hideAction);
    actions.append(hideAction);
    hideAction->setCheckable(true);
    hideAction->setChecked(true);
    hideAction->setVisible(false);

    closeAction = addAction("Close");
    actions.append(closeAction);
    closeAction->setVisible(false);
}

bool DockTitleBar::isActive()
{
    return _isActive;
}

