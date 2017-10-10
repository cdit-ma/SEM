#ifndef ENTITYITEM_H
#define ENTITYITEM_H

#include "../../../Controllers/ViewController/viewitem.h"
#include "../../../Controllers/ViewController/edgeviewitem.h"
#include "../../../theme.h"
#include <QGraphicsObject>
#include <QGraphicsSceneHoverEvent>
#include <QPen>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include "statictextitem.h"

#define DEFAULT_SIZE 80

#define SELECTED_LINE_WIDTH 3

enum class RENDER_STATE{NONE, BLOCK, MINIMAL, REDUCED, FULL};
//Forward class definition
class NodeItem;

class EntityItem: public QGraphicsObject
{
    Q_OBJECT

public:
    static const int ENTITY_ITEM_KIND = UserType + 1;
    enum KIND{
        EDGE,
        NODE,
    };
    enum ELEMENT_RECT{ER_PRIMARY_TEXT, ER_SECONDARY_TEXT, ER_MAIN_ICON, ER_MAIN_ICON_OVERLAY, ER_SECONDARY_ICON, ER_EXPANDED_STATE, ER_LOCKED_STATE, ER_STATUS, ER_CONNECT, ER_CONNECT_ICON, ER_EDGE_KIND_ICON, ER_INFORMATION, ER_NOTIFICATION, ER_EXPANDCONTRACT, ER_SELECTION, ER_DEPLOYED, ER_QOS, ER_MOVE, ER_RESIZE_ARROW, ER_TERTIARY_ICON, ER_CONNECT_SOURCE, ER_CONNECT_TARGET};
    

    EntityItem(ViewItem *viewItem, EntityItem* parentItem, KIND kind);
    ~EntityItem();
    int type() const;

    bool isHidden() const;

    RENDER_STATE getRenderState(qreal lod) const;
    EntityItem* getParent() const;
    NodeItem* getParentNodeItem() const;

    void unsetParent();
    bool isTopLevelItem() const;
    bool isReadOnly() const;


    ViewItem* getViewItem() const;

    virtual QPointF getSceneEdgeTermination(bool left) const = 0;
    virtual QPointF getSceneEdgeTermination(EDGE_DIRECTION direction, EDGE_KIND kind) const = 0;


    QColor getBaseBodyColor() const;
    virtual QColor getBodyColor() const;
    void setBaseBodyColor(QColor color);

    virtual void setPos(const QPointF &pos);
    int getID();

    virtual QRectF getElementRect(ELEMENT_RECT rect) const;
    virtual QPainterPath getElementPath(ELEMENT_RECT rect) const;

    void paintPixmap(QPainter *painter, qreal lod, ELEMENT_RECT pos, QPair<QString, QString> image, QColor tintColor=QColor());
    void paintPixmap(QPainter *painter, qreal lod, ELEMENT_RECT pos, QString imagePath, QString imageName, QColor tintColor=QColor());
    void paintPixmap(QPainter *painter, qreal lod, QRectF pos, QString imagePath, QString imageName, QColor tintColor=QColor());

private:
public:
    void AddNotification(QString image_path, QString image_name, QColor color);
    void ClearNotification();
    void renderText(QPainter* painter, qreal lod, ELEMENT_RECT pos, QString text, int textOptions = Qt::AlignVCenter | Qt::AlignLeft | Qt::TextWrapAnywhere);

    void setTooltip(ELEMENT_RECT rect, QString tooltip, QCursor cursor = Qt::ArrowCursor);

    QRectF translatedBoundingRect() const;
    virtual QRectF boundingRect() const = 0;
    virtual QRectF currentRect() const = 0;
    virtual QRectF viewRect() const;
    QRectF sceneViewRect() const;

    QSize iconSize() const;
    QSize smallIconSize() const;
    QPainterPath shape() const;
    QPainterPath sceneShape() const;

    void adjustPos(QPointF delta);
    virtual QPointF getPos() const;

    QPointF validateMove(QPointF delta);

    bool intersectsRectInScene(QRectF rectInScene) const;
    bool isDataProtected(QString keyName) const;

    void addRequiredData(QString keyName);
    void removeRequiredData(QString keyName);
    QStringList getRequiredDataKeys();
    void reloadRequiredData();
    QPen getPen();
    QPen getDefaultPen() const;
    void setDefaultPen(QPen pen);
    QPair<QString, QString> getIconPath();



    virtual QPointF getNearestGridPoint(QPointF newPos=QPointF());

    void setFontSize(int fontSize);

    void setIconOverlay(QString iconAlias, QString iconPath);
    void setIconOverlayVisible(bool visible);
    bool isIconOverlayVisible() const;

