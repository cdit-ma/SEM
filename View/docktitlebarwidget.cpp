#include "docktitlebarwidget.h"
#include <QDebug>
DockTitleBarWidget::DockTitleBarWidget(QWidget* parent) : QToolBar(parent)
{
    setFocusPolicy(Qt::ClickFocus);
    setFocusProxy(parent);
    //Setting as Custom Context Menu so the parent can catch this signal.
    setContextMenuPolicy(Qt::CustomContextMenu);
    setupToolBar();
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

    popOutAction = addAction("Pop Out/Restore");
    popOutAction->setCheckable(true);
    maximizeAction = addAction("Maximise/Minimise");
    maximizeAction->setCheckable(true);
    closeAction = addAction("Close");

    setIconSize(QSize(16,16));
}

