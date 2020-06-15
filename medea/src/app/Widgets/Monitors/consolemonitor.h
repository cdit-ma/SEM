#ifndef CONSOLEMONITOR_H
#define CONSOLEMONITOR_H

#include "monitor.h"

#include <QFrame>
#include <QToolBar>
#include <QTextBrowser>

class ConsoleMonitor: public Monitor
{
    Q_OBJECT

public:
    explicit ConsoleMonitor(QWidget* parent = nullptr);

private:
    void stateChanged(Notification::Severity state);
    void setupLayout();
    void themeChanged();

    QTextBrowser* editor = nullptr;
    QToolBar* toolbar = nullptr;
    QAction* abort_action = nullptr;
    QAction* clear_action = nullptr;
};

#endif // CONSOLEMONITOR_H