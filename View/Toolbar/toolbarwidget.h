#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "../nodeview.h"
#include "../GraphicsItems/nodeitem.h"
#include "../GraphicsItems/edgeitem.h"
#include "toolbarmenuaction.h"

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
    explicit ToolbarWidget(NodeView *parent = 0);

    void updateToolbar(QList<NodeItem*> nodeItems, QList<EdgeItem*> edgeItems);

public slots:
    void updateActionEnabledState(QString actionName, bool enabled);

    void addChildNode(ToolbarMenuAction *action);
    void addConnectedNode(ToolbarMenuAction* action);
    void connectNodes(ToolbarMenuAction* action);
    void displayConnectedNode(ToolbarMenuAction* action);
    void constructNewView();

    void setupLegalNodesList();
    void setupComponentList();
    void setupBlackBoxList();
    void setupEventPortInstanceList();

    void updateDisplayedChildren();
    void hardwareClusterMenuClicked(int viewMode);

    void connectMenuOpened();
    void setInstanceID();

    void setVisible(bool visible);
    void hide();
    void hideToolbar(bool actionTriggered = false);

private:
    void setupToolBar();
    void setupMenus();
    void makeConnections();

    void updateButtonsAndMenus(QList<NodeItem*> nodeItems);
    void updateSeparators();

    void hideButtons();
    void hideSeparators();

    void clearMenus();
    void resetButtonGroupFlags();

    void setupAdoptableNodesList(QStringList nodeKinds);
    void setupLegalNodesList(QList<NodeItem*> nodeList);
    void setupInstancesList(QList<NodeItem*> instances);

    void setupComponentList(QString actionKind);
    void setupEventPortInstanceList(QString eventPortKind);

    QToolButton* constructToolButton(QSize size, QString iconPng, double iconSizeRatio, QString tooltip = "");
    QFrame* constructFrameSeparator();

    ToolbarMenu* constructToolButtonMenu(QToolButton* parentButton);
    ToolbarMenuAction* constructSubMenuAction(NodeItem* nodeItem, ToolbarMenu* parentMenu);

    NodeView* nodeView;
    NodeItem* nodeItem;

    QHBoxLayout* toolbarLayout;

    QFrame* mainFrame;
    QFrame* shadowFrame;
    QFrame* alignFrame;
    QFrame* snippetFrame;
    QFrame* goToFrame;
    QFrame* alterViewFrame;

    QToolButton* addChildButton;
    QToolButton* deleteButton;
    QToolButton* connectButton;

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

    ToolbarMenu* addMenu;
    ToolbarMenu* connectMenu;
    ToolbarMenu* definitionMenu;
    ToolbarMenu* implementationMenu;

    ToolbarMenu* instancesMenu;
    ToolbarMenu* instanceOptionMenu;
    ToolbarMenu* displayedChildrenOptionMenu;

    ToolbarMenu* componentImplDefinitionsMenu;
    ToolbarMenu* componentInstDefinitionsMenu;
    ToolbarMenu* blackBoxDefinitionsMenu;
    ToolbarMenu* iep_definitionInstanceMenu;
    ToolbarMenu* oep_definitionInstanceMenu;

    ToolbarMenuAction* componentImplAction;
    ToolbarMenuAction* componentInstAction;
    ToolbarMenuAction* blackBoxInstanceAction;
    ToolbarMenuAction* inEventPortDelegateAction;
    ToolbarMenuAction* outEventPortDelegateAction;

    ToolbarMenuAction* componentImplMenuInfoAction;
    ToolbarMenuAction* componentInstMenuInfoAction;
    ToolbarMenuAction* blackBoxMenuInfoAction;
    ToolbarMenuAction* iep_menuInfoAction;
    ToolbarMenuAction* oep_menuInfoAction;

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
    bool snippetButtonsVisible;
    bool goToButtonsVisible;
    bool alterViewButtonsVisible;

    bool blackBoxMenuDone;
    bool componentImplMenuDone;
    bool componentInstMenuDone;
    bool inEventPortInstanceMenuDone;
    bool outEventPortInstanceMenuDone;
    bool connectMenuDone;

    QList<NodeItem*> legalNodeItems;
    QString chosenInstanceID;

};

#endif // TOOLBARWIDGET_H
