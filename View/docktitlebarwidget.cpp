#include "docktitlebarwidget.h"
#include <QDebug>
DockTitleBarWidget::DockTitleBarWidget(QWidget* parent) : QToolBar(parent)
{
    //This sets to the parent that everything is okay.
    setFocusPolicy(Qt::ClickFocus);
    setFocusProxy(parent);

    //Setting as Custom Context Menu so the parent can catch this signal.
    setContextMenuPolicy(Qt::CustomContextMenu);
    setupToolBar();
}

DockTitleBarWidget::~DockTitleBarWidget()
{
}

void DockTitleBarWidget::setLabelStyleSheet(QString style)
{
    if(titleLabel){
        titleLabel->setStyleSheet(style);
    }
}


void DockTitleBarWidget::setIcon(QPixmap pixmap)
{
    iconLabel->setPixmap(pixmap);
}

void DockTitleBarWidget::setTitle(QString title, Qt::Alignment alignment)
{
    titleLabel->setText(title);
    titleLabel->setAlignment(alignment);
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
    default:
        return 0;
    }
}

void DockTitleBarWidget::setupToolBar()
{
    iconLabel = new QLabel(this);
    iconLabel->setVisible(false);
    iconLabel->setFixedSize(16,16);
    titleLabel = new QLabel(this);

    QWidget* widget = new QWidget();
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    addWidget(iconLabel);
    addWidget(titleLabel);
    addWidget(widget);

    popOutAction = addAction("Pop Out");
    maximizeAction = addAction("Maximise/Minimise");
    maximizeAction->setCheckable(true);
    closeAction = addAction("Close");
    protectAction = addAction("Protect Window");
    protectAction->setCheckable(true);
    setIconSize(QSize(16,16));
}

