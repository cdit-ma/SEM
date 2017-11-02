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
    ConsoleMonitor(QWidget *parent = 0);
private:
    void stateChanged(Notification::Severity state);
    void setupLayout();
    void themeChanged();

    QTextBrowser* editor = 0;
    QToolBar* toolbar = 0;
    QAction* abort_action = 0;
    QAction* clear_action = 0;
};

#endif // CONSOLEMONITOR_H
