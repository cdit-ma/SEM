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
#include <QToolBar>

class ToolbarMenu;
class ToolbarMenuAction;

class ToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolbarWidget(NodeView* parentView = 0);

    void updateToolbar(QList<NodeItem*> nodeItems, QList<EdgeItem*> edgeItems);
    void clearToolbarMenus();

    void setupFunctionsList();
    QStringList getNonDeletableMenuActionKinds();

public slots:
    void updateActionEnabledState(QString actionName, bool enabled);

    void addChildNode(ToolbarMenuAction *action);
    void addConnectedNode(ToolbarMenuAction* action);
    void connectNodes(ToolbarMenuAction* action);
    void displayConnectedNode(ToolbarMenuAction* action = 0);
    void destructEdge();
    void expandContractNodes();
    void constructNewView();
    void getCPPForComponent();
    void setReadOnlyMode();
    void launchWiki();

    void setupAdoptableNodesList();
    void setupLegalNodesList();
    void setupComponentList();
    void setupBlackBoxList();
    void setupAggregateList();
    void setupVectorList();
    void setupHardwareList();
    void setupOutEventPortList();

    void setInstanceID();

    void menuActionHovered(QAction* action = 0);

    void updateDisplayedChildren();
    void hardwareClusterMenuClicked(int viewMode = -1);

    void hideToolbar(QAction* action = 0);
    void hide();
    void setVisible(bool visible);

    void setupTheme();

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
    void resetGroupFlags();

    void setupAdoptableNodesList(QStringList nodeKinds);
    void setupLegalNodesList(QList<NodeItem*> nodeList);
    void setupInstancesList(QList<NodeItem*> instances);
    void setupHardwareList(QList<EntityItem*> hardware);
    void setupComponentList(QString actionKind);

    void updateToolButtonIcons();
    QToolButton* constructToolButton(QSize size, double iconSizeRatio, QString iconPng, QString tooltip = "", QString iconPath = "Actions");

    ToolbarMenu* constructTopMenu(QToolButton* parentButton, bool instantPopup = true, bool addToDynamicMenuHash = false);
    ToolbarMenu* constructSubMenu(ToolbarMenuAction* parentAction, QString infoText, bool addToDynamicMenuHash = true);

    ToolbarMenuAction* constructMenuAction(NodeItem* nodeItem, ToolbarMenu* parentMenu);
    ToolbarMenuAction* constructSubMenuAction(NodeItem* nodeItem, ToolbarMenu* parentMenu);

    void closeOpenMenus();  // Mac
    QList<ToolbarMenu*> openMenus;  // Mac

    NodeView* nodeView;
    NodeItem* nodeItem;

    QFrame* mainFrame;
    QFrame* shadowFrame;

    QAction*  actionAlignSeperator;
    QAction*  actionExpandContractSeperator;
    QAction*  actionSnippetSeperator;
    QAction*  actionGoToSeperator;
    QAction*  actionAlterViewSeperator;

    QToolBar* toolbar;
    QHBoxLayout* toolbarLayout;

    QToolButton* addChildButton;
    QToolButton* deleteButton;
    QToolButton* connectButton;
    QToolButton* hardwareButton;
    QToolButton* disconnectHardwareButton;
    QToolButton* definitionButton;
    QToolButton* implementationButton;
    QToolButton* instancesButton;
    QToolButton* alignVerticallyButton;
    QToolButton* alignHorizontallyButton;
    QToolButton* exportSnippetButton;
    QToolButton* importSnippetButton;
    QToolButton* getCPPButton;
    QToolButton* setReadOnlyButton;
    QToolButton* unsetReadOnlyButton;
    QToolButton* connectionsButton;
    QToolButton* popupNewWindow;
    QToolButton* displayedChildrenOptionButton;
    QToolButton* expandButton;
    QToolButton* contractButton;
    QToolButton* tagButton;
    QToolButton* wikiButton;

    QHash<QToolButton*, QAction*> actionLookup;

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
    ToolbarMenu* inEventPortMenu;
    ToolbarMenu* outEventPortMenu;
    ToolbarMenu* inEventPortDelegateMenu;
    ToolbarMenu* outEventPortDelegateMenu;
    ToolbarMenu* outEventPortImplMenu;
    ToolbarMenu* aggregateInstMenu;
    ToolbarMenu* vectorInstMenu;
    ToolbarMenu* functionsMenu;

    ToolbarMenuAction* componentImplAction;
    ToolbarMenuAction* componentInstAction;
    ToolbarMenuAction* blackBoxInstAction;
    ToolbarMenuAction* inEventPortAction;
    ToolbarMenuAction* outEventPortAction;
    ToolbarMenuAction* inEventPortDelegateAction;
    ToolbarMenuAction* outEventPortDelegateAction;
    ToolbarMenuAction* outEventPortImplAction;
    ToolbarMenuAction* aggregateInstAction;
    ToolbarMenuAction* vectorInstAction;
    ToolbarMenuAction* processAction;

    QRadioButton* allNodes;
    QRadioButton* connectedNodes;
    QRadioButton* unconnectedNodes;

    bool showDeleteToolButton;
    bool showImportSnippetToolButton;
    bool showExportSnippetToolButton;
    bool showDefinitionToolButton;
    bool showImplementationToolButton;

    bool showWikiButton;
    bool showShowCPPToolButton;
    bool showSetReadyOnlyToolButton;
    bool showUnsetReadyOnlyToolButton;

    bool showAlignmentButtons;

    bool alterModelButtonsVisible;
    bool alignButtonsVisible;
    bool expandContractButtonsVisible;
    bool snippetButtonsVisible;
    bool goToButtonsVisible;
    bool alterViewButtonsVisible;

    int chosenInstanceID;
    QList<int> deploymentEdgeIDs;

    QStringList adoptableNodeKinds;
    QList<NodeItem*> legalNodeItems;
    QList<EntityItem*> hardwareNodeItems;

    // this hash stores the menus that are cleared/re-populated when the toolbar
    // is shown and a bool of whether the menu has been re-populated or not
    QHash<ToolbarMenu*, bool> dynamicMenuPopulated;

    // this hash is used to check whether each separator should be visible or not
    QHash<QAction*, bool> buttonsGroupVisible;

protected:
    bool event(QEvent *);
};

#endif // TOOLBARWIDGET_H
