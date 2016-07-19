#include "docktitlebarwidget.h"
#include "theme.h"
#include <QMainWindow>
#include <QDebug>
#include <QDrag>
#include <QMouseEvent>
#include <QMimeData>
#include <QMenu>
#include "../Widgets/New/medeawindownew.h"

DockTitleBarWidget::DockTitleBarWidget(QDockWidget *dockWidget, QString title) : QToolBar(dockWidget)
{

    setupToolBar();
    setIcon(Theme::theme()->getImage("Items","AssemblyDefinitions",QSize(16,16)));
    setTitle(title);

    subWindow = 0;

    currentArea = Qt::TopDockWidgetArea;
    mainWindow = 0;
    if(dockWidget->parentWidget()){
        mainWindow = (QMainWindow*)dockWidget->parentWidget();
    }
    currentWindow = mainWindow;


    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));


    connect(closeAction, SIGNAL(triggered(bool)), this, SIGNAL(closeWidget()));
    connect(maximizeAction, SIGNAL(triggered(bool)), this, SIGNAL(maximizeWidget()));
    connect(minimizeAction, SIGNAL(triggered(bool)), this, SIGNAL(minimizeWidget()));
    connect(popInAction, SIGNAL(triggered(bool)), this, SIGNAL(popInWidget()));
    //connect(popOutAction, SIGNAL(triggered(bool)), this, SIGNAL(popOutWidget()));
    connect(popOutAction, SIGNAL(triggered(bool)), this, SLOT(popout()));
    connect(popInAction, SIGNAL(triggered(bool)), this, SLOT(restore()));



    connect(maximizeAction, SIGNAL(triggered(bool)), this, SLOT(toolButtonTriggered()));
    connect(minimizeAction, SIGNAL(triggered(bool)), this, SLOT(toolButtonTriggered()));
    connect(popInAction, SIGNAL(triggered(bool)), this, SLOT(toolButtonTriggered()));
    connect(popOutAction, SIGNAL(triggered(bool)), this, SLOT(toolButtonTriggered()));



    if(dockWidget){
        //Connect
        dockWidget->setTitleBarWidget(this);
    }
    this->dockWidget = dockWidget;
    connect(dockWidget, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this, SLOT(dockWidgetAreaChanged(Qt::DockWidgetArea)));

    connect(dockWidget, SIGNAL(topLevelChanged(bool)), this, SLOT(widgetFloating(bool)));

}

QDockWidget *DockTitleBarWidget::getDockWidget()
{
    return dockWidget;
}

void DockTitleBarWidget::setIcon(QPixmap pixmap)
{
    icon->setPixmap(pixmap);
}

void DockTitleBarWidget::setTitle(QString title)
{
    this->title->setText(title);
}

void DockTitleBarWidget::setOriginalWindow(QMainWindow *original)
{
    mainWindow = original;
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

void DockTitleBarWidget::popout()
{
    if(subWindow == 0){
        subWindow = new MedeaWindowNew();
    }
    attachToWindow(subWindow);
}

void DockTitleBarWidget::restore()
{
    if(mainWindow != 0){
        attachToWindow(mainWindow);
    }
}

void DockTitleBarWidget::widgetFloating(bool floating)
{
}

void DockTitleBarWidget::dockWidgetAreaChanged(Qt::DockWidgetArea area)
{
    currentArea = area;
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

void DockTitleBarWidget::attachToWindow(QMainWindow *window)
{
    window->addDockWidget(currentArea, dockWidget);
    currentWindow=window;
    window->show();
}

void DockTitleBarWidget::togglePopIn(bool isFloating)
{

}

void DockTitleBarWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && title->rect().contains(event->pos())){
        QDrag *drag = new QDrag(this);



        QMimeData *mimeData = new QMimeData();
        mimeData->setData(MIME_DRAGWIDGET, QByteArray());
        drag->setMimeData(mimeData);
        drag->setPixmap(dockWidget->grab());

        //Start the drop.
        Qt::DropAction dropAction = drag->exec();
        qCritical() << dropAction;
        if(dropAction == Qt::MoveAction){
            QObject* target = drag->target();
            if(target){
                MedeaWindowNew* window = dynamic_cast<MedeaWindowNew*>(target);
                if(window){
                    attachToWindow(window);
                }
            }
        }else{
                if(subWindow == 0){
                    subWindow = new MedeaWindowNew();
                }
                attachToWindow(subWindow);
        }
    }
    if(event->button() == Qt::RightButton){
        qCritical() << "GG";
        currentWindow->createPopupMenu()->exec(event->screenPos().toPoint());
    }
}

