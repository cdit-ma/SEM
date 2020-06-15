#include "consolemonitor.h"
#include "../../theme.h"

#include <QBoxLayout>

ConsoleMonitor::ConsoleMonitor(QWidget* parent)
    : Monitor(parent)
{
    setupLayout();

    connect(Theme::theme(), &Theme::theme_Changed, this, &ConsoleMonitor::themeChanged);
    themeChanged();

    connect(this, &Monitor::StateChanged, this, &ConsoleMonitor::stateChanged);
    StateChanged(Notification::Severity::RUNNING);
}

void ConsoleMonitor::stateChanged(Notification::Severity state)
{
    //Abortable
    abort_action->setEnabled(state == Notification::Severity::RUNNING);
}

void ConsoleMonitor::setupLayout()
{
    auto layout = new QVBoxLayout(this);
    layout->setMargin(2);
    layout->setSpacing(2);

    editor = new QTextBrowser(this);
    
    toolbar = new QToolBar(this);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    abort_action = toolbar->addAction("Abort");
    clear_action = toolbar->addAction("Clear");

    connect(this, &ConsoleMonitor::AppendLine, editor, &QTextBrowser::append);
    connect(this, &ConsoleMonitor::Clear, editor, &QTextBrowser::clear);

    connect(abort_action, &QAction::triggered, this, &Monitor::Abort);
    connect(clear_action, &QAction::triggered, this, &Monitor::Clear);

    layout->addWidget(editor, 1);
    layout->addWidget(toolbar, 0, Qt::AlignRight);
}

void ConsoleMonitor::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getWidgetStyleSheet("ConsoleMonitor"));
    editor->setStyleSheet(theme->getLineEditStyleSheet("QTextBrowser"));

    clear_action->setIcon(theme->getIcon("Icons", "bin"));
    abort_action->setIcon(theme->getIcon("Icons", "circleCrossDark"));
    toolbar->setIconSize(theme->getIconSize());
}