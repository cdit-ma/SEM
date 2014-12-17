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
    void view_Cut();
    void view_Copy();
    void view_Paste();

    void view_Undo();
    void view_Redo();

    void view_SelectAll();
    void view_ClearSelection();
    void view_SetViewAspects(QStringList aspects);

    void view_SortModel();
    void view_SetSelectedTableModel(AttributeTableModel* model);
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
