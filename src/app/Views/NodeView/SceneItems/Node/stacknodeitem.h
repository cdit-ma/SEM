#ifndef STACKNODEITEM_H
#define STACKNODEITEM_H

#include "basicnodeitem.h"

typedef QPair<int, int> CellIndex;



class StackNodeItem: public BasicNodeItem
{
    Q_OBJECT
public:
    StackNodeItem(NodeViewItem *viewItem, NodeItem *parentItem, Qt::Orientation orientation = Qt::Vertical);
    static CellIndex GetCellIndex(NodeItem* child);

    void setAlignment(Qt::Orientation orientation);
    QPointF getElementPosition(BasicNodeItem *child);

    QPointF GetGridAlignedTopLeft() const;
    
    void SetColumnLimit(int column_limit);

    void SetRenderCellArea(int row, int col, bool render, bool alt_body_color = false);

    void SetRenderCellText(int row, int col, bool render, QString label = "");
    
    void SetRenderCellGapIcons(int row, int col, bool render, QString icon_path = "", QString icon_name = "");
    void SetRenderCellPrefixIcon(int row, int col, bool render, QString icon_path, QString icon_name);
    void SetRenderCellSuffixIcon(int row, int col, bool render, QString icon_path, QString icon_name);
    void SetRenderCellHoverIcons(int row, int col, bool render, QString icon_path, QString icon_name);
    
    void SetCellOrientation(int row, int col, Qt::Orientation orientation);
    void SetCellSpacing(int row, int col, int spacing);


    void childPosChanged(EntityItem* child);
    void RecalculateCells();

    Qt::Orientation getCellOrientation(const CellIndex& index);
    void dataChanged(const QString& key_name, const QVariant& data);
protected:
    
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
public:
    QMarginsF getDefaultCellMargin() const;
    void setDefaultCellSpacing(qreal spacing);


    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    void bodyHover(bool handle, const QPointF& pos);
    QRectF childrenRect() const;
    void ChildSizeChanged(EntityItem* item);
    void ChildIndexChanged(EntityItem* item);
    void ChildCountChanged();
    
    
    
    QMarginsF getCellMargin(const CellIndex& index);
    qreal getDefaultCellSpacing() const;
    qreal getCellSpacing(const CellIndex& index);
    

    QRectF GetGapIconRect(const Qt::Orientation orientation, const QRectF& prev_rect, const QRectF& current_rect);

    Qt::Orientation orientation;

    void updateCells();

    QRectF getRowRect(int row) const;
    

    

    struct PersistentCellInfo{
        CellIndex index;

        bool render_rect = false;
        bool render_text = false;
        bool render_prefix_icon = false;
        bool render_suffix_icon = false;
        bool render_gap_icons = false;
        bool render_hover_icon = false;

        
        
        

        Qt::Orientation orientation;
        
        bool use_alt_color = false;
        StaticTextItem* text_item = 0;

        QPair<QString, QString> prefix_icon;
        QPair<QString, QString> suffix_icon;
        QPair<QString, QString> gap_icon;
        QPair<QString, QString> hovered_icon;
        QSize icon_size;

        qreal minimum_height = 0;
        qreal minimum_width = 0;

        QMarginsF margin;
        qreal spacing;

        bool render_icons(){
            return render_gap_icons || render_prefix_icon || render_suffix_icon;
        }

        bool render_always(){
            return minimum_width > 0 || minimum_height > 0 || render_suffix_icon;
        }
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

        bool allocated = false;

        QRectF bounding_rect;
        QRectF child_rect;

        QList<CellIconRect> child_gap_rects;
        QHash<NodeItem*, QPointF> child_offsets;
        QList<NodeItem*> children;
        int prefix_gap_index = -1;
        int suffix_gap_index = -1;
    };
    int column_limit_ = -1;


    bool sub_areas_dirty = true;

    QHash<CellIndex, PersistentCellInfo> cell_infos;
    QHash<CellIndex, Cell> cells;
    QRectF cell_rect;

    qreal cell_spacing;

    PersistentCellInfo& SetupCellInfo(int row, int col);
};

#endif // STACKNODEITEM_H
