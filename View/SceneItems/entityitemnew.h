#ifndef ENTITYITEMNEW_H
#define ENTITYITEMNEW_H

#include "../viewitem.h"
#include <QGraphicsObject>
#include <QGraphicsSceneHoverEvent>
#include <QPen>


#define SELECTED_LINE_WIDTH 3
class EntityItemNew: public QGraphicsObject
{
    Q_OBJECT

public:
    enum KIND{EDGE, NODE};
    enum ELEMENT_RECT{ER_MAIN_LABEL, ER_SECONDARY_LABEL, ER_SECONDARY_TEXT, ER_MAIN_ICON, ER_MAIN_ICON_OVERLAY, ER_SECONDARY_ICON, ER_EXPANDED_STATE, ER_LOCKED_STATE, ER_STATUS, ER_CONNECT_IN, ER_CONNECT_OUT, ER_INFORMATION, ER_NOTIFICATION};

    enum RENDER_STATE{RS_NONE, RS_BLOCK, RS_MINIMAL, RS_REDUCED, RS_FULL};

    EntityItemNew(ViewItem *viewItem, EntityItemNew* parentItem, KIND kind);
    ~EntityItemNew();

    RENDER_STATE getRenderState(qreal lod) const;
    VIEW_STATE getViewState() const;

    EntityItemNew* getParent() const;

    int getID();

    virtual QRectF getElementRect(ELEMENT_RECT rect) = 0;
    void paintPixmap(QPainter *painter, qreal lod, ELEMENT_RECT pos, QString alias, QString imageName, bool update=false, QColor tintColor=QColor());


    QRectF translatedBoundingRect() const;
    virtual QRectF sceneBoundingRect() const;
    virtual QRectF boundingRect() const = 0;

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
    void setDefaultPen(QPen pen);

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
    void destruct();

public:

    void setHovered(bool isHovered);
    void setHighlighted(bool isHighlight);
    void setSelected(bool selected);
    void setActiveSelected(bool active);
    /*
protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
*/
private:
    void connectViewItem(ViewItem* viewItem);
    void disconnectViewItem();

    EntityItemNew* parentItem;
    ViewItem* viewItem;
    QStringList requiredDataKeys;

    QHash<ELEMENT_RECT, QPixmap> imageMap;

    bool selectEnabled;
    bool hoverEnabled;
    bool _isHovered;
    bool _isHightlighted;
    bool _isSelected;
    bool _isActiveSelected;

    QPen defaultPen;

    KIND kind;


};

#endif // GRAPHMLITEM_H
