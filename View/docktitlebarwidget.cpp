#include "docktitlebarwidget.h"
#include "theme.h"
DockTitleBarWidget::DockTitleBarWidget(QWidget *parent) : QToolBar(parent)
{
    setupToolBar();
    setIcon(Theme::theme()->getImage("Items","AssemblyDefinitions",QSize(16,16)));
    setTitle("Interfaces");
    themeChanged();
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
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
                    "background:" + highlightColor +";"
                    "}"
                    "QToolButton:pressed {background:" + pressedColor + "; }"
                  );
    title->setStyleSheet("color:" +textColor +";");




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
    title = new QLabel(this);
    icon->setFixedSize(16,16);

    QWidget* widget = new QWidget();
    widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    addWidget(icon);
    addWidget(title);
    addWidget(widget);


    minimizeAction = addAction("Min");
    maximizeAction = addAction("Max");
    popInAction = addAction("PopIn");
    popOutAction = addAction("PopOut");
    popInAction->setVisible(false);
    popOutAction->setVisible(false);
    closeAction = addAction("Close");


    setIconSize(QSize(16,16));
}

