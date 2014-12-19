#ifndef GRAPHMLVIEW_H
#define GRAPHMLVIEW_H

#include <QHash>
#include <QObject>
#include "../Controller/newcontroller.h"
#include "graphmlviewitem.h"

class GraphMLView: public QObject
{
    Q_OBJECT
public:
    GraphMLView();
    virtual void connectController(NewController* controller)=0;
    virtual void disconnectController()=0;
    NewController* getController();

signals:
    virtual void view_Cut();
    virtual void view_Copy();
    virtual void view_Paste();

    virtual void view_Undo();
    virtual void view_Redo();

    virtual void view_SelectAll();
    virtual void view_ClearSelection();
    virtual void view_SetViewAspects(QStringList aspects);

    virtual void view_SortModel();
    virtual void view_SetSelectedTableModel(AttributeTableModel* model);
    virtual void view_TriggerSelected(GraphML*, bool setSelected = true);
public slots:
    virtual void view_RefreshView() = 0;
    virtual void view_SetViewDrawDepth(int depth) = 0;
    virtual void view_PrintErrorText(GraphML* item, QString text) = 0;

    virtual void view_ConstructItem(GraphML* item) = 0;
    virtual void view_DestructItem(QString ID) = 0;

    virtual void view_FocusItem(GraphML* item) = 0;
    virtual void view_SelectItem(GraphML* item, bool selected=true) = 0;
    virtual void view_SetItemOpacity(GraphML* item, qreal opacity =0) =0;

private:
    QHash<QString, GraphMLViewItem*> guiItems;
    NewController* attachedController;
    bool controllerConnected;
};

#endif // GRAPHMLVIEW_H
