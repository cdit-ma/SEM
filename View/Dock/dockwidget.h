#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>

#include "../../Controller/toolbarcontroller.h"
#include "dockwidgetparentactionitem.h"
#include "dockwidgetactionitem.h"
#include "dockwidgetitem.h"

class DockWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit DockWidget(ToolActionController* tc, ToolActionController::DOCK_TYPE type, QWidget *parent = 0);

    ToolActionController::DOCK_TYPE getDockType();

    void addItem(DockWidgetItem* item);
    void addItems(QList<DockWidgetItem*> items);
    void addItems(QList<QAction*> actions);

    void clearDock();
    bool isEmpty();
    
    void updateHeaderText(QString text);
    void updateInfoLabel(QString text);
    void displayInfoLabel(bool display);

signals:
    void actionClicked(DockWidgetActionItem* action);
    void backButtonClicked();

public slots:
    void themeChanged();
    void dockActionClicked();

    void viewItemConstructed(int ID);
    void viewItemDestructed(int ID);

private:
    void setupHeaderLayout();

    ToolActionController* toolActionController;
    ToolActionController::DOCK_TYPE dockType;

    QVBoxLayout* alignLayout;
    QVBoxLayout* mainLayout;    
    QVBoxLayout* headerLayout;

    QWidget* mainWidget;
    DockWidgetItem* kindLabel;
    QToolButton* backButton;
    QLabel* infoLabel;
    bool containsHeader;

    // these lists store the widgets in the main layout
    QList<DockWidgetItem*> childrenItems;
    QHash<int, DockWidgetItem*> childrenIDHash;

    QList<QToolBar*> itemToolbars;
    QHash<DockWidgetItem*, QToolBar*> itemToolbarHash;

};

#endif // DOCKWIDGET_H
