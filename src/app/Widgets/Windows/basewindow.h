#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QMainWindow>

#include "../../Controllers/WindowManager/windowmanager.h"
#include "../DockWidgets/basedockwidget.h"

class BaseWindow : public QMainWindow
{
    Q_OBJECT
    friend class WindowManager;

public:
    enum WindowType {MAIN_WINDOW, VIEW_WINDOW, INVISIBLE_WINDOW};

    QList<BaseDockWidget*> getDockWidgets();
    int getID();
    WindowType getType();

    void setDockWidgetsVisible(bool visible);
    void addDockWidget(BaseDockWidget *widget);

    // NOTE: These are non-virtual functions
    //  QMainWindow's implementations are called within the functions
    void addDockWidget(Qt::DockWidgetArea area, QDockWidget* widget);
    virtual void addDockWidget(Qt::DockWidgetArea area, QDockWidget* widget, Qt::Orientation orientation);
    void removeDockWidget(QDockWidget* widget);

    void setDockWidgetVisibility(BaseDockWidget* widget, bool visible);
    QMenu* createPopupMenu() override;

    void resetDockWidgets();

protected:
    explicit BaseWindow(QWidget* parent = nullptr, WindowType type = VIEW_WINDOW);
    ~BaseWindow() override;

    void closeEvent(QCloseEvent *) override;
    bool focusNextPrevChild(bool next) override;

signals:
    void dockWidgetAdded(BaseDockWidget* widget);
    void dockWidgetVisibilityChanged();

public slots:
    void tryClose();

private slots:
    void themeChanged();

    void showContextMenu(const QPoint &point);
    void setDockWidgetMaximized(int id, bool maximized);
    void _setDockWidgetVisibility(int ID, bool visible);

private:
    void removeAllDockWidgets();
    void updateActions();

    QList<int> previouslyVisibleDockIDs;
    WindowType windowType;

    QHash<int, BaseDockWidget*> currentDockWidgets;
    QList<BaseDockWidget*> ownedDockWidgets;

    int ID;
    static int _WindowID;

    bool terminating = false;
    QAction* reset_action = nullptr;
};

#endif // BASEWINDOW_H