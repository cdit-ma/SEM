#ifndef VIEW_CONTROLLER_INTERFACE_H
#define VIEW_CONTROLLER_INTERFACE_H

#include <QSet>
#include <QObject>
#include <QPointF>
#include <QVariant>
#include <QList>

#include "kinds.h"
#include "nodekinds.h"
#include "edgekinds.h"
#include "dataupdate.h"

class ModelController;
enum class MODEL_SEVERITY;

class ViewControllerInterface: public QObject
{
    Q_OBJECT
public:
    void ConnectModelController(ModelController* controller);
protected:
    virtual void NodeConstructed(int parent_id, int id, NODE_KIND node_kind) = 0;
    virtual void EdgeConstructed(int id, EDGE_KIND edge_kind, int src_id, int dst_id) = 0;
    virtual void EntityDestructed(int id, GRAPHML_KIND kind) = 0;
    virtual void DataChanged(int id, DataUpdate data) = 0;
    virtual void DataRemoved(int id, const QString& key_name) = 0;
    virtual void NodeEdgeKindsChanged(int id) = 0;
    virtual void NodeTypesChanged(int id) = 0;
    virtual void AddNotification(MODEL_SEVERITY severity, const QString& title, const QString& description, int id) = 0;
    virtual void ModelReady(bool ready) = 0;

signals:
    void SetupModelController(QString file_path = "");
    void ImportProjects(QList<QString> graphml_datalist);
    void UndoRedoUpdated();
    void ReloadWorkerDefinitions();

    //Model Notifier Signals
    void ProjectFileChanged(QString);
    void ShowProgress(bool, QString);
    void ProgressUpdated(int);
    void ProjectModified(bool);
    void ActionFinished();

    //Model Actions
    void TriggerAction(QString action_name);
    void SetData(int id, QString key_name, QVariant value);
    void RemoveData(int id, QString key_name);
    void Undo();
    void Redo();
    void Delete(QList<int> entity_ids);
    void Paste(QList<int> entity_ids, QString paste_data);
    void Replicate(QList<int> entity_ids);

    void constructDDSQOSProfile();
    void ConstructNodeAtIndex(int parent_id, NODE_KIND node_kind, int index);
    void ConstructNodeAtPos(int parent_id, NODE_KIND node_kind, QPointF position);
    
    void ConstructConnectedNodeAtIndex(int parent_id, NODE_KIND node_kind, int dst_id, EDGE_KIND edge_kind, int index);
    void ConstructConnectedNodeAtPos(int parent_id, NODE_KIND node_kind, int dst_id, EDGE_KIND edge_kind, QPointF position);

    void ConstructEdges(QList<int> source_ids, QList<int> dst_ids, EDGE_KIND edge_kind);
    void DestructEdges(QList<int> source_ids, QList<int> dst_ids, EDGE_KIND edge_kind);

    void DestructAllEdges(QList<int> source_ids, EDGE_KIND edge_kind, QSet<EDGE_DIRECTION> edge_directions);
    
};

#endif // VIEW_CONTROLLER_INTERFACE_H