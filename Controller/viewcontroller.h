#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "entityadapter.h"
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

    QStringList getSearchSuggestions();


    QStringList getAdoptableNodeKinds();
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
    void projectSaved(QString filePath);
    void projectPathChanged(QString);
    void projectModified(bool);
    void initializeModel();
    void modelReady(bool);
    void controllerReady(bool);
    void viewItemConstructed(ViewItem* viewItem);
    void viewItemDestructing(int ID, ViewItem *viewItem);
    void triggerAction(QString action);
    void setData(int, QString, QVariant);
    void showToolbar(QPoint globalPos, QPointF itemPos = QPointF());


    void undo();
    void redo();

    void teardown();






    void undoRedoChanged();


    void deleteEntities(QList<int> IDs);
    void cutEntities(QList<int> IDs);
    void copyEntities(QList<int> IDs);
    void pasteIntoEntity(QList<int> IDs, QString data);
    void replicateEntities(QList<int> IDs);

    void constructNode(int parentID, QString kind, QPointF pos = QPointF());
    void importProjects(QStringList fileData);
    void vc_openProject(QString fileName, QString filePath);

    void seachSuggestions(QStringList suggestions);

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
    void closeProject();
    void closeMEDEA();
    void saveProject();
    void saveAsProject();
    void _importProjects();


    void fitView();
    void centerSelection();

    void cut();
    void copy();
    void paste();
    void replicate();

    void deleteSelection();
    void constructDDSQOSProfile();

    void setModelReady(bool okay);

    void setControllerReady(bool ready);

    QList<ViewItem*> search(QString searchString);
    void searchSuggestionsRequested(QString searchString="");

private slots:
    void initializeController();
    void table_dataChanged(int ID, QString key, QVariant data);

private:
    bool destructViewItem(ViewItem* item);
    QList<ViewItem*> getViewItems(QList<int> IDs);

    NodeViewNew* getActiveNodeView();
    void _teardownProject();


    bool _newProject();
    bool _saveProject();
    bool _saveAsProject();
    bool _closeProject();
    bool _openProject(QString filePath = "");
    QList<int> getIDsOfKind(QString kind);
    bool _modelReady;


    bool _controllerReady;


    bool destructChildItems(ViewItem* parent);
    bool clearVisualItems();


    ViewItem* getViewItem(int ID);

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
