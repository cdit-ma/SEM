#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QMainWindow>
#include "../../Controllers/WindowManager/windowmanager.h"
#include "../DockWidgets/basedockwidget.h"

class BaseWindow : public QMainWindow{

    Q_OBJECT
friend class WindowManager;

public:
    enum WindowType {MAIN_WINDOW, VIEW_WINDOW};
protected:
    BaseWindow(QWidget* parent = 0, WindowType type = VIEW_WINDOW);
    ~BaseWindow();

public:
    bool hasDockWidgets();
    QList<BaseDockWidget*> getDockWidgets();
    int getID();
    WindowType getType();

    void setDockWidgetsVisible(bool visible);
    void addDockWidget(BaseDockWidget *widget);

    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget);
    virtual void addDockWidget(Qt::DockWidgetArea area, QDockWidget* widget, Qt::Orientation orientation);
    void removeDockWidget(QDockWidget* widget);

    void setDockWidgetVisibility(BaseDockWidget* widget, bool visible);
    QMenu *createPopupMenu();

signals:
    void dockWidgetAdded(BaseDockWidget* widget);
private slots:
    void themeChanged();
    void showContextMenu(const QPoint &point);

    void setDockWidgetMaximized(int ID, bool maximized);
    void _setDockWidgetVisibility(int ID, bool visible);
public slots:
    void tryClose();

private:
    void removeAllDockWidgets();
    void updateActions();
protected:
    void resetDockWidgets();
public:
    QList<int> previouslyVisibleDockIDs;
    WindowType windowType;

    QHash<int, BaseDockWidget*> currentDockWidgets;
    QList<BaseDockWidget*> ownedDockWidgets;

protected:
    void closeEvent(QCloseEvent *);
private:
    int ID;
    bool terminating;
    static int _WindowID;
    QAction* reset_action;
    // QWidget interface
protected:
    bool focusNextPrevChild(bool next);
};

#endif //BASEWINDOW_H


