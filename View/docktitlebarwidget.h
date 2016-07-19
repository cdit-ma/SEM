#ifndef DOCKTITLEBARWIDGET_H
#define DOCKTITLEBARWIDGET_H

#include <QToolBar>
#include <QLabel>
#include <QAction>
#include <QDockWidget>


#define MIME_DRAGWIDGET "application/MEDEA-DragWidget"

class DockTitleBarWidget : public QToolBar
{
    Q_OBJECT
public:
    explicit DockTitleBarWidget(QDockWidget *dockWidget, QString title="");
    QDockWidget* getDockWidget();

    void setIcon(QPixmap pixmap);
    void setTitle(QString title);

    void setOriginalWindow(QMainWindow* original);
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
    void popout();
    void restore();
    void widgetFloating(bool floating);

    void dockWidgetAreaChanged(Qt::DockWidgetArea area);
private:
    void setupToolBar();
    void attachToWindow(QMainWindow* window);

    void togglePopIn(bool isFloating);

private:
    QLabel* title;
    QLabel* icon;

    QMainWindow* mainWindow;
    QMainWindow* subWindow;
    QMainWindow* currentWindow;
    Qt::DockWidgetArea currentArea;

    QDockWidget* dockWidget;
    QAction* closeAction;
    QAction* maximizeAction;
    QAction* minimizeAction;
    QAction* popInAction;
    QAction* popOutAction;

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // DOCKTITLEBARWIDGET_H
