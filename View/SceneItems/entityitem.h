#ifndef ENTITYITEMNEW_H
#define ENTITYITEMNEW_H

#include "../../View/viewitem.h"
#include <QGraphicsObject>


class EntityItemNew: public QGraphicsObject
{
    Q_OBJECT

public:
    enum KIND{EDGE, NODE};
    enum RENDER_STATE{RS_NONE, RS_BLOCK, RS_MINIMAL, RS_REDUCED, RS_FULL};

    EntityItemNew(ViewItem *viewItem, EntityItem* parentItem, KIND kind);
    ~EntityItemNew();

    RENDER_STATE getRenderState(qreal lod) const;
    VIEW_STATE getViewState() const;

    EntityItemNew* getParent();

    virtual QRectF sceneBoundingRect() const;
    virtual QRectF boundingRect() const;

    bool intersectsRectInScene(QRectF rectInScene) const;

    void addRequiredData(QString keyName);
    void reloadRequiredData();

    void setData(QString keyName, QVariant value);
    QVariant getData(QString keyName);
    bool hasData(QString keyName);
    bool isDataEditable(QString keyName);

    void handleSelection(bool setSelected, bool controlPressed);
    void setSelectionEnabled(bool enabled);
    bool isSelected();
    bool isActiveSelected();
    bool isSelectionEnabled();

    void handleHover(bool hovered);
    void setHoverEnabled(bool enabled);
    bool isHovered();
    bool isHoverEnabled();
    bool isHighlighted();

    QPen getPen(qreal lod = -1);

    bool isNodeItem();
    bool isEdgeItem();

signals:
    //Request changes
    void req_setSelected(ViewItem*, bool);
    void req_setActiveSelected(ViewItem*, bool);
    void req_clearSelection();

    void req_setData(ViewItem* item, QString keyName, QVariant data);

    void req_triggerAction(QString actionName);

    void req_centerItem(EntityItemNew* entityItem);
    void req_hovered(EntityItemNew*, bool);

    //Inform of changes
    void sizeChanged();
    void positionChanged();
private slots:
    virtual void dataChanged(QString keyName, QVariant data) = 0;

    virtual void setHovered(bool isHovered);
    virtual void setHighlighted(bool isHighlight);
    virtual void setSelected(bool selected);
    virtual void setActiveSelected(bool active);
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

private:
    void connectViewItem(ViewItem* viewItem);
    void disconnectViewItem();

    EntityItem* parentItem;
    ViewItem* viewItem;
    QStringList requiredDataKeys;

    bool selectEnabled;
    bool hoverEnabled;
    bool _isHovered;
    bool _isHightlighted;
    bool _isSelected;
    bool _isActiveSelected;

    KIND kind;


};

#endif // GRAPHMLITEM_H
