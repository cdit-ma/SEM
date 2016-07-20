#include "medeadockwidget.h"
#include "../../View/docktitlebarwidget.h"
#include "../../View/theme.h"
#include "medeawindownew.h"
#include <QDebug>
#include <QEvent>
int MedeaDockWidget::_DockWidgetID = 0;
MedeaDockWidget::MedeaDockWidget(QString title, Qt::DockWidgetArea area, QWidget *parent):QDockWidget(title, parent)
{
    ID = ++_DockWidgetID;
    sourceWindow = 0;
    currentWindow = 0;
    initialArea = area;
    _isActive = false;

    titleBar = new DockTitleBarWidget(this);
    setTitleBarWidget(titleBar);

    _isProtected = false;

    setContextMenuPolicy(Qt::PreventContextMenu);
    setTitle(title);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    if(titleBar){
        //Do connects.
        connect(titleBar->getAction(DockTitleBarWidget::DA_CLOSE), SIGNAL(triggered(bool)), this, SIGNAL(closeWidget()));
        connect(titleBar->getAction(DockTitleBarWidget::DA_MAXIMIZE), SIGNAL(triggered(bool)),this, SIGNAL(maximizeWidget(bool)));
        connect(titleBar->getAction(DockTitleBarWidget::DA_POPOUT), SIGNAL(triggered(bool)),this, SIGNAL(popOutWidget(bool)));
        connect(titleBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
        titleBar->installEventFilter(this);
    }
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    themeChanged();
}

MedeaDockWidget::~MedeaDockWidget()
{
    delete titleBar;
}

int MedeaDockWidget::getID()
{
    return ID;
}

void MedeaDockWidget::setSourceWindow(MedeaWindowNew *window)
{
    sourceWindow = window;
}

MedeaWindowNew *MedeaDockWidget::getSourceWindow()
{
    return sourceWindow;
}

bool MedeaDockWidget::isProtected()
{
    return _isProtected;
}

void MedeaDockWidget::setProtected(bool protect)
{
    _isProtected = protect;
    setCloseVisible(!protect);
}

void MedeaDockWidget::setWidget(QWidget *widget)
{
    if(this->widget()){
        this->widget()->removeEventFilter(this);
        this->widget()->setFocusProxy(0);
    }
    widget->installEventFilter(this);
    this->setFocusProxy(widget);
    QDockWidget::setWidget(widget);
}

void MedeaDockWidget::setCurrentWindow(MedeaWindowNew *window)
{
    if (currentWindow) {
        currentWindow->removeMedeaDockWidget(this);
    }
    if (window) {
        window->addMedeaDockWidget(this, initialArea);
        if (!sourceWindow) {
            setSourceWindow(window);
        }
    }
    currentWindow = window;
}

MedeaWindowNew *MedeaDockWidget::getCurrentWindow()
{
    return currentWindow;
}

void MedeaDockWidget::setIcon(QPixmap icon)
{
    if(titleBar){
        titleBar->setIcon(icon);
    }
}

void MedeaDockWidget::setTitle(QString title, Qt::Alignment alignment)
{
    if(titleBar){
        titleBar->setTitle(title, alignment);
    }
}

QString MedeaDockWidget::getTitle()
{
    QString title = "";
    if(titleBar){
        title = titleBar->getTitle();
    }
    return title;
}

void MedeaDockWidget::setActive(bool active)
{
    if(_isActive != active){
        _isActive = active;
        updateTitleLabelStyle();
    }
}

bool MedeaDockWidget::isActive()
{
    return _isActive;
}

void MedeaDockWidget::setCloseVisible(bool visible)
{
    setActionVisible(DockTitleBarWidget::DA_CLOSE, visible);
}

void MedeaDockWidget::setMaximizeVisible(bool visible)
{
    setActionVisible(DockTitleBarWidget::DA_MAXIMIZE, visible);
}

void MedeaDockWidget::setPopOutVisible(bool visible)
{
    setActionVisible(DockTitleBarWidget::DA_POPOUT, visible);
}

void MedeaDockWidget::setMaximizeToggled(bool toggled)
{
    setActionToggled(DockTitleBarWidget::DA_MAXIMIZE, toggled);
}

void MedeaDockWidget::setPopOutToggled(bool toggled)
{
    setActionToggled(DockTitleBarWidget::DA_POPOUT, toggled);
}

void MedeaDockWidget::themeChanged()
{
    Theme* theme = Theme::theme();

    QString altBGColor = theme->getAltBackgroundColorHex();
    QString textColor = theme->getTextColorHex(Theme::CR_NORMAL);
    QString highlightColor = theme->getHighlightColorHex();
    QString pressedColor = theme->getPressedColorHex();

    setStyleSheet("QToolBar{margin:1px 0px;background: " + altBGColor + ";border:1px solid gray;}"
                "QToolBar#focussed{color:" + highlightColor + ";font-weight:bold;}"
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
    labelStyle_Normal = "QLabel{color:" + textColor + ";}";
    labelStyle_Focussed = "QLabel{color:" + highlightColor + ";font-weight:bold;}";

    updateTitleLabelStyle();


    QAction* closeAction = getAction(DockTitleBarWidget::DA_CLOSE);
    QAction* maxAction = getAction(DockTitleBarWidget::DA_MAXIMIZE);
    QAction* popAction = getAction(DockTitleBarWidget::DA_POPOUT);

    if(closeAction){
        closeAction->setIcon(theme->getIcon("Actions", "Close"));
    }
    if(maxAction){
        maxAction->setIcon(theme->getIcon("Actions", "DockMaximize"));
    }
    if(popAction){
        popAction->setIcon(theme->getIcon("Actions", "DockPopOut"));
    }
}

void MedeaDockWidget::showContextMenu(const QPoint &point)
{
    if(parentWidget()){
        parentWidget()->customContextMenuRequested(mapToParent(point));
    }
}

void MedeaDockWidget::updateTitleLabelStyle()
{
    if(titleBar){
        if(isActive()){
            titleBar->setLabelStyleSheet(labelStyle_Focussed);
        }else{
            titleBar->setLabelStyleSheet(labelStyle_Normal);
        }
    }
}
void MedeaDockWidget::setActionVisible(DockTitleBarWidget::DOCK_ACTION action, bool visible)
{
    QAction* a = getAction(action);
    if(a){
        a->setVisible(visible);
    }
}

void MedeaDockWidget::setActionToggled(DockTitleBarWidget::DOCK_ACTION action, bool toggled)
{
    QAction* a = getAction(action);
    if(a){
        a->setChecked(toggled);
    }
}

QAction *MedeaDockWidget::getAction(DockTitleBarWidget::DOCK_ACTION action)
{
    QAction* a = 0;
    if(titleBar){
        a = titleBar->getAction(action);
    }
    return a;
}

bool MedeaDockWidget::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::FocusIn){
        MedeaWindowManager::manager()->setActiveDockWidget(this);
    }
    return QObject::eventFilter(object, event);

}
