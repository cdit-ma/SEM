#ifndef CONSOLE_WIDGET_H
#define CONSOLE_WIDGET_H

#include <QToolBar>
#include <QTextBrowser>

class ConsoleWidget: public QWidget
{
    Q_OBJECT
public:
    ConsoleWidget(QWidget *parent = 0);
    void setCancelButtonVisible(bool visible);
signals:
    void Clear();
    void Cancel();
    void AppendLine(QString text);
private:
    void setupLayout();
    void themeChanged();

    QTextBrowser* editor = 0;
    QToolBar* toolbar = 0;
    QAction* cancel_action = 0;
    QAction* clear_action = 0;
};

#endif // JENKINSJOBMONITORWIDGET_H
