#include "stacknodeitem.h"

#include <QDebug>
#include <cmath>

StackNodeItem::StackNodeItem(NodeViewItem *viewItem, NodeItem *parentItem, Qt::Orientation orientation):
    BasicNodeItem(viewItem, parentItem)
{
    setSortOrdered(true);
    setResizeEnabled(false);
    setGridEnabled(false);

    removeRequiredData("width");
    removeRequiredData("height");
    reloadRequiredData();
    this->orientation = orientation;
}

QPointF StackNodeItem::getStemAnchorPoint() const
{
    //QPointF offset;
    return gridRect().topLeft();
}

int StackNodeItem::GetHorizontalGap() const{
    return 4 * getGridSize();
}
int StackNodeItem::GetVerticalGap() const{
    return getGridSize();
}

QMap<int, QMap<int, QSizeF> > StackNodeItem::GetGridRectangles(const QList<NodeItem*> &children, int ignore_indexes_higher_than){
    QMap<int, QMap<int, QSizeF> > grid_rectangles;

    bool ignore_any = ignore_indexes_higher_than != -1;
    auto should_cache = !ignore_any && children.size();

    if(should_cache && cached_grid_rectangles.size()){
        qCritical() << "Using Cache!";
        return cached_grid_rectangles;
    }

    for(auto c : children){
        auto c_index = c->getSortOrder();
        auto c_row = c->getSortOrderRow();
        auto c_row_group = c->getSortOrderRowSubgroup();
        cached_node_items[c_row].insert(c_row_group,c);
        
        if(ignore_any && c_index >= ignore_indexes_higher_than){
            continue;
        }
        

        //Insert into lookup
        auto c_size = c->currentRect();
        
        auto& row_size = grid_rectangles[c_row][c_row_group];
        
        bool stack_height = c_row == 0 && c_row_group != 0;
        bool stack_width = !stack_height;

        auto adjusted_width = c_size.width() + GetHorizontalGap();

        if(stack_height){
            
            auto adjusted_height = c_size.height() + GetVerticalGap();
            row_size.rheight() += adjusted_height;
            
            if(row_size.width() < adjusted_width){
                row_size.rwidth() = adjusted_width;
            }
        }
        
        if(stack_width){
            row_size.rwidth() += adjusted_width;

            if(row_size.height() < c_size.height()){
                row_size.rheight() = c_size.height();
            }
        }
    }

    if(should_cache){
        qCritical() << "Storing Cache!";
        cached_grid_rectangles = grid_rectangles;
    }
    
    return grid_rectangles;
}

QPointF StackNodeItem::GetGridAlignedTopLeft() const{
    auto grid_size = getGridSize();
    auto item_offset = gridRect().topLeft();

    //offset to the nearest grid
    qreal x_mod = fmod(item_offset.x(), grid_size);
    qreal y_mod = fmod(item_offset.y(), grid_size);

    if(x_mod > 0){
        item_offset.rx() += grid_size - x_mod;
    }

    if(y_mod > 0){
        item_offset.ry() += grid_size - y_mod;
    }

    return item_offset;
}

QPointF StackNodeItem::getElementPosition(BasicNodeItem *child)
{
    auto child_index = child->getSortOrder();
    auto child_row = child->getSortOrderRow();
    auto child_col = child->getSortOrderRowSubgroup();

    auto item_offset = GetGridAlignedTopLeft();
    
    auto grid_sizes = GetGridRectangles(getChildNodes(), child_index);

    for(auto row : grid_sizes.keys()){
        auto row_width = 0;
        auto row_height = 0;
        auto& row_map = grid_sizes[row];
        
        for(auto col : row_map.keys()){
            auto& row_col_size = row_map[col];

            auto row_height_offset = col == 0 ? GetVerticalGap() : 0;

            auto adjusted_height = row_col_size.height() + row_height_offset;

            if(row < child_row){
                if(adjusted_height > row_height){
                    row_height = adjusted_height; 
                }
            }

            if(row == child_row){
                bool sum_height = row == 0 && (col == child_col && col != 0);
                bool sum_width = !sum_height;

                if(sum_width){
                    row_width += row_col_size.width();
                }else if(sum_height){
                    
                    if(adjusted_height > row_height){
                        row_height = adjusted_height; 
                    }
                }
            }
        }

        item_offset.rx() += row_width;
        item_offset.ry() += row_height;
    }
    
    return item_offset;
}



void StackNodeItem::childPosChanged(EntityItem* child)
{
    cached_grid_rectangles.clear();
    cached_node_items.clear();
    
    auto new_index = child ? child->getData("index").toInt() : 0;
    auto ordered_children = getSortedChildNodes();

    for(auto c : ordered_children){
        auto child_index = c->getSortOrder();
        if(child_index >= new_index){
            c->setPos(QPointF());
        }
    }

    BasicNodeItem::childPosChanged(child);
    update();
}



void StackNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    BasicNodeItem::paintBackground(painter, option, widget);

    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);
    painter->setClipRect(option->exposedRect);

    
    if(state >= RENDER_STATE::BLOCK && isExpanded()){
        painter->setPen(Qt::NoPen);

        auto children = getChildNodes();
        
        if(!cached_node_items.size() && children.size()){
            GetGridRectangles(children);
        }
        auto body_rect = bodyRect();
        {
            
            QRectF rect;

            for(auto node : cached_node_items[0].values(-1)){
                rect = rect.united(node->translatedBoundingRect());
            }




            if(rect.isValid()){
                rect.setBottom(body_rect.bottom());
                painter->setPen(Qt::NoPen);
                painter->setBrush(getHeaderColor().lighter(110));
                
                painter->drawRect(rect);

                if(!left_text_item){
                    left_text_item = new StaticTextItem(Qt::AlignHCenter | Qt::AlignTop);
                }
                rect.setHeight(getGridSize());
                painter->setPen(getPen());
                left_text_item->RenderText(painter, getRenderState(lod), rect, "Input Parameters");
            }
        }

        {
             QRectF rect;

            for(auto col : cached_node_items[1].uniqueKeys()){
                for(auto node : cached_node_items[1].values(col)){
                    rect = rect.united(node->translatedBoundingRect());
                }
            }


            if(rect.isValid()){
                painter->setPen(Qt::NoPen);
                painter->setBrush(getHeaderColor().lighter(110));
                rect.setBottom(body_rect.bottom());
                rect.setRight(body_rect.right());
                
                painter->drawRect(rect);

                if(!bottom_text_item){
                    bottom_text_item = new StaticTextItem(Qt::AlignHCenter | Qt::AlignTop);
                }
                rect.setHeight(getGridSize());
                painter->setPen(getPen());
                bottom_text_item->RenderText(painter, getRenderState(lod), rect, "Variables");
            }
        }
    }
    NodeItem::paint(painter, option, widget);

}