    void setTertiaryIcon(QString path, QString image);
    void setTertiaryIconVisible(bool visible);
    bool isTertiaryIconVisible() const;
    QPair<QString, QString> getTertiaryIcon() const;


private:
    int fontSize;
    QFont textFont;

public:
    //Model State Get/Setters
    void setData(QString keyName, QVariant value);
    QVariant getData(QString keyName) const;

    // QVa/riant getProperty(QString propertyName) const;
    bool hasData(QString keyName) const;

    qreal getDefaultZValue() const;
public:
    //State Handlers
    void handleExpand(bool expand);
    void handleHover(bool hovered);
    void handleSelection(bool append);

protected:
    void removeData(QString keyName);
    void setDefaultZValue(qreal z);
public:
    //Item State Getters
    bool isNodeItem();
    bool isEdgeItem();

public:
    //Feature State Setters
    virtual void setExpanded(bool);
    void setMoveEnabled(bool enabled);
    void setHoverEnabled(bool enabled);
    void setExpandEnabled(bool enabled);
    void setSelectionEnabled(bool enabled);

    virtual void setMoveStarted();
    virtual bool setMoveFinished();

public:
    //State Getters
    bool isSelected() const;
    bool isExpanded() const;
    bool isActiveSelected() const;
    bool isHovered() const;
    bool isHighlighted() const;
    bool isMoving() const;

    int getGridSize() const;
    virtual QPointF getTopLeftOffset() const;

public:
    //Feature State Getters
    bool isSelectionEnabled();
    bool isExpandEnabled();
    bool isMoveEnabled();
    bool isHoverEnabled();

    void setIgnorePosition(bool ignore);
    bool isIgnoringPosition();



public slots:
    virtual void dataChanged(QString keyName, QVariant data);
signals:
    //Request changes
    void req_Move(QPointF delta);
    void req_StartMove();
    void req_FinishMove();


    //Request changes
    void req_selected(ViewItem*, bool);
    void req_activeSelected(ViewItem*);

    void req_setData(ViewItem* item, QString keyName, QVariant data);
    void req_removeData(ViewItem* item, QString keyName);
    void req_editData(ViewItem* item, QString keyName);

    void req_triggerAction(QString actionName);

    void req_centerItem(EntityItem* entityItem);
    void req_hovered(EntityItem*, bool);
    void req_expanded(EntityItem*, bool);

    //Inform of changes
    void sizeChanged();
    void positionChanged();
    void scenePosChanged();
private slots:
    virtual void propertyChanged(QString propertyName, QVariant data);
    virtual void dataRemoved(QString keyName);
    void destruct();

public:
    void setHovered(bool isHovered);
    void setHighlighted(bool isHighlight);
    void setSelected(bool selected);
    void setActiveSelected(bool active);
    virtual void updateZValue(bool childSelected = true, bool childActive = false);
private:
    void paintPixmapRect(QPainter* painter, QString imageAlias, QString imageName, QRectF rect);
    void paintPixmap(QPainter* painter, QRectF imageRect, QPixmap pixmap) const;
    QPixmap getPixmap(QString imageAlias, QString imageName, QSize requiredSize=QSize(), QColor tintColor=QColor()) const;
    QSize getPixmapSize(QRectF rect, qreal lod) const;

    void connectViewItem(ViewItem* viewItem);
    void disconnectViewItem();

    EntityItem* parentItem;
    ViewItem* viewItem;
    QStringList requiredDataKeys;

    bool paintIconOverlay;
    QPair<QString, QString> iconOverlayIconPath;
    QPair<QString, QString> secondaryIconPath;
    QPair<QString, QString> tertiaryIconPath;
    bool paintTertiaryIcon;

public:
    QPair<QString, QString> notification_icon;
    QColor notification_color;
    bool paint_notification = false;
private:

    //QHash<ELEMENT_RECT, ImageMap> imageMap;
    QHash<ELEMENT_RECT, StaticTextItem*> textMap;

    QHash<ELEMENT_RECT, QString> tooltipMap;
    QHash<ELEMENT_RECT, QCursor> tooltipCursorMap;



    bool selectEnabled;
    bool hoverEnabled;
    bool moveEnabled;
    bool expandEnabled;

    bool _isHovered;
    bool _isHighlighted;
    bool _isSelected;
    bool _isActiveSelected;
    bool _isExpanded;

    QPointF positionPreMove;

    bool _isMoving;
    bool _isMouseMoving;
    bool _hasMouseMoved;

    QPointF previousMovePoint;

    QPen defaultPen;
    QColor bodyColor;

    KIND kind;
    bool ignorePosition;

    qreal defaultZValue;


protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);


    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // ENTITYITEM_H
