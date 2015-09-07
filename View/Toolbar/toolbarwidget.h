#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "../nodeview.h"
#include "../GraphicsItems/nodeitem.h"
#include "../GraphicsItems/edgeitem.h"
#include "toolbarwidgetaction.h"

#include <QWidget>
#include <QToolButton>
#include <QMenu>
#include <QFrame>


class ToolbarWidgetAction;
class ToolbarWidgetMenu;

class ToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ToolbarWidget(NodeView *parent = 0);

    void updateSelectedItems(QList<NodeItem*> nodeItems, QList<EdgeItem*> edgeItems);
    void updateSeparators(bool snippet = true, bool goTo = true);

    void showDefinitionButton(QString definitionID);
    void showImplementationButton(QString implementationID);
    void showSnippetButton(QString button, bool show);

protected:
    void enterEvent(QEvent* event);
    void leaveEvent(QEvent* event);

public slots:
    void updateActionEnabled(QString actionName, bool enabled);

    void displayAllChildren();
    void displayConnectedChildren();
    void displayUnconnectedChildren();
    void hardwareClusterMenuClicked(int viewMode);

    void goToDefinition();
    void goToImplementation();
    void goToInstance();

    void addChildNode();
    void connectNodes();
    void addConnectedNode();
    void makeNewView();

    void attachOptionMenu();

    void setVisible(bool visible);
    void hideToolbar(bool actionTriggered);
    void hide();

private:
    void setupToolBar();
    void setupMenus();
    void makeConnections();

    void updateToolButtons();
    void updateMenuLists();
    void updateRadioButtons(QList<NodeItem*> hardwareClusters);

    void multipleSelection(QList<NodeItem*> items, QList<EdgeItem*> edgeItems = QList<EdgeItem*>());

    void setupAdoptableNodesList(QStringList nodeKinds);
    void setupLegalNodesList(QList<NodeItem*> nodeList);
    void setupInstancesList(QList<NodeItem*> instances);

    void setupFilesList(QList<NodeItem*> files, QString kind);
    void setupComponentList(QList<NodeItem*> components, QString kind);
    void setupBlackBoxList(QList<NodeItem*> blackBoxes);
    void setupBlackBoxInstanceList(QList<NodeItem*> blackBoxInstances);

    void setupChildrenComponentInstanceList(QList<NodeItem *> componentInstances);
    void setupInEventPortInstanceList();
    void setupOutEventPortInstanceList();

    void clearMenus();

    NodeView* nodeView;
    NodeItem* currentSelectedItem;

    NodeItem* nodeItem;
    NodeItem* prevNodeItem;

    QString definitionNodeID;
    QString implementationNodeID;

    QToolButton* addChildButton;
    QToolButton* deleteButton;
    QToolButton* connectButton;

    QToolButton* showNewViewButton;
    QToolButton* showConnectionsButton;

    QToolButton* definitionButton;
    QToolButton* implementationButton;
    QToolButton* instancesButton;

    QToolButton* alignVerticallyButton;
    QToolButton* alignHorizontallyButton;

    QToolButton* exportSnippetButton;
    QToolButton* importSnippetButton;

    QToolButton* displayedChildrenOptionButton;

    ToolbarWidgetAction* componentImplAction;
    ToolbarWidgetAction* componentInstanceAction;
    ToolbarWidgetAction* inEventPortDelegateAction;
    ToolbarWidgetAction* outEventPortDelegateAction;
    ToolbarWidgetAction* blackBoxInstanceAction;

    ToolbarWidgetMenu* addMenu;
    ToolbarWidgetMenu* connectMenu;
    ToolbarWidgetMenu* definitionMenu;
    ToolbarWidgetMenu* implementationMenu;

    ToolbarWidgetMenu* instancesMenu;
    ToolbarWidgetMenu* instanceOptionMenu;
    ToolbarWidgetMenu* displayedChildrenOptionMenu;

    ToolbarWidgetMenu* fileMenu;
    ToolbarWidgetMenu* inEventPort_componentInstanceMenu;
    ToolbarWidgetMenu* outEventPort_componentInstanceMenu;
    ToolbarWidgetMenu* blackBoxInstanceMenu;

    ToolbarWidgetAction* fileDefaultAction;
    ToolbarWidgetAction* inEventPort_componentInstanceDefaultAction;
    ToolbarWidgetAction* outEventPort_componentInstanceDefaultAction;
    ToolbarWidgetAction* blackBoxInstanceDefaultAction;

    QRadioButton* allNodes;
    QRadioButton* connectedNodes;
    QRadioButton* unconnectedNodes;

    QList<QToolButton*> singleSelectionToolButtons;
    QList<QToolButton*> multipleSelectionToolButtons;
    QList<QToolButton*> alterModelToolButtons;

    QFrame* mainFrame;
    QFrame* shadowFrame;
    QFrame* snippetFrame;
    QFrame* alterViewFrame;
    QFrame* goToFrame;

    bool showToolbar;
    bool showAlterViewFrame;
    bool showSnippetFrame;
    bool showGoToFrame;

    bool eventFromToolbar;
    bool deleteButtonVisible;
};

#endif // TOOLBARWIDGET_H
