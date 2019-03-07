#ifndef ENTITYITEM_H
#define ENTITYITEM_H

#include "../../../Controllers/ViewController/viewitem.h"
#include "../../../Controllers/ViewController/edgeviewitem.h"
#include "../../../../modelcontroller/keynames.h"
#include "../../../theme.h"
#include <QGraphicsObject>
#include <QGraphicsSceneHoverEvent>
#include <QPen>
#include <QPainter>
#include <QStack>
#include <QStyleOptionGraphicsItem>
#include "statictextitem.h"

#include <functional>

#define DEFAULT_SIZE 80

#define SELECTED_LINE_WIDTH 3

enum class RENDER_STATE{NONE, BLOCK, MINIMAL, REDUCED, FULL};
//Forward class definition
class NodeItem;

class EntityItem: public QGraphicsObject
{
    Q_OBJECT

public:
    enum KIND{
        EDGE,
        NODE,
    };
    enum class EntityRect{
        SHAPE,
        MOVE,
        EXPAND_CONTRACT,

        HEADER,
        BODY,

        PRIMARY_TEXT,
        SECONDARY_TEXT,
        TERTIARY_TEXT,
        
        MAIN_ICON,
        MAIN_ICON_OVERLAY,
        SECONDARY_ICON,
        TERTIARY_ICON,
        
        LOCKED_STATE_ICON,
        NOTIFICATION_RECT,
        RESIZE_ARROW_ICON,

        CONNECT_SOURCE,
        CONNECT_TARGET
    };
    static QList<EntityRect> GetEntityRects();
    

    EntityItem(ViewItem *viewItem, EntityItem* parentItem, KIND kind);
    ~EntityItem();

    bool isHidden() const;

    RENDER_STATE getRenderState(qreal lod) const;
    EntityItem* getParent() const;
    NodeItem* getParentNodeItem() const;

    void unsetParent();
    bool isReadOnly() const;


    ViewItem* getViewItem() const;

    virtual QPointF getSceneEdgeTermination(EDGE_DIRECTION direction, EDGE_KIND kind) const = 0;

    QColor getTextColor() const;
    void setTextColor(QColor color);

    QColor getHighlightTextColor() const;
    void setHighlightTextColor(QColor color);

    virtual QColor getHeaderColor() const;
    void setHeaderColor(QColor color);


    QColor getHighlightColor() const;
    void setHighlightColor(QColor color);
   
    
    QColor getBaseBodyColor() const;
    virtual QColor getBodyColor() const;
    void setBaseBodyColor(QColor color);

    QColor getAltBodyColor() const;
    void setAltBodyColor(QColor color);

    QColor getAltTextColor() const;
    void setAltTextColor(QColor color);

    virtual void setPos(const QPointF &pos);
    int getID();

    virtual QRectF getElementRect(EntityRect rect) const = 0;
    virtual QPainterPath getElementPath(EntityRect rect) const;

    void paintPixmap(QPainter *painter, qreal lod, EntityRect pos, const QPair<QString, QString>& image, QColor tintColor=QColor());
    void paintPixmap(QPainter *painter, qreal lod, EntityRect pos, const QString& imagePath, const QString& imageName, QColor tintColor=QColor());

    void paintPixmap(QPainter *painter, qreal lod, const QRectF& pos, const QPair<QString, QString>& image, QColor tintColor=QColor());

    void paintPixmap(QPainter *painter, qreal lod, const QRectF& pos, const QString& imagePath, const QString& imageName, QColor tintColor=QColor());

private:
public:
    void renderText(QPainter* painter, qreal lod, EntityRect pos, QString text, int textOptions = Qt::AlignVCenter | Qt::AlignLeft | Qt::TextWrapAnywhere);

    void addHoverFunction(EntityRect rect, std::function<void (bool, QPointF)> func);
    void removeHoverFunction(EntityRect rect);

    QRectF translatedBoundingRect() const;
    virtual QRectF boundingRect() const = 0;
    virtual QRectF currentRect() const = 0;
    virtual QRectF viewRect() const;
    QRectF sceneViewRect() const;

    QSize smallIconSize() const;
    QPainterPath shape() const;

    void adjustPos(QPointF delta);
    virtual QPointF getPos() const;

    virtual QPointF validateMove(QPointF delta);

    bool isDataProtected(QString keyName) const;

    void addRequiredData(const QString& keyName);
    void removeRequiredData(const QString& keyName);
    bool isDataRequired(const QString& key_name) const;


    void reloadRequiredData();
    QPen getPen();
    QPen getDefaultPen() const;
    void setDefaultPen(QPen pen);
    

    virtual QPointF getNearestGridPoint(QPointF newPos=QPointF());

    void setFontSize(int fontSize);


