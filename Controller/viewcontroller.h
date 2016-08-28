#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "../View/viewitem.h"
#include "selectionhandler.h"
#include "../Widgets/New/selectioncontroller.h"
#include "actioncontroller.h"
#include "toolbarcontroller.h"

class NewController;
class ToolbarWidgetNew;
class NodeViewNew;
class ViewController : public QObject
{
    Q_OBJECT
public:
    ViewController();
    ~ViewController();

    QStringList getNodeKinds();
    SelectionController* getSelectionController();
    ActionController* getActionController();
    ToolActionController* getToolbarController();

    QList<ViewItem*> getConstructableNodeDefinitions(QString kind);
    QList<ViewItem*> getValidEdges(Edge::EDGE_CLASS kind);

    QStringList _getSearchSuggestions();


    QStringList getAdoptableNodeKinds();
    QList<Edge::EDGE_CLASS> getValidEdgeKindsForSelection();
    QStringList getValidValuesForKey(int ID, QString keyName);
    void setDefaultIcon(ViewItem* viewItem);
    ViewItem* getModel();
    bool isModelReady();
    bool isControllerReady();

    bool canUndo();
    bool canRedo();

    QVector<ViewItem*> getOrderedSelection(QList<int> selection);

    void setController(NewController* c);
signals:
    //TO OTHER VIEWS SIGNALS

    void vc_controllerReady(bool);
    void vc_viewItemConstructed(ViewItem* viewItem);
    void vc_viewItemDestructing(int ID, ViewItem *viewItem);
    void vc_showToolbar(QPoint globalPos, QPointF itemPos = QPointF());
    void vc_gotSearchSuggestions(QStringList suggestions);

    void vc_editTableCell(int ID, QString keyName);

    void mc_showProgress(bool, QString);
    void mc_progressChanged(int);




    void mc_modelReady(bool);
    void mc_projectModified(bool);
    void mc_undoRedoUpdated();


    //TO CONTROLLER SIGNALS

    void vc_setupModel();
    void vc_undo();
    void vc_redo();
    void vc_triggerAction(QString);
    void vc_setData(int, QString, QVariant);
    void vc_removeData(int, QString);
    void vc_deleteEntities(QList<int> IDs);
    void vc_cutEntities(QList<int> IDs);
    void vc_copyEntities(QList<int> IDs);
    void vc_paste(QList<int> IDs, QString data);
    void vc_replicateEntities(QList<int> IDs);

    void vc_constructNode(int parentID, QString kind, QPointF pos = QPointF());
    void vc_constructEdge(QList<int> sourceIDs, int dstID, Edge::EDGE_CLASS edgeKind = Edge::EC_UNDEFINED);

    void vc_constructConnectedNode(int parentID, QString nodeKind, int dstID, Edge::EDGE_CLASS edgeKind = Edge::EC_UNDEFINED, QPointF pos=QPointF());


    void vc_importProjects(QStringList fileData);
    void vc_openProject(QString fileName, QString filePath);

    void vc_projectSaved(QString filePath);
    void vc_projectPathChanged(QString);


public slots:
    void actionFinished(bool success, QString gg);
    void controller_entityConstructed(int ID, ENTITY_KIND eKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties);
    void controller_entityDestructed(int ID, ENTITY_KIND eKind, QString kind);
    void controller_dataChanged(int ID, QString key, QVariant data);
    void controller_dataRemoved(int ID, QString key);

    void controller_propertyChanged(int ID, QString property, QVariant data);
    void controller_propertyRemoved(int ID, QString property);

    void setClipboardData(QString data);

    void newProject();
    void openProject();
    void importProjects();
    void saveProject();
    void saveAsProject();
    void closeProject();
    void closeMEDEA();


    void fitView();
    void centerSelection();

    void cut();
    void copy();
    void paste();
    void replicate();
    void deleteSelection();
    void renameActiveSelection();

    void constructDDSQOSProfile();
    void requestSearchSuggestions();


    void setModelReady(bool okay);
    void setControllerReady(bool ready);


    QList<ViewItem*> search(QString searchString);
private slots:
    void initializeController();
    void table_dataChanged(int ID, QString key, QVariant data);

private:
    bool destructViewItem(ViewItem* item);
    QList<ViewItem*> getViewItems(QList<int> IDs);

    NodeViewItem* getNodeViewItem(int ID);

    NodeViewItem* getSharedParent(NodeViewItem* node1, NodeViewItem* node2);

    NodeViewNew* getActiveNodeView();
    void _teardownProject();


    bool _newProject();
    bool _saveProject();
    bool _saveAsProject();
    bool _closeProject();
    void _importProjects();
    bool _openProject(QString filePath = "");
    QList<int> getIDsOfKind(QString kind);
    bool _modelReady;


    bool _controllerReady;


    bool destructChildItems(ViewItem* parent);
    bool clearVisualItems();


    ViewItem* getViewItem(int ID);

    QHash<QString, int> treeLookup;
    QHash<QString, QList<int> > itemKindLists;
    QHash<int, ViewItem*> viewItems;
    QList<int> topLevelItems;
    ViewItem* modelItem;
    ViewItem* rootItem;

    SelectionController* selectionController;
    ActionController* actionController;
    ToolActionController* toolbarController;

    ToolbarWidgetNew* toolbar;
    NewController* controller;

    bool showSearchSuggestions;
};

#endif // VIEWCONTROLLER_H
