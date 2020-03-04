#ifndef ENTITYITEM_H
#define ENTITYITEM_H

#include "../../../Controllers/ViewController/viewitem.h"
#include "../../../Controllers/ViewController/edgeviewitem.h"
#include "../../../../modelcontroller/keynames.h"
#include "../../../theme.h"
#include "statictextitem.h"

#include <QGraphicsObject>
#include <QGraphicsSceneHoverEvent>
#include <QPen>
#include <QPainter>
#include <QStack>
#include <QStyleOptionGraphicsItem>

#include <functional>

enum class RENDER_STATE{NONE, BLOCK, MINIMAL, REDUCED, FULL};

class NodeItem;
class EntityItem : public QGraphicsObject
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
    ~EntityItem() override;
    
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
    
    // NOTE: This is a non-virtual function - QGraphicsObject's function is called from within
    virtual void setPos(const QPointF &pos);
    
    int getID();
    
    virtual QRectF getElementRect(EntityRect rect) const = 0;
    virtual QPainterPath getElementPath(EntityRect rect) const;
    
    void paintPixmap(QPainter *painter, qreal lod, EntityRect pos, const QPair<QString, QString>& image, QColor tintColor=QColor());
    void paintPixmap(QPainter *painter, qreal lod, EntityRect entityRect, const QString& imagePath, const QString& imageName, QColor tintColor=QColor());
    void paintPixmap(QPainter *painter, qreal lod, const QRectF& pos, const QPair<QString, QString>& image, QColor tintColor=QColor());
    void paintPixmap(QPainter *painter, qreal lod, const QRectF& pos, const QString& imagePath, const QString& imageName, QColor tintColor=QColor());
    
    void renderText(QPainter* painter, qreal lod, EntityRect pos, QString text, int textOptions = Qt::AlignVCenter | Qt::AlignLeft | Qt::TextWrapAnywhere);
    
    void addHoverFunction(EntityRect rect, std::function<void (bool, QPointF)> func);
    
    QRectF translatedBoundingRect() const;
    QRectF boundingRect() const override = 0;
    virtual QRectF currentRect() const = 0;
    virtual QRectF viewRect() const;
    
    QSize smallIconSize() const;
    QPainterPath shape() const override;
    
    void adjustPos(QPointF delta);
    virtual QPointF getPos() const;
    
    virtual QPointF validateMove(QPointF delta);
    
    bool isDataProtected(const QString& keyName) const;
    
    void addRequiredData(const QString& keyName);
    void removeRequiredData(const QString& keyName);
    bool isDataRequired(const QString& key_name) const;
    
    void reloadRequiredData();
    QPen getPen();
    void setDefaultPen(QPen pen);
    
    virtual QPointF getNearestGridPoint(QPointF newPos);
    
    void setIconVisible(const EntityRect& rect, const QPair<QString, QString>& icon, bool visible);
    void setIconVisible(const EntityRect& rect, bool visible);
    bool isIconVisible(const EntityRect& rect) const;
    
    const QPair<QString, QString>& getIcon(const EntityRect& rect);
    void setIcon(const EntityRect& rect, const QPair<QString, QString>& icon);
    bool gotIcon(const EntityRect& rect) const;

    QVariant getData(const QString& key_name) const;
    bool hasData(const QString& key_name) const;
    
    qreal getDefaultZValue() const;

    //State Handlers
    void handleExpand(bool expand);
    void handleSelection(bool append);

    //Item State Getters
    bool isNodeItem();
    bool isEdgeItem();
    
    //Feature State Setters
    virtual void setExpanded(bool);
    
    void setMoveEnabled(bool enabled);
    void setExpandEnabled(bool enabled);
    virtual void setMoveStarted();
    
    virtual bool setMoveFinished();
    
    //State Getters
    bool isExpanded() const;
    
    bool isSelected() const; // NOTE: This is a non-virtual function
    bool isActiveSelected() const;
    
    bool isHovered() const;
    bool isHovered(EntityRect area) const;
    bool isHighlighted() const;
    bool isMoving() const;
    
    int getGridSize() const;
    virtual QPointF getTopLeftOffset() const { return {}; };
    
    //Feature State Getters
    virtual bool isExpandEnabled();
    bool isMoveEnabled();
    void setIgnorePosition(bool ignore);
    
    bool isIgnoringPosition();
    void setHovered(bool isHovered);
    void setHighlighted(bool isHighlight);
    
    void setSelected(bool selected); // NOTE: This is a non-virtual function
    void setActiveSelected(bool active);
    
    virtual void updateZValue(bool childSelected, bool childActive);
    
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

signals:
    //Request changes
    void req_Move(QPointF delta);
    void req_StartMove();
    void req_FinishMove();

    void req_selected(ViewItem*, bool);
    void req_activeSelected(ViewItem*);

    void req_removeData(ViewItem* item, QString keyName);
    void req_editData(ViewItem* item, QString keyName);

    void req_triggerAction(QString actionName);

    void req_centerItem(EntityItem* entityItem);
    void req_expanded(EntityItem*, bool);

    void hoveredChanged();
    void selectionChanged();

    //Inform of changes
    void sizeChanged();
    void positionChanged();

public slots:
    virtual void dataChanged(const QString& key_name, const QVariant& data);
    virtual void dataRemoved(const QString& key_name){};

protected:
    void removeData(const QString& keyName);
    void setDefaultZValue(qreal z);
    
    void paintPixmapRect(QPainter* painter, const QString& imageAlias, const QString& imageName, QRectF rect);
    
    void AddTooltip(const QString& tooltip);
    void AddCursor(const QCursor& cursor);

    StaticTextItem* getTextItem(EntityRect rect);

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    
private:
    void updateIcon();
    
    void shapeHover(bool handle, const QPointF& point);
    void setAreaHovered(EntityRect rect, bool is_hovered);
    
    void paintPixmap(QPainter* painter, QRectF imageRect, const QPixmap& pixmap) const;
    QPixmap getPixmap(const QString& imageAlias, const QString& imageName, QSize requiredSize = QSize(), QColor tintColor = QColor()) const;
    QSize getPixmapSize(QRectF rect, qreal lod) const;
    
    void connectViewItem(ViewItem* viewItem);
    void disconnectViewItem();


    EntityItem* parent_item = nullptr;
    ViewItem* view_item = nullptr;
    
    qreal default_z_value = 0;
    
    QHash<EntityRect, std::function<void (bool, const QPointF&)> > hover_function_map;
    
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

    QPointF positionPreMove;
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
};

inline uint qHash(EntityItem::EntityRect key, uint seed){
    return ::qHash(static_cast<uint>(key), seed);
};

#endif // ENTITYITEM_H
