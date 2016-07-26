#include "medeadockwidget.h"
#include "../../View/docktitlebarwidget.h"
#include "../../View/theme.h"
#include "medeawindownew.h"
#include <QDebug>
#include <QEvent>
int MedeaDockWidget::_DockWidgetID = 0;
MedeaDockWidget::MedeaDockWidget(DOCKWIDGET_TYPE type):QDockWidget()
{
    ID = ++_DockWidgetID;
    this->type = type;
    sourceWindow = 0;
    currentWindow = 0;
    initialArea = Qt::TopDockWidgetArea;
    _isActive = false;

    _isFocusEnabled = false;
    titleBar = new DockTitleBarWidget(this);
    setTitleBarWidget(titleBar);

    _isProtected = false;

    setContextMenuPolicy(Qt::PreventContextMenu);

    if(titleBar){
        //Do connects.
        connect(titleBar->getAction(DockTitleBarWidget::DA_CLOSE), SIGNAL(triggered(bool)), this, SIGNAL(closeWidget()));
        connect(titleBar->getAction(DockTitleBarWidget::DA_MAXIMIZE), SIGNAL(triggered(bool)),this, SIGNAL(maximizeWidget(bool)));
        connect(titleBar->getAction(DockTitleBarWidget::DA_POPOUT), SIGNAL(triggered(bool)),this, SIGNAL(popOutWidget()));
        connect(titleBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
        titleBar->installEventFilter(this);
    }
    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    themeChanged();
}

MedeaDockWidget::~MedeaDockWidget()
{
}

int MedeaDockWidget::getID()
{
    return ID;
}

MedeaDockWidget::DOCKWIDGET_TYPE MedeaDockWidget::getDockType()
{
    return type;
}

Qt::DockWidgetArea MedeaDockWidget::getDockWidgetArea()
{
    return initialArea;
}

void MedeaDockWidget::setDockWidgetArea(Qt::DockWidgetArea area)
{
    initialArea = area;
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

void MedeaDockWidget::setWidget(QWidget *w)
{
    if(widget()){
        widget()->removeEventFilter(this);
        widget()->setFocusProxy(0);
    }
    w->installEventFilter(this);
    setFocusProxy(w);
    QDockWidget::setWidget(w);
}

void MedeaDockWidget::setCurrentWindow(MedeaWindowNew *window)
{
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
        QDockWidget::setWindowTitle(title);
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
    if (_isActive != active) {
        _isActive = active;
        updateActiveStyleSheet();
    }
}

bool MedeaDockWidget::isActive()
{
    return _isActive;
}

void MedeaDockWidget::setFocusEnabled(bool enabled)
{
    _isFocusEnabled = enabled;
}

bool MedeaDockWidget::isFocusEnabled()
{
    return _isFocusEnabled;
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

void MedeaDockWidget::setProtectVisible(bool visible)
{
    setActionVisible(DockTitleBarWidget::DA_PROTECT, visible);
}

void MedeaDockWidget::setMaximizeToggled(bool toggled)
{
    setActionToggled(DockTitleBarWidget::DA_MAXIMIZE, toggled);
}

void MedeaDockWidget::setPopOutToggled(bool toggled)
{
    setActionToggled(DockTitleBarWidget::DA_POPOUT, toggled);
}

void MedeaDockWidget::setProtectToggled(bool toggled)
{
    setActionToggled(DockTitleBarWidget::DA_PROTECT, toggled);
}


void MedeaDockWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    titleBar->setLabelStyleSheet("color:" + theme->getTextColorHex(Theme::CR_NORMAL) + ";");
    updateActiveStyleSheet();

    QAction* closeAction = getAction(DockTitleBarWidget::DA_CLOSE);
    QAction* maxAction = getAction(DockTitleBarWidget::DA_MAXIMIZE);
    QAction* popAction = getAction(DockTitleBarWidget::DA_POPOUT);
    QAction* protectAction = getAction(DockTitleBarWidget::DA_PROTECT);

    if(closeAction){
        closeAction->setIcon(theme->getIcon("Actions", "Close"));
    }
    if(maxAction){
        maxAction->setIcon(theme->getIcon("Actions", "DockMaximize"));
    }
    if(popAction){
        popAction->setIcon(theme->getIcon("Actions", "DockPopOut"));
    }
    if(protectAction){
        protectAction->setIcon(theme->getIcon("Actions", "Lock_Open"));
    }
}

void MedeaDockWidget::showContextMenu(const QPoint &point)
{
    if(parentWidget()){
        parentWidget()->customContextMenuRequested(mapToParent(point));
    }
}

void MedeaDockWidget::updateActiveStyleSheet()
{
    if (isActive()) {
        QString activeColor = Theme::theme()->getActiveWidgetBorderColorHex();
        setStyleSheet("QGraphicsView { border: 1px solid " + activeColor + ";}"
                      "QToolButton::!hover { background:" + activeColor +";}"
                      "QToolBar { background:" + activeColor + "; border: 1px solid " + activeColor + ";}");
    } else {
        setStyleSheet("");
        //setStyleSheet("QToolBar { border: 1px solid " + Theme::theme()->getAltBackgroundColorHex() + ";}");
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
    if(_isFocusEnabled && event->type() == QEvent::FocusIn){
        MedeaWindowManager::manager()->setActiveDockWidget(this);
    }
    return QObject::eventFilter(object, event);

}
