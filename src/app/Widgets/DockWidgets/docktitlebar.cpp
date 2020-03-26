#include "docktitlebar.h"
#include "../../theme.h"

#include <QToolButton>

/**
 * @brief DockTitleBar::DockTitleBar
 * @param parent
 */
DockTitleBar::DockTitleBar(QWidget* parent)
    : QToolBar(parent)
{
    setFocusPolicy(Qt::ClickFocus);
    setContentsMargins(0,0,0,0);

    //Setting as Custom Context Menu so the parent can catch this signal.
    setContextMenuPolicy(Qt::CustomContextMenu);
    setupToolBar();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

/**
 * @brief DockTitleBar::setActive
 * @param active
 */
void DockTitleBar::setActive(bool active)
{
    if (_isActive != active) {
        _isActive = active;
        updateActiveStyle();
    }
}

/**
 * @brief DockTitleBar::setIcon
 * @param iconPath
 * @param iconName
 */
void DockTitleBar::setIcon(const QString& iconPath, const QString& iconName)
{
    iconAction->setProperty("iconPath", iconPath);
    iconAction->setProperty("iconName", iconName);
    iconAction->setIcon(Theme::theme()->getIcon(iconPath, iconName));
}

/**
 * @brief DockTitleBar::setTitle
 * @param title
 * @param alignment
 */
void DockTitleBar::setTitle(const QString& title, Qt::Alignment alignment)
{
    if (titleLabel) {
        titleLabel->setText(title);
        titleLabel->setAlignment(alignment | Qt::AlignVCenter);
    }
}

/**
 * @brief DockTitleBar::getTitle
 * @return
 */
QString DockTitleBar::getTitle() const
{
    if (titleLabel) {
        return titleLabel->text();
    }
    return "";
}

/**
 * @brief DockTitleBar::getAction
 * @param action
 * @return
 */
QAction* DockTitleBar::getAction(DockTitleBar::DOCK_ACTION action)
{
    switch(action) {
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
            return nullptr;
    }
}

/**
 * @brief DockTitleBar::addToolAction
 * @param action
 * @param iconPath
 * @param iconName
 * @param alignment
 */
void DockTitleBar::addToolAction(QAction* action, const QString& iconPath, const QString& iconName, Qt::Alignment alignment)
{
    if (action) {
        if (alignment == Qt::AlignLeft) {
            insertAction(iconAction, action);
        } else if (alignment == Qt::AlignRight) {
            addAction(action);
        } else if (alignment == Qt::AlignCenter) {
            insertAction(maximizeAction, action);
        }
        updateIcon(action, iconPath, iconName);
    }
}

/**
 * @brief DockTitleBar::themeChanged
 */
void DockTitleBar::themeChanged()
{
    updateActiveStyle();

    Theme* theme = Theme::theme();
    setIconSize(theme->getIconSize());

    for (auto action : actions()) {
        QString path = action->property("iconPath").toString();
        QString name = action->property("iconName").toString();
        if (!path.isEmpty() && !name.isEmpty()) {
            updateIcon(action, path, name);
        }
    }
}

/**
 * @brief DockTitleBar::getToolActions
 * @return
 */
QList<QAction*> DockTitleBar::getToolActions() const
{
    return toolActions;
}

/**
 * @brief DockTitleBar::updateActiveStyle
 */
void DockTitleBar::updateActiveStyle()
{
    setStyleSheet(Theme::theme()->getDockTitleBarStyleSheet(_isActive));
}

/**
 * @brief DockTitleBar::updateIcon
 * @param action
 * @param iconPath
 * @param iconName
 * @param newIcon
 */
void DockTitleBar::updateIcon(QAction* action, const QString& iconPath, const QString& iconName)
{
    if (action) {
        action->setProperty("iconPath", iconPath);
        action->setProperty("iconName", iconName);
        action->setIcon(Theme::theme()->getIcon(iconPath, iconName));
    }
}

/**
 * @brief DockTitleBar::setupToolBar
 */
void DockTitleBar::setupToolBar()
{
    iconAction = addAction("");

    // TODO: Test further and keep an eye out for weird looking/behaving icons on DockWidget's DockTitleBars
    //  As far as I'm aware, the icons are purely used for display and have no functionality; this shouldn't be needed
    /*
    iconAction->setCheckable(true);
    iconAction->setChecked(true);
    connect(iconAction, &QAction::triggered, [=](){iconAction->setChecked(true);});
    */

    auto button = (QToolButton*) widgetForAction(iconAction);
    button->setObjectName("WINDOW_ICON");
    button->setAutoRaise(false);

    titleLabel = new QLabel(this);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    addWidget(titleLabel);

    maximizeAction = addAction("Maximise/Minimise");
    updateIcon(maximizeAction, "ToggleIcons", "maximize");
    maximizeAction->setCheckable(true);
    maximizeAction->setVisible(false);

    popOutAction = addAction("Pop Out");
    updateIcon(popOutAction, "Icons", "popOut");
    popOutAction->setShortcutContext(Qt::WindowShortcut);
    popOutAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    popOutAction->setVisible(false);
    toolActions.append(popOutAction);

    protectAction = addAction("Protect Window");
    updateIcon(protectAction, "ToggleIcons", "lock");
    protectAction->setCheckable(true);
    protectAction->setVisible(false);
    toolActions.append(protectAction);

    hideAction = addAction("Show/Hide Window");
    updateIcon(hideAction, "ToggleIcons", "visible");
    hideAction->setCheckable(true);
    hideAction->setChecked(true);
    hideAction->setVisible(false);
    toolActions.append(hideAction);

    closeAction = addAction("Close");
    updateIcon(closeAction, "Icons", "cross");
    closeAction->setVisible(false);
    toolActions.append(closeAction);
}