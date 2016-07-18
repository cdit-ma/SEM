#include "docktitlebarwidget.h"
#include "theme.h"

DockTitleBarWidget::DockTitleBarWidget(QString title, QWidget *parent) : QToolBar(parent)
{
    setupToolBar();
    setIcon(Theme::theme()->getImage("Items","AssemblyDefinitions",QSize(16,16)));
    setTitle(title);

    //themeChanged();
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(closeAction, SIGNAL(triggered(bool)), this, SIGNAL(closeWidget()));
    connect(maximizeAction, SIGNAL(triggered(bool)), this, SIGNAL(maximizeWidget()));
    connect(minimizeAction, SIGNAL(triggered(bool)), this, SIGNAL(minimizeWidget()));
    connect(popInAction, SIGNAL(triggered(bool)), this, SIGNAL(popInWidget()));
    connect(popOutAction, SIGNAL(triggered(bool)), this, SIGNAL(popOutWidget()));
}

void DockTitleBarWidget::setIcon(QPixmap icon)
{
    this->icon->setPixmap(icon);
}

void DockTitleBarWidget::setTitle(QString title)
{
    this->title->setText(title);
}

void DockTitleBarWidget::themeChanged()
{
    Theme* theme = Theme::theme();

    QString altBGColor = theme->getAltBackgroundColorHex();
    QString textColor = theme->getTextColorHex(Theme::CR_NORMAL);
    QString highlightColor = theme->getHighlightColorHex();
    QString pressedColor = theme->getPressedColorHex();

    setStyleSheet("QToolBar{margin:1px 0px;background: " + altBGColor + ";border:1px solid gray;}"
                  "QToolButton {"
                  "padding:0px;"
                  "border: none;"
                  "background:" + altBGColor + ";"
                  "}"
                  "QToolButton:hover {"
                  "background:" + highlightColor + ";"
                  "}"
                  "QToolButton:pressed {background:" + pressedColor + "; }"
                  );

    title->setStyleSheet("color:" + textColor +";");

    closeAction->setIcon(theme->getIcon("Actions", "Close"));
    popInAction->setIcon(theme->getIcon("Actions", "Popup"));
    popOutAction->setIcon(theme->getIcon("Actions", "Popup"));
    maximizeAction->setIcon(theme->getIcon("Actions", "Maximize"));
    minimizeAction->setIcon(theme->getIcon("Actions", "Minimize"));
}

void DockTitleBarWidget::setupToolBar()
{
    icon = new QLabel(this);
    icon->setVisible(false);
    icon->setFixedSize(16,16);
    title = new QLabel(this);

    QWidget* widget = new QWidget();
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    addWidget(icon);
    addWidget(title);
    addWidget(widget);

    popInAction = addAction("PopIn");
    popOutAction = addAction("PopOut");
    //popInAction->setVisible(false);
    //popOutAction->setVisible(false);
    minimizeAction = addAction("Min");
    maximizeAction = addAction("Max");
    closeAction = addAction("Close");

    setIconSize(QSize(16,16));
}

