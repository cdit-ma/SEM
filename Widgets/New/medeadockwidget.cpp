#include "medeadockwidget.h"
#include "../../View/docktitlebarwidget.h"
#include "../../View/theme.h"
#include "medeawindownew.h"
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
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

    connect(this, &QDockWidget::visibilityChanged, this, &MedeaDockWidget::_visibilityChanged);
    _isProtected = false;

    setContextMenuPolicy(Qt::CustomContextMenu);


    if(titleBar){
        //Do connects.
        connect(titleBar->getAction(DockTitleBarWidget::DA_CLOSE), &QAction::triggered, this, &MedeaDockWidget::title_Close);
        connect(titleBar->getAction(DockTitleBarWidget::DA_MAXIMIZE), &QAction::triggered, this, &MedeaDockWidget::title_Maximize);
        connect(titleBar->getAction(DockTitleBarWidget::DA_POPOUT), &QAction::triggered, this, &MedeaDockWidget::title_PopOut);
        connect(titleBar->getAction(DockTitleBarWidget::DA_HIDE), &QAction::triggered, this, &MedeaDockWidget::title_Visible);

        connect(titleBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
        titleBar->installEventFilter(this);
    }

    connect(Theme::theme(), &Theme::theme_Changed, this, &MedeaDockWidget::themeChanged);
    visibilityChanged(false);
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

DockTitleBarWidget *MedeaDockWidget::getTitleBar()
{
    return titleBar;
}

void MedeaDockWidget::setTitleBarIconSize(int height)
{
    if(titleBar){
        titleBar->setToolBarIconSize(height);
    }
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

void MedeaDockWidget::setIcon(QPair<QString, QString> pair)
{
    setIcon(pair.first, pair.second);
}

void MedeaDockWidget::setIcon(QString prefix, QString alias)
{
    if(titleBar){
        titleBar->setIcon(Theme::theme()->getImage(prefix, alias, QSize(16,16)));
    }
}

void MedeaDockWidget::setTitle(QString title, Qt::Alignment alignment)
{
    if(titleBar){
        titleBar->setTitle(title, alignment);
        QDockWidget::setWindowTitle(title);
        emit titleChanged();
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
        titleBar->setActive(active);
        themeChanged();
        emit dockSetActive(active);
    }
}

bool MedeaDockWidget::isActive()
{
    return _isActive;
}

void MedeaDockWidget::setMaximized(bool maximized)
{
    setVisible(maximized);
    setMaximizeToggled(maximized);
    setMaximizeEnabled(!maximized);
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

void MedeaDockWidget::setHideVisible(bool visible)
{
    setActionVisible(DockTitleBarWidget::DA_HIDE, visible);
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

void MedeaDockWidget::setMaximizeEnabled(bool enabled)
{
    setActionEnabled(DockTitleBarWidget::DA_MAXIMIZE, enabled);

}

void MedeaDockWidget::setPopOutToggled(bool toggled)
{
    setActionToggled(DockTitleBarWidget::DA_POPOUT, toggled);
}

void MedeaDockWidget::setProtectToggled(bool toggled)
{
    setActionToggled(DockTitleBarWidget::DA_PROTECT, toggled);
}

void MedeaDockWidget::close()
{
    title_Close(true);
}

void MedeaDockWidget::title_Maximize(bool maximize)
{
    emit req_Maximize(ID, maximize);
}

void MedeaDockWidget::title_Visible(bool visible)
{
    emit req_Visible(ID, visible);
}

void MedeaDockWidget::title_PopOut(bool)
{
    emit req_PopOut(ID);
}

void MedeaDockWidget::title_Close(bool)
{
    emit req_Close(ID);
}

void MedeaDockWidget::_visibilityChanged(bool visible)
{
    setActionToggled(DockTitleBarWidget::DA_HIDE, visible);
}

void MedeaDockWidget::destruct()
{
    MedeaWindowManager::destructDockWidget(this);
}

void MedeaDockWidget::showContextMenu(const QPoint &point)
{
    if(parentWidget()){
        parentWidget()->customContextMenuRequested(mapToParent(point));
    }
}

void MedeaDockWidget::closeOrHide()
{
    QAction* a = getAction(DockTitleBarWidget::DA_CLOSE);
    if(a && a->isVisible()){
        title_Close(false);
    }else{
        title_Visible(false);
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

void MedeaDockWidget::setActionEnabled(DockTitleBarWidget::DOCK_ACTION action, bool enabled)
{
    QAction* a = getAction(action);
    if(a){
        a->setEnabled(enabled);
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

    if(object == titleBar && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if(mouseEvent->button() == Qt::MiddleButton){
            closeOrHide();
            return true;
        }
    }

    return QObject::eventFilter(object, event);
}
