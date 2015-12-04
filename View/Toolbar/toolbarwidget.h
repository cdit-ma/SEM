#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "../nodeview.h"
#include "../GraphicsItems/nodeitem.h"
#include "../GraphicsItems/edgeitem.h"
#include "toolbarmenuaction.h"
#include <QRadioButton>
#include <QWidget>
#include <QToolButton>
#include <QMenu>
#include <QFrame>

class ToolbarMenu;
class ToolbarMenuAction;

class ToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolbarWidget(NodeView* parentView = 0);

    void updateToolbar(QList<NodeItem*> nodeItems, QList<EdgeItem*> edgeItems);
    void setupFunctionsList();

public slots:
    void updateActionEnabledState(QString actionName, bool enabled);

    void addChildNode(ToolbarMenuAction *action);
    void addConnectedNode(ToolbarMenuAction* action);
    void connectNodes(ToolbarMenuAction* action);
    void displayConnectedNode(ToolbarMenuAction* action = 0);
    void expandContractNodes();
    void constructNewView();

    void setupAdoptableNodesList();
    void setupLegalNodesList();
    void setupComponentList();
    void setupBlackBoxList();
    void setupEventPortInstanceList();
    void setupAggregateList();
    void setupVectorList();
    void setupHardwareList();

    void setInstanceID();

    void menuActionHovered(QAction* action = 0);

    void updateDisplayedChildren();
    void hardwareClusterMenuClicked(int viewMode = -1);

    void hideToolbar(QAction* action = 0);
    void hide();
    void setVisible(bool visible);

    void setupTheme(VIEW_THEME theme = VT_NORMAL_THEME);

    // these slots and their corresponding list are only needed for Mac
    void appendToOpenMenusList();
    void removeFromOpenMenusList();

private:
    void setupToolBar();
    void setupMenus();
    void makeConnections();

    void updateButtonsAndMenus(QList<NodeItem *> nodeItems);
    void updateSeparators();

    void hideButtons();
    void hideSeparators();

    void clearMenus();
    void resetButtonGroupFlags();

    void setupAdoptableNodesList(QStringList nodeKinds);
    void setupLegalNodesList(QList<NodeItem*> nodeList);
    void setupInstancesList(QList<NodeItem*> instances);
    void setupHardwareList(QList<EntityItem*> hardware);

    void setupComponentList(QString actionKind);
    void setupEventPortInstanceList(QString eventPortKind);

    QToolButton* constructToolButton(QSize size, double iconSizeRatio, QString iconPng, QString tooltip = "", QString iconPath = "Actions");
    QFrame* constructFrameSeparator();

    ToolbarMenu* constructToolButtonMenu(QToolButton* parentButton, bool instantPopup = true);
    ToolbarMenuAction* constructMenuAction(NodeItem* nodeItem, ToolbarMenu* parentMenu);
    ToolbarMenuAction* constructSubMenuAction(NodeItem* nodeItem, ToolbarMenu* parentMenu);

    void closeOpenMenus();  // Mac
    QList<ToolbarMenu*> openMenus;  // Mac

    NodeView* nodeView;
    NodeItem* nodeItem;

    QHBoxLayout* toolbarLayout;

    QFrame* mainFrame;
    QFrame* shadowFrame;
    QFrame* alignFrame;
    QFrame* expandContractFrame;
    QFrame* snippetFrame;
    QFrame* goToFrame;
    QFrame* alterViewFrame;

    QToolButton* addChildButton;
    QToolButton* deleteButton;
    QToolButton* connectButton;
    QToolButton* hardwareButton;

    QToolButton* definitionButton;
    QToolButton* implementationButton;
    QToolButton* instancesButton;

    QToolButton* alignVerticallyButton;
    QToolButton* alignHorizontallyButton;

    QToolButton* exportSnippetButton;
    QToolButton* importSnippetButton;

    QToolButton* connectionsButton;
    QToolButton* popupNewWindow;
    QToolButton* displayedChildrenOptionButton;

    QToolButton* expandButton;
    QToolButton* contractButton;

    ToolbarMenu* addMenu;
    ToolbarMenu* connectMenu;
    ToolbarMenu* definitionMenu;
    ToolbarMenu* implementationMenu;
    ToolbarMenu* instancesMenu;

    ToolbarMenu* hardwareMenu;
    ToolbarMenu* hardwareClusterViewMenu;

    ToolbarMenu* componentImplMenu;
    ToolbarMenu* componentInstMenu;
    ToolbarMenu* blackBoxInstMenu;
    ToolbarMenu* inEventPortDelegateMenu;
    ToolbarMenu* outEventPortDelegateMenu;
    ToolbarMenu* aggregateInstMenu;
    ToolbarMenu* vectorInstMenu;
    ToolbarMenu* functionsMenu;

    ToolbarMenuAction* componentImplAction;
    ToolbarMenuAction* componentInstAction;
    ToolbarMenuAction* blackBoxInstAction;
    ToolbarMenuAction* inEventPortDelegateAction;
    ToolbarMenuAction* outEventPortDelegateAction;
    ToolbarMenuAction* aggregateInstAction;
    ToolbarMenuAction* vectorInstAction;
    ToolbarMenuAction* processAction;

    ToolbarMenuAction* componentImplMenuInfoAction;
    ToolbarMenuAction* componentInstMenuInfoAction;
    ToolbarMenuAction* blackBoxMenuInfoAction;
    ToolbarMenuAction* inEventPortDelegateMenuInfoAction;
    ToolbarMenuAction* outEventPortDelegateMenuInfoAction;
    ToolbarMenuAction* aggregateInstMenuInfoAction;
    ToolbarMenuAction* vectorInstMenuInfoAction;
    ToolbarMenuAction* functionsMenuInfoAction;

    QRadioButton* allNodes;
    QRadioButton* connectedNodes;
    QRadioButton* unconnectedNodes;

    bool showDeleteToolButton;
    bool showImportSnippetToolButton;
    bool showExportSnippetToolButton;
    bool showDefinitionToolButton;
    bool showImplementationToolButton;

    bool alterModelButtonsVisible;
    bool alignButtonsVisible;
    bool expandContractButtonsVisible;
    bool snippetButtonsVisible;
    bool goToButtonsVisible;
    bool alterViewButtonsVisible;

    bool blackBoxMenuDone;
    bool componentImplMenuDone;
    bool componentInstMenuDone;
    bool inEventPortInstanceMenuDone;
    bool outEventPortInstanceMenuDone;
    bool aggregateMenuDone;
    bool vectorMenuDone;
    bool addMenuDone;
    bool connectMenuDone;
    bool hardwareMenuDone;

    int chosenInstanceID;
    QStringList adoptableNodeKinds;
    QList<NodeItem*> legalNodeItems;
    QList<EntityItem*> hardwareNodeItems;

    VIEW_THEME currentTheme;
};

#endif // TOOLBARWIDGET_H
