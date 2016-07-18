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

    connect(maximizeAction, SIGNAL(triggered(bool)), this, SLOT(toolButtonTriggered()));
    connect(minimizeAction, SIGNAL(triggered(bool)), this, SLOT(toolButtonTriggered()));
    connect(popInAction, SIGNAL(triggered(bool)), this, SLOT(toolButtonTriggered()));
    connect(popOutAction, SIGNAL(triggered(bool)), this, SLOT(toolButtonTriggered()));
}

void DockTitleBarWidget::setIcon(QPixmap pixmap)
{
    icon->setPixmap(pixmap);
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
    popInAction->setIcon(theme->getIcon("Actions", "Download"));
    popOutAction->setIcon(theme->getIcon("Actions", "Popup"));
    maximizeAction->setIcon(theme->getIcon("Actions", "Maximize"));
    minimizeAction->setIcon(theme->getIcon("Actions", "Minimize"));
}

void DockTitleBarWidget::toolButtonTriggered()
{
    QAction* senderAction = qobject_cast<QAction*>(QObject::sender());
    QAction* pairAction = 0;
    if (senderAction == popInAction) {
        pairAction = popOutAction;
    } else if (senderAction == popOutAction) {
        pairAction = popInAction;
    } else if (senderAction == minimizeAction) {
        pairAction = maximizeAction;
    } else if (senderAction == maximizeAction) {
        pairAction = minimizeAction;
    }
    if (pairAction) {
        pairAction->setVisible(true);
        senderAction->setVisible(false);
    }
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

    popInAction = addAction("Pop In");
    popInAction->setVisible(false);
    popOutAction = addAction("Pop Out");
    minimizeAction = addAction("Minimise");
    minimizeAction->setVisible(false);
    maximizeAction = addAction("Maximise");
    closeAction = addAction("Close");

    setIconSize(QSize(16,16));
}

