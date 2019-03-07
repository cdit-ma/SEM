#include "basedockwidget.h"

#include <QDebug>
#include <QEvent>
#include <QMouseEvent>

#include "docktitlebar.h"
#include "../../theme.h"
#include "../Windows/basewindow.h"

int BaseDockWidget::_DockWidgetID = 0;
BaseDockWidget::BaseDockWidget(BaseDockType type, QWidget* parent):QDockWidget(parent)
{
    //TODO: Setting QDockWidget parent as the MainWindow fixes application shortcut issues
    ID = ++_DockWidgetID;

    setProperty("ID", ID);
    this->type = type;
    sourceWindow = 0;
    currentWindow = 0;
    initialArea = Qt::TopDockWidgetArea;
    _isActive = false;

    _isFocusEnabled = false;
    titleBar = new DockTitleBar(this);
    setTitleBarWidget(titleBar);

    connect(this, &QDockWidget::visibilityChanged, this, &BaseDockWidget::_visibilityChanged);
    _isProtected = false;

    setContextMenuPolicy(Qt::CustomContextMenu);

    if(titleBar){
        //Do connects.
        connect(titleBar->getAction(DockTitleBar::DA_CLOSE), &QAction::triggered, this, &BaseDockWidget::title_Close);
        connect(titleBar->getAction(DockTitleBar::DA_MAXIMIZE), &QAction::triggered, this, &BaseDockWidget::title_Maximize);
        connect(titleBar->getAction(DockTitleBar::DA_POPOUT), &QAction::triggered, this, &BaseDockWidget::title_PopOut);
        connect(titleBar->getAction(DockTitleBar::DA_HIDE), &QAction::triggered, this, &BaseDockWidget::title_Visible);

        connect(titleBar, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
        titleBar->installEventFilter(this);
    }

    connect(Theme::theme(), &Theme::theme_Changed, this, &BaseDockWidget::themeChanged);
    visibilityChanged(false);

    // this adds a border to the dock widgets when they are floating
    borderFrame = new QFrame(this);
    borderFrame->setStyleSheet("border-radius: " + Theme::theme()->getSharpCornerRadius() + "; border: 1px outset gray;");
    borderFrame->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    borderFrame->hide();
    connect(this, &BaseDockWidget::topLevelChanged, borderFrame, &QFrame::setVisible);
}

BaseDockWidget::~BaseDockWidget()
{

}

void BaseDockWidget::themeChanged(){
    
    setIcon(titleIcon.first, titleIcon.second);
}


int BaseDockWidget::getID() const
{
    return ID;
}

BaseDockType BaseDockWidget::getBaseDockType() const
{
    return type;
}

Qt::DockWidgetArea BaseDockWidget::getDockWidgetArea()
{
    return initialArea;
}

void BaseDockWidget::setDockWidgetArea(Qt::DockWidgetArea area)
{
    initialArea = area;
}

void BaseDockWidget::setSourceWindow(BaseWindow *window)
{
    sourceWindow = window;
}

BaseWindow *BaseDockWidget::getSourceWindow()
{
    return sourceWindow;
}

QPair<QString, QString> BaseDockWidget::getIcon()
{
    return titleIcon;
}

DockTitleBar *BaseDockWidget::getTitleBar()
{
    return titleBar;
}

void BaseDockWidget::removeTitleBar(){
    if(titleBar){
        delete titleBar;
        titleBar = 0;
    }
}


bool BaseDockWidget::isProtected() const
{
    return _isProtected;
}

void BaseDockWidget::setProtected(bool protect)
{
    _isProtected = protect;
    setCloseVisible(!protect);
}

void BaseDockWidget::setWidget(QWidget *w)
{
    if(widget()){
        widget()->removeEventFilter(this);
        widget()->setFocusProxy(0);
    }
    if(w){
        w->installEventFilter(this);
        setFocusProxy(w);
    }
    QDockWidget::setWidget(w);
}

void BaseDockWidget::setCurrentWindow(BaseWindow *window)
{
    currentWindow = window;
}

BaseWindow *BaseDockWidget::getCurrentWindow()
{
    return currentWindow;
}

void BaseDockWidget::setIcon(QPair<QString, QString> pair)
{

    setIcon(pair.first, pair.second);
}

void BaseDockWidget::setIcon(QString prefix, QString alias)
{
    if(titleBar){
        titleIcon.first = prefix;
        titleIcon.second = alias;
        titleBar->setIcon(prefix, alias);

        
        emit iconChanged(); 

        auto action = toggleViewAction();
        action->setIcon(Theme::theme()->getIcon(prefix, alias));
    }
}

void BaseDockWidget::setTitle(QString title, Qt::Alignment alignment)
{
    if(titleBar){
        titleBar->setTitle(title, alignment);
        QDockWidget::setWindowTitle(title);
        emit titleChanged();
    }
    this->title = title;
}

QString BaseDockWidget::getTitle() const
{
    return this->title;
}

void BaseDockWidget::setActive(bool active)
{
    if (_isActive != active) {
        _isActive = active;
        titleBar->setActive(active);
        themeChanged();
        emit dockSetActive(active);
        if(active){
            raise();
            widget()->setFocus();
        }
    }
}

bool BaseDockWidget::isActive()
{
    return _isActive;
}

void BaseDockWidget::setFocusEnabled(bool enabled)
{
    _isFocusEnabled = enabled;
}


void BaseDockWidget::setIconVisible(bool visible)
{
    setActionVisible(DockTitleBar::DA_ICON, visible);
}

void BaseDockWidget::setCloseVisible(bool visible)
{
    setActionVisible(DockTitleBar::DA_CLOSE, visible);
}

void BaseDockWidget::setHideVisible(bool visible)
{
    setActionVisible(DockTitleBar::DA_HIDE, visible);
}

void BaseDockWidget::setMaximizeVisible(bool visible)
{
    setActionVisible(DockTitleBar::DA_MAXIMIZE, visible);
}

void BaseDockWidget::setPopOutVisible(bool visible)
{
    setActionVisible(DockTitleBar::DA_POPOUT, visible);
}


void BaseDockWidget::setMaximizeToggled(bool toggled)
{
    setActionToggled(DockTitleBar::DA_MAXIMIZE, toggled);
}



void BaseDockWidget::close()
{
    title_Close(true);
}

void BaseDockWidget::title_Maximize(bool maximize)
{
    emit req_Maximize(ID, maximize);
}

void BaseDockWidget::title_Visible(bool visible)
{
    emit req_Visible(ID, visible);
}

void BaseDockWidget::title_PopOut(bool)
{
    emit req_PopOut(ID);
}

void BaseDockWidget::title_Close(bool)
{
    emit req_Close(ID);
}

void BaseDockWidget::_visibilityChanged(bool visible)
{
    setActionToggled(DockTitleBar::DA_HIDE, visible);
}

void BaseDockWidget::destruct()
{
    WindowManager::manager()->destructDockWidget(this);
}

void BaseDockWidget::showContextMenu(const QPoint &point)
{
    if(parentWidget()){
        parentWidget()->customContextMenuRequested(mapToParent(point));
    }
}

void BaseDockWidget::closeOrHide()
{
    QAction* a = getAction(DockTitleBar::DA_CLOSE);
    if(a && a->isVisible()){
        title_Close(false);
    }else{
        title_Visible(false);
    }
}


void BaseDockWidget::setActionVisible(DockTitleBar::DOCK_ACTION action, bool visible)
{
    QAction* a = getAction(action);
    if(a){
        a->setVisible(visible);
    }
}

void BaseDockWidget::setActionToggled(DockTitleBar::DOCK_ACTION action, bool toggled)
{
    QAction* a = getAction(action);
    if(a){
        a->setChecked(toggled);
    }
}

QAction *BaseDockWidget::getAction(DockTitleBar::DOCK_ACTION action)
{
    QAction* a = 0;
    if(titleBar){
        a = titleBar->getAction(action);
    }
    return a;
}

bool BaseDockWidget::eventFilter(QObject *object, QEvent *event)
{
    if(object == titleBar && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if(mouseEvent->button() == Qt::MiddleButton){
            closeOrHide();
            return true;
        }
    }

    return QObject::eventFilter(object, event);
}

void BaseDockWidget::resizeEvent(QResizeEvent *event)
{
    borderFrame->setFixedSize(this->size());
    QDockWidget::resizeEvent(event);
}
