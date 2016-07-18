#ifndef DOCKTITLEBARWIDGET_H
#define DOCKTITLEBARWIDGET_H

#include <QToolBar>
#include <QLabel>
#include <QAction>
#include <QDockWidget>

class DockTitleBarWidget : public QToolBar
{
    Q_OBJECT
public:
    explicit DockTitleBarWidget(QString title = "", QWidget *parent = 0);

    void setIcon(QPixmap pixmap);
    void setTitle(QString title);

signals:
    void maximizeWidget();
    void minimizeWidget();
    void popInWidget();
    void popOutWidget();
    void closeWidget();
    void renameWidget();

private slots:
    void themeChanged();
    void toolButtonTriggered();

private:
    void setupToolBar();

private:
    QLabel* title;
    QLabel* icon;

    QAction* closeAction;
    QAction* maximizeAction;
    QAction* minimizeAction;
    QAction* popInAction;
    QAction* popOutAction;
};

#endif // DOCKTITLEBARWIDGET_H
