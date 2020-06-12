#include "loadmorewidget.h"
#include "../theme.h"

LoadMoreWidget::LoadMoreWidget()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setupLayout();
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &LoadMoreWidget::themeChanged);
    themeChanged();
}

void LoadMoreWidget::themeChanged()
{
    auto theme = Theme::theme();
    load_more_action->setIcon(theme->getIcon("Icons", "refresh"));
    setStyleSheet(theme->getToolBarStyleSheet() + "QToolButton{border:none;;}");
}

void LoadMoreWidget::setupLayout()
{
    auto load_more_button = new QToolButton(this);
    load_more_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    load_more_button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    load_more_button->setStyleSheet("QToolButton{ background: rgba(0,0,0,0); border: none; }");

    load_more_action = addWidget(load_more_button);
    load_more_action->setText("Load more");
	load_more_button->setDefaultAction(load_more_action);
	connect(load_more_action, &QAction::triggered, this, &LoadMoreWidget::LoadMore);
}