    void setIconVisible(const EntityRect rect, const QPair<QString, QString>& icon, bool visible);
    void setIconVisible(const EntityRect rect, bool visible);

    bool gotIcon(const EntityRect rect) const;
    bool isIconVisible(const EntityRect rect) const;
    const QPair<QString, QString>& getIcon(const EntityRect rect);
    void setIcon(const EntityRect rect, const QPair<QString, QString>& icon);

private:
    void shapeHover(bool handle, const QPointF& point);
    
public:
    QVariant getData(const QString& key_name) const;
    bool hasData(const QString& key_name) const;

    qreal getDefaultZValue() const;
public:
    //State Handlers
    void handleExpand(bool expand);
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

    virtual void setMoveStarted();
    virtual bool setMoveFinished();

public:
    //State Getters
    bool isSelected() const;
    bool isExpanded() const;
    bool isContracted() const;
    
    bool isActiveSelected() const;
    bool isHovered() const;
    bool isHighlighted() const;
    bool isMoving() const;

    int getGridSize() const;
    virtual QPointF getTopLeftOffset() const;

    bool isHovered(EntityRect area) const;

public:
    //Feature State Getters
    bool isSelectionEnabled();
    virtual bool isExpandEnabled();
    bool isMoveEnabled();
    bool isHoverEnabled();

    void setIgnorePosition(bool ignore);
    bool isIgnoringPosition();
private:
    void updateIcon();
public slots:
    virtual void dataChanged(const QString& key_name, const QVariant& data);
    virtual void dataRemoved(const QString& key_name){};

signals:
    //Request changes
    void req_Move(QPointF delta);
    void req_StartMove();
    void req_FinishMove();

    //Request changes
    void req_selected(ViewItem*, bool);
    void req_activeSelected(ViewItem*);

    void req_removeData(ViewItem* item, QString keyName);
    void req_editData(ViewItem* item, QString keyName);

    void req_triggerAction(QString actionName);

    void req_centerItem(EntityItem* entityItem);
    void req_hovered(EntityItem*, bool);
    void req_expanded(EntityItem*, bool);

    void hoveredChanged();
    void selectionChanged();

    //Inform of changes
    void sizeChanged();
    void positionChanged();
    void scenePosChanged();
private slots:
    void destruct();
private:
    void setAreaHovered(EntityRect rect, bool is_hovered);
public:
    void setHovered(bool isHovered);
    void setHighlighted(bool isHighlight);
    void setSelected(bool selected);
    void setActiveSelected(bool active);
    virtual void updateZValue(bool childSelected = true, bool childActive = false);

protected:
    void paintPixmapRect(QPainter* painter, QString imageAlias, QString imageName, QRectF rect);
    void AddTooltip(const QString& tooltip);
    void AddCursor(const QCursor& cursor);
private:
    void paintPixmap(QPainter* painter, QRectF imageRect, QPixmap pixmap) const;
    QPixmap getPixmap(const QString& imageAlias, const QString& imageName, QSize requiredSize=QSize(), QColor tintColor=QColor()) const;
    QSize getPixmapSize(QRectF rect, qreal lod) const;

    void connectViewItem(ViewItem* viewItem);
    void disconnectViewItem();

protected:
    StaticTextItem* getTextItem(EntityRect rect);
private:
    QHash<EntityRect, std::function<void (bool, const QPointF&)> > hover_function_map;

    EntityItem* parent_item = 0;
    ViewItem* view_item = 0;
    qreal default_z_value = 0;

    QHash<EntityRect, StaticTextItem*> textMap;
    
    QHash<EntityRect, QPair<QString, QString> > icon_map;
    QSet<EntityRect> visible_icons;

    QSet<QString> required_data_keys;
    QSet<EntityRect> hovered_areas;

    QStack<QString> tooltip_stack;
    QStack<QCursor> cursor_stack;



    bool is_move_enabled = true;
    bool is_expand_enabled = false;

    bool is_hovered = false;
    bool is_highlighted = false;
    bool is_selected = false;
    bool is_active_selected = false;
    
    bool is_expanded = true;
    bool is_moving = false;
    bool is_mouse_moving = false;
    bool has_mouse_moving = false;

    bool is_ignoring_positon = false;

    bool paintTertiaryIcon;

    QPointF positionPreMove;

    bool _isMouseMoving;
    bool _hasMouseMoved;

    QPointF previousMovePoint;

    QPen defaultPen;
    QColor bodyColor;
    QColor text_color;
    QColor alt_text_color;
    QColor highlight_color;
    QColor header_color;
    QColor alt_body_color;
    QColor highlight_text_color;

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

inline uint qHash(EntityItem::EntityRect key, uint seed){
    return ::qHash(static_cast<uint>(key), seed);
};

#endif // ENTITYITEM_H
