#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "../nodeview.h"
#include "../GraphicsItems/nodeitem.h"
#include "../GraphicsItems/edgeitem.h"
#include "toolbarwidgetaction.h"
#include "toolbarabstractbutton.h"

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

    void updateToolbar(QList<NodeItem*> nodeItems, QList<EdgeItem*> edgeItems);

signals:
    void toolbar_menuOnFocus(ToolbarWidgetMenu* menu);

public slots:
    void updateActionEnabledState(QString actionName, bool enabled);

    void updateDisplayedChildren();
    void hardwareClusterMenuClicked(int viewMode);

    void addChildNode();
    void addConnectedNode();
    void connectNodes();
    void goToConnectedNode();
    void constructNewView();

    void attachOptionMenu();
    void menuOnFocus(ToolbarWidgetMenu* menu);

    void setVisible(bool visible);
    void hide();
    void hideToolbar(bool actionTriggered);

    void setupComponentList();
    void setupBlackBoxList();
    void setupEventPortInstanceList();

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

    QToolButton* constructToolButton(QSize size, QString iconPng, double iconSizeRatio, QString tooltip);
    QFrame* constructFrameSeparator();

    ToolbarWidgetMenu* constructToolMenu(QToolButton *parentButton);
    ToolbarWidgetAction* constructSubMenuAction(NodeItem* nodeItem, ToolbarWidgetMenu* parentMenu);

    void setupAdoptableNodesList(QStringList nodeKinds);
    void setupLegalNodesList(QList<NodeItem*> nodeList);
    void setupInstancesList(QList<NodeItem*> instances);

    void setupComponentList(QString actionKind);
    void setupEventPortInstanceList(QString eventPortKind);

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

    ToolbarWidgetMenu* addMenu;
    ToolbarWidgetMenu* connectMenu;
    ToolbarWidgetMenu* definitionMenu;
    ToolbarWidgetMenu* implementationMenu;

    ToolbarWidgetMenu* instancesMenu;
    ToolbarWidgetMenu* instanceOptionMenu;
    ToolbarWidgetMenu* displayedChildrenOptionMenu;

    ToolbarWidgetMenu* componentDefinitionsMenu;
    ToolbarWidgetMenu* blackBoxDefinitionsMenu;
    ToolbarWidgetMenu* iep_definitionInstanceMenu;
    ToolbarWidgetMenu* oep_definitionInstanceMenu;

    ToolbarWidgetAction* componentImplAction;
    ToolbarWidgetAction* componentInstanceAction;
    ToolbarWidgetAction* blackBoxInstanceAction;
    ToolbarWidgetAction* inEventPortDelegateAction;
    ToolbarWidgetAction* outEventPortDelegateAction;

    ToolbarWidgetAction* componenMenuDefaultAction;
    ToolbarWidgetAction* blackBoxMenuDefaultAction;
    ToolbarWidgetAction* iep_menuDefaultAction;
    ToolbarWidgetAction* oep_menuDefaultAction;

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

};

#endif // TOOLBARWIDGET_H
