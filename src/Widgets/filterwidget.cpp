#include "filterwidget.h"
#include <QHBoxLayout>
#include "../theme.h"
#include <QDebug>

FilterWidget::FilterWidget() : QToolBar(0){
    setupLayout();
    
    connect(line_edit, &QLineEdit::textChanged, this, &FilterWidget::filterChanged);
    connect(reset_button, &QToolButton::clicked, line_edit, &QLineEdit::clear);
    
    setFocusProxy(line_edit);
    setFocusPolicy(Qt::StrongFocus);
    line_edit->installEventFilter(this);

    setToolTip("Filter the Menu");

    connect(Theme::theme(), &Theme::theme_Changed, this, &FilterWidget::themeChanged);
    themeChanged();
}

void FilterWidget::themeChanged(){
    auto theme = Theme::theme();
    setStyleSheet(theme->getToolBarStyleSheet() + " QToolButton{background:rgba(0,0,0,0);border:none;}");
    line_edit->setStyleSheet(theme->getLineEditStyleSheet());
    icon_button->setStyleSheet("QToolButton{background:rgba(0,0,0,0);border:none;}");
    reset_button->setIcon(theme->getIcon("Icons", "cross"));
    auto filter_icon = theme->getImage("Icons", "filterList", QSize(), theme->getMenuIconColor());
    icon_button->setIcon(filter_icon);
}

void FilterWidget::setupLayout(){
    icon_button = new QToolButton(this);
    //icon_button->setEnabled(false);
    icon_button->setFocusPolicy(Qt::NoFocus);
    reset_button = new QToolButton(this);
    //reset_button->setFocusPolicy(Qt::NoFocus);

    line_edit = new QLineEdit(this);
    line_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    line_edit->setAttribute(Qt::WA_MacShowFocusRect, false);
    line_edit->setPlaceholderText("Filter Entities...");

    line_edit->setFocusPolicy(Qt::StrongFocus);
    line_edit->setFocus();

    addWidget(icon_button);
    addWidget(line_edit);
    addWidget(reset_button);
}



bool FilterWidget::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::FocusIn){
        if(object == line_edit){
            QMetaObject::invokeMethod(line_edit, "selectAll", Qt::QueuedConnection);
        }
   }
   return false;
}
