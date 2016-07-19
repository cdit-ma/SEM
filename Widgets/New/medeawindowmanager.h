#ifndef MEDEAWINDOWMANAGER_H
#define MEDEAWINDOWMANAGER_H

#include <QObject>
#include <QAction>
#include <QMenu>

class DockTitleBarWidget;
class MedeaWindowNew;
class MedeaWindowManager:public QObject
{
    Q_OBJECT
public:
    MedeaWindowManager();

    void setMainWindow(MedeaWindowNew* window);
    MedeaWindowNew* getNewSubWindow();

    QMenu* getMenu(MedeaWindowNew* window=0);


private slots:
    void changeWidgetWindow();
    void widgetButtonPressed();
private:
    void addWindow(MedeaWindowNew* window);
    void removeWindow(MedeaWindowNew* window);




    QMenu* menu;
    MedeaWindowNew* mainWindow;
    QHash<MedeaWindowNew*, int> windows;

    DockTitleBarWidget* dock;
    int windowCount;

public:
    static MedeaWindowManager* manager();

private:
    static MedeaWindowManager* managerSingleton;
};

#endif // MEDEAWINDOWMANAGER_H
