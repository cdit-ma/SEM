#include "consolewidget.h"
#include <QBoxLayout>
#include "../../theme.h"

ConsoleWidget::ConsoleWidget(QWidget* parent ){
    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &ConsoleWidget::themeChanged);
    themeChanged();
}

void ConsoleWidget::setCancelButtonVisible(bool visible){
    toolbar->setVisible(visible);
}

void ConsoleWidget::setupLayout(){
    auto layout = new QVBoxLayout(this);

    editor = new QTextBrowser(this);
    
    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    cancel_action = toolbar->addAction("Cancel");
    clear_action = toolbar->addAction("Clear");

    connect(this, &ConsoleWidget::AppendLine, editor, &QTextBrowser::append);
    connect(this, &ConsoleWidget::Clear, editor, &QTextBrowser::clear);

    connect(cancel_action, &QAction::triggered, this, &ConsoleWidget::Cancel);
    connect(clear_action, &QAction::triggered, this, &ConsoleWidget::Clear);

    layout->addWidget(editor, 1);
    layout->addWidget(toolbar, 0, Qt::AlignRight);
}

void ConsoleWidget::themeChanged(){
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getWidgetStyleSheet("ConsoleWidget"));
    editor->setStyleSheet(theme->getLineEditStyleSheet("QTextBrowser"));

    clear_action->setIcon(theme->getIcon("Icons", "bin"));
    cancel_action->setIcon(theme->getIcon("Icons", "circleCrossDark"));
    toolbar->setIconSize(theme->getIconSize());
}