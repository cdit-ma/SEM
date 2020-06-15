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

    void setAlignment(Qt::Orientation _orientation);
    QPointF getElementPosition(BasicNodeItem *child) override;
    
    QPointF GetGridAlignedTopLeft() const;
    
    void SetColumnCount(int column_limit);
    void SetUseColumnCount(int row, int col, bool use_column_count);
    
    void SetRenderCellArea(int row, int col, bool render, bool alt_body_color = false);
    void SetRenderCellText(int row, int col, bool render, const QString& label = "");
    
    void SetRenderCellGapIcons(int row, int col, bool render, const QString& icon_path = "", const QString& icon_name = "");
    void SetRenderCellPrefixIcon(int row, int col, bool render, const QString& icon_path, const QString& icon_name);
    void SetRenderCellSuffixIcon(int row, int col, bool render, const QString& icon_path, const QString& icon_name);
    void SetRenderCellHoverIcons(int row, int col, bool render, const QString& icon_path, const QString& icon_name);
    
    void SetCellSpacing(int row, int col, int spacing);
    void SetCellOrientation(int row, int col, Qt::Orientation _orientation);
    
    Qt::Orientation getCellOrientation(const CellIndex& index);
    
    void childPosChanged(EntityItem* child) override {};
    void dataChanged(const QString& key_name, const QVariant& data) override;
    
    void RecalculateCells();
    
    QMarginsF getDefaultCellMargin() const;
    void setDefaultCellSpacing(qreal spacing);
    
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void bodyHover(bool handle, const QPointF& pos);
    
    QRectF childrenRect() const override;
    void ChildSizeChanged(EntityItem* item);
    void ChildIndexChanged(EntityItem* item);
    void ChildCountChanged();
    
    int getCellColumnCount(const CellIndex& index);
    QMarginsF getCellMargin(const CellIndex& index);
    qreal getDefaultCellSpacing() const;
    qreal getCellSpacing(const CellIndex& index);

    bool allowIcons();
    bool getCellRenderPrefixIcon(const CellIndex& index);
    bool getCellRenderSuffixIcon(const CellIndex& index);
    bool getCellRenderGapIcon(const CellIndex& index);
    
    QRectF GetGapIconRect(const CellIndex& index, int child_index, const QRectF& prev_rect, const QRectF& current_rect);
    
    QRectF getRowRect(int row) const;
    
    void updateCells();
    
    struct PersistentCellInfo
    {
        CellIndex index;

        bool render_rect = false;
        bool render_text = false;
        bool render_prefix_icon = false;
        bool render_suffix_icon = false;
        bool render_gap_icons = false;
        bool render_hover_icon = false;

        Qt::Orientation orientation;
        
        StaticTextItem* text_item = nullptr;
        QPair<QString, QString> prefix_icon;
    
        QPair<QString, QString> suffix_icon;
        QPair<QString, QString> gap_icon;
        QPair<QString, QString> hovered_icon;
        QSize icon_size;
        
        bool use_alt_color = false;
        bool use_column_count = false;
        
        qreal minimum_height = 0;
        qreal minimum_width = 0;

        QMarginsF margin;
        qreal spacing;

        bool render_icons() {
            return render_gap_icons || render_prefix_icon || render_suffix_icon;
        }

        bool render_always() {
            return minimum_width > 0 || minimum_height > 0 || render_prefix_icon || render_suffix_icon;
        }
    };

    struct CellIconRect
    {
        QRectF gap_rect;
        QRectF icon_rect;
        bool hovered = false;
        int index = -1;
    };

    struct Cell
    {
        CellIndex index;
        Qt::Orientation orientation;

        QRectF bounding_rect;
        QRectF child_rect;
        QList<CellIconRect> child_gap_rects;
    
        QHash<NodeItem*, QPointF> child_offsets;
        QList<NodeItem*> children;
        
        bool allocated = false;
        int prefix_gap_index = -1;
        int suffix_gap_index = -1;
    };
    
    Qt::Orientation orientation;
    
    int column_count_ = -1;
    bool sub_areas_dirty = true;

    QHash<CellIndex, PersistentCellInfo> cell_infos;
    QHash<CellIndex, Cell> cells;
    
    QRectF cell_rect;
    qreal cell_spacing;

    PersistentCellInfo& SetupCellInfo(int row, int col);
};

#endif // STACKNODEITEM_H
