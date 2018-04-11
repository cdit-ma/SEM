#ifndef STACKNODEITEM_H
#define STACKNODEITEM_H

#include "basicnodeitem.h"

typedef QPair<int, int> CellIndex;



class StackNodeItem: public BasicNodeItem
{
    Q_OBJECT
public:
    StackNodeItem(NodeViewItem *viewItem, NodeItem *parentItem, Qt::Orientation orientation = Qt::Vertical);


    QPointF getStemAnchorPoint() const;
    QPointF getElementPosition(BasicNodeItem *child);

    QPointF GetGridAlignedTopLeft() const;

    void SetRenderCellArea(int row, int col, bool render, QColor color = QColor());
    void SetRenderCellText(int row, int col, bool render, QString label = "", QColor color = QColor());
    void SetRenderCellIcons(int row, int col, bool render, QString icon_path = "", QString icon_name = "", QSize icon_size = QSize());
    void SetCellOrientation(int row, int col, Qt::Orientation orientation);
    void SetCellSpacing(int row, int col, int spacing);

    void childPosChanged(EntityItem* child);



    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private:
    void ChildSizeChanged(EntityItem* item);
    void ChildIndexChanged(EntityItem* item);
    void ChildCountChanged();
    
    void RecalculateCells();
    
    QMarginsF getDefaultCellMargin() const;
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
        
        QColor rect_color;
        StaticTextItem* text_item = 0;
        QColor text_color;
        QPair<QString, QString> icon;
        QSize icon_size;


        QMarginsF margin;
        qreal spacing;
    };

    struct Cell{
        CellIndex index;
        Qt::Orientation orientation;

        QRectF bounding_rect;
        QRectF child_rect;

        QList<QRectF> child_gap_rects;
        QMap<NodeItem*, QPointF> child_offsets;
        QList<NodeItem*> children;
    };

    bool sub_areas_dirty = true;

    QMap<CellIndex, PersistentCellInfo> cell_info;
    QMap<CellIndex, Cell> cells;

    PersistentCellInfo& SetupCellInfo(int row, int col);
};

#endif // STACKNODEITEM_H
