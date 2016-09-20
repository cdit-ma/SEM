#ifndef ENTITYITEMNEW_H
#define ENTITYITEMNEW_H

#include "../viewitem.h"
#include <QGraphicsObject>
#include <QGraphicsSceneHoverEvent>
#include <QPen>

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#define ICON_RATIO (4.0 / 6.0)
#define SMALL_ICON_RATIO (1.0 / 6.0)
#define LABEL_RATIO (2.5 / 6.0)
#define DEFAULT_SIZE 80

#define SELECTED_LINE_WIDTH 3

struct ImageMap{
    QPixmap pixmap;
    QString imageAlias;
    QString imageName;
    QSizeF imageSize;
};

//Forward class definition
class NodeItemNew;

class EntityItemNew: public QGraphicsObject
{
    Q_OBJECT

public:
    static const int ENTITY_ITEM_KIND = UserType + 1;
    enum KIND{
        EDGE,
        NODE,
    };
    enum ELEMENT_RECT{ER_PRIMARY_TEXT, ER_SECONDARY_TEXT, ER_MAIN_ICON, ER_MAIN_ICON_OVERLAY, ER_SECONDARY_ICON, ER_EXPANDED_STATE, ER_LOCKED_STATE, ER_STATUS, ER_CONNECT, ER_CONNECT_ICON, ER_EDGE_KIND_ICON, ER_INFORMATION, ER_NOTIFICATION, ER_EXPANDCONTRACT, ER_SELECTION, ER_DEPLOYED, ER_QOS, ER_MOVE};
    enum RENDER_STATE{RS_NONE, RS_BLOCK, RS_MINIMAL, RS_REDUCED, RS_FULL};

    EntityItemNew(ViewItem *viewItem, EntityItemNew* parentItem, KIND kind);
    ~EntityItemNew();
    int type() const;

    RENDER_STATE getRenderState(qreal lod) const;
    EntityItemNew* getParent() const;
    NodeItemNew* getParentNodeItem() const;

    void unsetParent();
    bool isTopLevelItem() const;
    bool isReadOnly() const;


    ViewItem* getViewItem() const;

    virtual QPointF getSceneEdgeTermination(bool left) const = 0;


    QColor getBaseBodyColor() const;
    virtual QColor getBodyColor() const;
    void setBaseBodyColor(QColor color);

    virtual void setPos(const QPointF &pos);
    int getID();

    virtual QRectF getElementRect(ELEMENT_RECT rect) const;
    virtual QPainterPath getElementPath(ELEMENT_RECT rect) const;

    void paintPixmap(QPainter *painter, qreal lod, ELEMENT_RECT pos, QString imageAlias, QString imageName, QColor tintColor=QColor(), bool update=false);
    void paintPixmap(QPainter *painter, qreal lod, QRectF imageRect, QString imageAlias, QString imageName, QColor tintColor=QColor());
    void paintPixmap(QPainter *painter, qreal lod, QRectF imageRect, QPair<QString, QString> image, QColor tintColor=QColor());
    void paintPixmap(QPainter *painter, qreal lod, ELEMENT_RECT pos, QPair<QString, QString> image, QColor tintColor=QColor());

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
    void setDefaultPen(QPen pen);
    QPair<QString, QString> getIconPath();



    virtual QPointF getNearestGridPoint(QPointF newPos=QPointF());

    void setFontSize(int fontSize);

    void setIconOverlay(QString iconAlias, QString iconPath);
    void setIconOverlayVisible(bool visible);


private:
    int fontSize;
    QFont textFont;

public:
    //Model State Get/Setters
    void setData(QString keyName, QVariant value);
    QVariant getData(QString keyName) const;

    QVariant getProperty(QString propertyName) const;
    bool hasData(QString keyName) const;
    bool isDataEditable(QString keyName);

public:
    //State Handlers
    void handleExpand(bool expand);
    void handleHover(bool hovered);
    void handleSelection(bool append);

protected:
    void removeData(QString keyName);
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
    int getMajorGridCount() const;
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

    void req_centerItem(EntityItemNew* entityItem);
    void req_hovered(EntityItemNew*, bool);
    void req_expanded(EntityItemNew*, bool);

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

    void renderText(QPainter* painter, qreal lod, QRectF textRect, QString text, int fontSize = -1) const;
private:
    void paintPixmapRect(QPainter* painter, QString imageAlias, QString imageName, QRectF rect);
    void paintPixmap(QPainter* painter, QRectF imageRect, QPixmap pixmap) const;
    QPixmap getPixmap(QString imageAlias, QString imageName, QSize requiredSize=QSize(), QColor tintColor=QColor()) const;
    QSize getPixmapSize(QRectF rect, qreal lod) const;

    void connectViewItem(ViewItem* viewItem);
    void disconnectViewItem();

    EntityItemNew* parentItem;
    ViewItem* viewItem;
    QStringList requiredDataKeys;

    bool paintIconOverlay;
    QPair<QString, QString> iconOverlayIconPath;
    QPair<QString, QString> secondaryIconPath;


    QHash<ELEMENT_RECT, ImageMap> imageMap;

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

#endif // GRAPHMLITEM_H
