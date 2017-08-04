#include "docktitlebar.h"
#include "../../theme.h"

#include <QDebug>
#include <QBitmap>

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

void DockTitleBar::setToolBarIconSize(int height)
{
    setIconSize(QSize(height, height));
    iconLabel->setFixedSize(height +2,height);
}

void DockTitleBar::setIcon(QString iconPath, QString iconName){
    icon_path.first = iconPath;
    icon_path.second = iconName;
    updateIcon(iconAction, icon_path.first, icon_path.second);    
}
QPixmap DockTitleBar::getIcon()
{
    return *iconLabel->pixmap();
}

void DockTitleBar::setTitle(QString title, Qt::Alignment alignment)
{
    titleLabel->setText(title);
    titleLabel->setAlignment(alignment | Qt::AlignVCenter);
}

QString DockTitleBar::getTitle()
{
    return titleLabel->text();
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

void DockTitleBar::addToolAction(QAction* action, Qt::Alignment alignment)
{
    if (alignment == Qt::AlignLeft) {
        insertAction(iconAction, action);
    } else if (alignment == Qt::AlignRight) {
        addAction(action);
    }
}
void DockTitleBar::updateIcon(QAction* action, QString iconPath, QString iconName){
    if(action){
        action->setIcon(Theme::theme()->getIcon(iconPath, iconName));
    }
}

void DockTitleBar::themeChanged()
{
    updateActiveStyle();
    Theme* theme = Theme::theme();

    updateIcon(closeAction, "Icons", "cross");
    updateIcon(maximizeAction, "Icons", "maximizeToggle");
    updateIcon(popOutAction, "Icons", "popOut");
    updateIcon(protectAction, "Icons", "lockToggle");
    updateIcon(hideAction, "Icons", "visibleToggle");
    updateIcon(iconAction, icon_path.first, icon_path.second);
}

void DockTitleBar::updateActiveStyle()
{
    setStyleSheet(Theme::theme()->getDockTitleBarStyleSheet(isActive()));
}

void DockTitleBar::setupToolBar()
{
    iconLabel = new QLabel(this);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("margin-right: 2px;");

    titleLabel = new QLabel(this);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    iconAction = addWidget(iconLabel);
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

    setToolBarIconSize(16);
}

bool DockTitleBar::isActive()
{
    return _isActive;
}

