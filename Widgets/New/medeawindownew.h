#ifndef MEDEAWINDOWNEW_H
#define MEDEAWINDOWNEW_H

#include <QMainWindow>
#include "medeawindowmanager.h"
#include "medeadockwidget.h"
class MedeaWindowNew : public QMainWindow{

    Q_OBJECT
friend class MedeaWindowManager;

public:
    enum WindowType {MAIN_WINDOW, VIEW_WINDOW};
public:
    MedeaWindowNew(QWidget* parent = 0, WindowType type = VIEW_WINDOW);
    ~MedeaWindowNew();

public:
    bool hasDockWidgets();
    QList<MedeaDockWidget*> getDockWidgets();
    int getID();
    WindowType getType();

    void addDockWidget(MedeaDockWidget *widget);

    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget);
    virtual void addDockWidget(Qt::DockWidgetArea area, QDockWidget* widget, Qt::Orientation orientation);
    void removeDockWidget(QDockWidget* widget);


    QMenu *createPopupMenu();

signals:
    void dockWidgetAdded(MedeaDockWidget* widget);
private slots:
    void themeChanged();
    void showContextMenu(const QPoint &point);

    void setDockWidgetMaximized(int ID, bool maximized);
    void setDockWidgetVisibility(int ID, bool visible);
public slots:
    void tryClose();

private:
    void removeAllDockWidgets();
    void updateActions();

    QList<int> previouslyVisibleDockIDs;
    WindowType windowType;

    QHash<int, MedeaDockWidget*> currentDockWidgets;
    QList<MedeaDockWidget*> ownedDockWidgets;

protected:
    void closeEvent(QCloseEvent *);
private:
    int ID;
    bool terminating;
    static int _WindowID;

    // QWidget interface
protected:
    bool focusNextPrevChild(bool next);
};

#endif


