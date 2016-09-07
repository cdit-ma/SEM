#include "docktitlebarwidget.h"
#include <QDebug>
#include "theme.h"

DockTitleBarWidget::DockTitleBarWidget(QWidget* parent) : QToolBar(parent)
{
    _isActive = false;
    //This sets to the parent that everything is okay.
    setFocusPolicy(Qt::ClickFocus);
    setFocusProxy(parent);
    //setMouseTracking(true);

    setContentsMargins(0,0,0,0);


    //Setting as Custom Context Menu so the parent can catch this signal.
    setContextMenuPolicy(Qt::CustomContextMenu);
    setupToolBar();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

DockTitleBarWidget::~DockTitleBarWidget()
{
}

void DockTitleBarWidget::setActive(bool active)
{
    if(_isActive != active){
        _isActive = active;
        updateActiveStyle();
    }
}

QList<QAction *> DockTitleBarWidget::getToolActions()
{
    return actions;
}

void DockTitleBarWidget::setToolBarIconSize(int height)
{
    setIconSize(QSize(height, height));
    iconLabel->setFixedSize(height +2,height);
}

void DockTitleBarWidget::setIcon(QPixmap pixmap)
{
    iconLabel->setPixmap(pixmap);
    iconAction->setVisible(!pixmap.isNull());
}

void DockTitleBarWidget::setTitle(QString title, Qt::Alignment alignment)
{
    titleLabel->setText(title);
    titleLabel->setAlignment(alignment | Qt::AlignVCenter);
}

QString DockTitleBarWidget::getTitle()
{
    return titleLabel->text();
}

QAction *DockTitleBarWidget::getAction(DockTitleBarWidget::DOCK_ACTION action)
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

void DockTitleBarWidget::themeChanged()
{
    updateActiveStyle();
    Theme* theme = Theme::theme();

    //Update Icons.
    if(closeAction){
        closeAction->setIcon(theme->getIcon("Actions", "Close"));
    }
    if(maximizeAction){
        maximizeAction->setIcon(theme->getIcon("Actions", "Maximize"));
    }
    if(popOutAction){
        popOutAction->setIcon(theme->getIcon("Actions", "DockPopOut"));
    }
    if(protectAction){
        protectAction->setIcon(theme->getIcon("Actions", "Lock_Open"));
    }
    if(hideAction){
        hideAction->setIcon(theme->getIcon("Actions", "Visible"));
    }

}

void DockTitleBarWidget::updateActiveStyle()
{
    setStyleSheet(Theme::theme()->getDockTitleBarStyleSheet(isActive()));
}

void DockTitleBarWidget::setupToolBar()
{
    iconLabel = new QLabel(this);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("margin-right: 2px;");

    titleLabel = new QLabel(this);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    iconAction = addWidget(iconLabel);
    iconAction->setVisible(false);
    addWidget(titleLabel);

    popOutAction = addAction("Pop Out");
    actions.append(popOutAction);
    popOutAction->setVisible(false);

    maximizeAction = addAction("Maximise/Minimise");
    //actions.append(maximizeAction);
    maximizeAction->setCheckable(true);
    maximizeAction->setVisible(false);

    protectAction = addAction("Protect Window");
    actions.append(protectAction);
    protectAction->setCheckable(true);
    protectAction->setVisible(false);

    hideAction = addAction("Hide Window");
    actions.append(hideAction);
    hideAction->setCheckable(true);
    hideAction->setChecked(true);
    hideAction->setVisible(false);

    closeAction = addAction("Close");
    actions.append(closeAction);
    closeAction->setVisible(false);


    setToolBarIconSize(16);
}

bool DockTitleBarWidget::isActive()
{
    return _isActive;
}

