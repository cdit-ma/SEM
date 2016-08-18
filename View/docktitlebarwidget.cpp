#include "docktitlebarwidget.h"
#include <QDebug>
DockTitleBarWidget::DockTitleBarWidget(QWidget* parent) : QToolBar(parent)
{
    //This sets to the parent that everything is okay.
    setFocusPolicy(Qt::ClickFocus);
    setFocusProxy(parent);
    //setMouseTracking(true);

    setContentsMargins(0,0,0,0);


    //Setting as Custom Context Menu so the parent can catch this signal.
    setContextMenuPolicy(Qt::CustomContextMenu);
    setupToolBar();
}

DockTitleBarWidget::~DockTitleBarWidget()
{
}

void DockTitleBarWidget::setToolBarIconSize(int height)
{
    setIconSize(QSize(height, height));
    iconLabel->setFixedSize(height +2,height);
}

void DockTitleBarWidget::setLabelStyleSheet(QString style)
{
    if (titleLabel) {
        titleLabel->setStyleSheet(style);
    }
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
    popOutAction->setVisible(false);
    maximizeAction = addAction("Maximise/Minimise");
    maximizeAction->setCheckable(true);
    maximizeAction->setVisible(false);
    closeAction = addAction("Close");
    closeAction->setVisible(false);
    protectAction = addAction("Protect Window");
    protectAction->setCheckable(true);
    protectAction->setVisible(false);
    hideAction = addAction("Hide Window");
    hideAction->setVisible(false);

    setToolBarIconSize(16);
}

