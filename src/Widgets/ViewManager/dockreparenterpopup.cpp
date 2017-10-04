#include "dockreparenterpopup.h"
#include <QBoxLayout>
#include <QScrollArea>
#include <QToolButton>
#include <QScrollBar>
#include <QPainter>
#include "../../Controllers/WindowManager/windowmanager.h"
#include "../../Widgets/DockWidgets/basedockwidget.h"
#include "../../Widgets/Windows/basewindow.h"
#include "../../theme.h"

DockReparenterPopup::DockReparenterPopup():PopupWidget(PopupWidget::TYPE::DIALOG, 0){
    setupLayout();
    connect(Theme::theme(), &Theme::theme_Changed, this, &DockReparenterPopup::themeChanged);
    themeChanged();
}

bool DockReparenterPopup::ReparentDockWidget(BaseDockWidget* dock_widget){
    if(dock_widget){
        auto manager = WindowManager::manager();
        this->dock_widget = dock_widget;

        //Hide all current buttons
        for(auto button : button_lookup.values()){
            button->hide();
        }
        
        //Add a new Window Button first
        auto new_window_button = getWindowAction(0);
        //Construct a CentralWindow widget second will ensure that it'll be second if it's valid
        getWindowAction(manager->getCentralWindow());

        //Remove the dockWindows current Window, and the MainWindow from the windows this dock_widget can be put into
        auto valid_windows = manager->getWindows().toSet();
        valid_windows.remove(dock_widget->getCurrentWindow());
        valid_windows.remove(manager->getMainWindow());
        
        //New Window should always be visible
        new_window_button->show();
        //Add New Buttons for the other valid windows, and show them
        for(auto window : valid_windows){
            if(window->getType() != BaseWindow::INVISIBLE_WINDOW){
                auto window_button = getWindowAction(window);
                window_button->show();
            }
        }
    
        //Forces a blocking size adjustment to ensure its the right width.
        QMetaObject::invokeMethod(this, "adjustSize", Qt::QueuedConnection);
        //Centralize it
        WindowManager::MoveWidget(this);
        //Exec it as modal, the windowTriggered function will accept/reject the modality
        return exec();
    }
    return false;
}

void DockReparenterPopup::windowTriggered(int window_id){
    if(dock_widget){
        auto manager = WindowManager::manager();
        BaseWindow* window = 0;
        
        if(window_id >= 0){
            window = manager->getWindow(window_id);
        }else if(window_id == -1){
            window = manager->constructSubWindow();
            window->setWindowTitle("Sub Window #" + QString::number(window->getID() - 2));
            window->show();
            window->activateWindow();
        }else{
            //DO NOTHING
        }

        if(window && manager->reparentDockWidget(dock_widget, window)){
            accept();
        }
        //Unset the dock_widget
        dock_widget = 0;
    }
}

QToolButton* DockReparenterPopup::getWindowAction(BaseWindow* window){
    int window_id = -1;

    if(window){
        window_id = window->getID();
    }
    
    auto button = button_lookup.value(window_id, 0);
    if(!button){
        button = new QToolButton(this);
        button->setIconSize(QSize(96,96));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setFixedHeight(120);
        button->hide();
        
        button_lookup[window_id] = button;
        h_layout->addWidget(button, 1, Qt::AlignCenter);
        connect(button, &QToolButton::clicked, [=](){windowTriggered(window_id);});
    }

    if(window){
        auto window_pix = window->grab().scaled(QSize(96,96), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        auto icon_pix = QPixmap(96,96);
        icon_pix.fill(Qt::transparent);

        int x_offset = (icon_pix.width() - window_pix.width()) / 2;
        int y_offset = (icon_pix.height() - window_pix.height()) / 2;
        {
            QPainter painter(&icon_pix);
            painter.drawPixmap(x_offset, y_offset, window_pix);
        }
        button->setIcon(QIcon(icon_pix));
        button->setText(window->windowTitle());
    }else{
        button->setIcon(Theme::theme()->getIcon("Icons", "medeaLogo"));
        button->setText("New Window");
    }
    return button;
}

void DockReparenterPopup::themeChanged(){
    auto theme = Theme::theme();
    setStyleSheet(theme->getToolBarStyleSheet() + theme->getLabelStyleSheet() + "QScrollArea {background: transparent;}" + theme->getScrollBarStyleSheet() + "QFrame{background:transparent;}  QLabel{}");
    close_action->setIcon(theme->getIcon("Icons", "cross"));
    toolbar->setIconSize(theme->getIconSize());
}

void DockReparenterPopup::reject(){
    windowTriggered(-2);
    QDialog::reject();
}

void DockReparenterPopup::setupLayout(){
    auto parent_widget = new QWidget(this);
    parent_widget->setMinimumWidth(150);
    parent_widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));


    auto layout = new QVBoxLayout(parent_widget);
    layout->setMargin(2);
    layout->setSpacing(5);

    toolbar = new QToolBar(this);
    toolbar->setStyleSheet("QToolButton:!hover{border:none;background:transparent;}");
    auto label_title = new QLabel("Select Window", this);

    label_title->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    label_title->setAlignment(Qt::AlignCenter);
    
    toolbar->addWidget(label_title);
    close_action = toolbar->addAction("Exit");

    
    
    h_layout = new QHBoxLayout();
    h_layout->setMargin(0);
    h_layout->setSpacing(5);
    
    layout->addWidget(toolbar);
    layout->addLayout(h_layout, Qt::AlignHCenter);
    
    //Get the close action to call windowTriggered with -2
    connect(close_action, &QAction::triggered, this, &QDialog::reject);
    setWidget(parent_widget);
}