#include "filterwidget.h"
#include <QHBoxLayout>
#include "../theme.h"
#include <QDebug>

FilterWidget::FilterWidget(){
    setupLayout();
    
    connect(line_edit, &QLineEdit::textChanged, this, &FilterWidget::filterChanged);
    
    setFocusProxy(line_edit);
    setFocusPolicy(Qt::StrongFocus);
    line_edit->installEventFilter(this);

    setToolTip("Filter the Menu");

    connect(Theme::theme(), &Theme::theme_Changed, this, &FilterWidget::themeChanged);
    themeChanged();
}

void FilterWidget::themeChanged(){
    auto theme = Theme::theme();
    line_edit->setStyleSheet(theme->getLineEditStyleSheet()  + "QLineEdit{border-radius: " + theme->getSharpCornerRadius() + ";}");
}

void FilterWidget::setupLayout(){
    auto layout = new QHBoxLayout(this);
    layout->setMargin(3);

    line_edit = new QLineEdit(this);
    line_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    line_edit->setAttribute(Qt::WA_MacShowFocusRect, false);
    line_edit->setPlaceholderText("Filter Entities...");

    line_edit->setFocusPolicy(Qt::StrongFocus);
    line_edit->setFocus();

    layout->addWidget(line_edit);
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
