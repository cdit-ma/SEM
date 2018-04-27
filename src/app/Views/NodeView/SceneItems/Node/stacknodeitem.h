#ifndef STACKNODEITEM_H
#define STACKNODEITEM_H

#include "basicnodeitem.h"

typedef QPair<int, int> CellIndex;



class StackNodeItem: public BasicNodeItem
{
    Q_OBJECT
public:
    StackNodeItem(NodeViewItem *viewItem, NodeItem *parentItem, Qt::Orientation orientation = Qt::Vertical);

    void setAlignment(Qt::Orientation orientation);
    QPointF getStemAnchorPoint() const;
    QPointF getElementPosition(BasicNodeItem *child);

    QPointF GetGridAlignedTopLeft() const;

    void SetRenderCellArea(int row, int col, bool render, bool alt_body_color = false);
    void SetRenderCellText(int row, int col, bool render, QString label = "");
    void SetRenderCellIcons(int row, int col, bool render, QString icon_path = "", QString icon_name = "", QSize icon_size = QSize());
    void SetRenderCellHoverIcons(int row, int col, QString icon_path, QString icon_name);
    
    void SetCellOrientation(int row, int col, Qt::Orientation orientation);
    void SetCellSpacing(int row, int col, int spacing);
    void SetCellMargins(int row, int col, QMarginsF margins);

    void childPosChanged(EntityItem* child);

protected:
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
public:
    QMarginsF getDefaultCellMargin() const;
    void setDefaultCellSpacing(qreal spacing);


    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    QRectF childrenRect() const;
    void ChildSizeChanged(EntityItem* item);
    void ChildIndexChanged(EntityItem* item);
    void ChildCountChanged();
    
    void RecalculateCells();
    
    QMarginsF getCellMargin(const CellIndex& index) const;
    qreal getDefaultCellSpacing() const;
    qreal getCellSpacing(const CellIndex& index) const;
    Qt::Orientation getCellOrientation(const CellIndex& index) const;

    int GetHorizontalGap() const;
    int GetVerticalGap() const;
    Qt::Orientation orientation;

    void updateCells();

    QRectF getRowRect(int row) const;
    

    static CellIndex GetCellIndex(NodeItem* child);

    struct PersistentCellInfo{
        CellIndex index;

        bool render_rect = false;
        bool render_text = false;
        bool render_icons = false;

        Qt::Orientation orientation;
        
        bool use_alt_color = false;
        StaticTextItem* text_item = 0;
        QPair<QString, QString> icon;
        QPair<QString, QString> hovered_icon;
        QSize icon_size;


        QMarginsF margin;
        qreal spacing;
    };

    struct CellIconRect{
        QRectF gap_rect;
        QRectF icon_rect;
        bool hovered = false;
        int index = -1;
    };

    struct Cell{
        CellIndex index;
        Qt::Orientation orientation;

        QRectF bounding_rect;
        QRectF child_rect;

        QList<CellIconRect> child_gap_rects;
        QMap<NodeItem*, QPointF> child_offsets;
        QList<NodeItem*> children;
    };



    bool sub_areas_dirty = true;

    QMap<CellIndex, PersistentCellInfo> cell_info;
    QMap<CellIndex, Cell> cells;
    QRectF cell_rect;

    qreal cell_spacing;

    PersistentCellInfo& SetupCellInfo(int row, int col);
};

#endif // STACKNODEITEM_H
