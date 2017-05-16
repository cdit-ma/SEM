#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QToolButton>

#include "../../Controllers/ToolbarController/toolbarcontroller.h"
#include "dockwidgetparentactionitem.h"
#include "dockwidgetactionitem.h"
#include "dockwidgetitem.h"

class DockWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DockWidget(ToolbarController* tc, ToolbarController::DOCK_TYPE type, QWidget *parent = 0);

    ToolbarController::DOCK_TYPE getDockType();

    void addItem(DockWidgetItem* item);
    void addItems(QList<DockWidgetItem*> items);
    void addItems(QList<QAction*> actions);

    bool isEmpty();
    void clearDock();
    
    void updateHeaderText(QString text);
    void updateInfoLabel(QString text);
    void displayInfoLabel(bool display);

signals:
    void actionClicked(DockWidgetActionItem* action);
    void backButtonClicked();

public slots:
    void themeChanged();
    void dockActionClicked();

    void highlightItem(int ID = -1);

    void viewItemConstructed(int ID);
    void viewItemGroupConstructed(int ID);
    void viewItemDestructed(int ID);

private:
    void setupLayout();
    void setupHeaderLayout();

    ToolbarController* toolActionController;
    ToolbarController::DOCK_TYPE dockType;

    QVBoxLayout* mainLayout;
    QVBoxLayout* itemsLayout;

    QScrollArea* scrollArea;
    QWidget* mainWidget;
    DockWidgetItem* kindLabel;
    QToolButton* backButton;
    QLabel* infoLabel;

    DockWidgetActionItem* prevHighlightedItem;
    int prevHighlightedItemID;

    // these lists store the widgets in the main layout
    QList<DockWidgetItem*> childrenItems;
    QHash<int, DockWidgetActionItem*> actionItemIDHash;
    QHash<int, DockWidgetParentActionItem*> parentActionItemIDHash;

    QList<QToolBar*> itemToolbars;
    QHash<DockWidgetItem*, QToolBar*> itemToolbarHash;

};

#endif // DOCKWIDGET_H
