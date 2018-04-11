#include "stacknodeitem.h"

#include <QDebug>
#include <QDateTime>
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

    disconnect(this, &NodeItem::childSizeChanged, this, &NodeItem::childPosChanged);
    disconnect(this, &NodeItem::childPositionChanged, this, &NodeItem::childPosChanged);

    connect(this, &NodeItem::childSizeChanged, this, &StackNodeItem::ChildSizeChanged);
    connect(this, &NodeItem::childIndexChanged, this, &StackNodeItem::ChildIndexChanged);
    connect(this, &NodeItem::childCountChanged, this, &StackNodeItem::ChildCountChanged);

    RecalculateCells();
}
void StackNodeItem::RecalculateCells(){
    sub_areas_dirty = true;
    updateCells();
}

void StackNodeItem::ChildCountChanged(){
    //qCritical() << "CHILD COUNT CHANGED";
    RecalculateCells();
}

void StackNodeItem::ChildSizeChanged(EntityItem* item){
    //qCritical() << "CHILD CHANGED SIZE";
    RecalculateCells();
}

void StackNodeItem::ChildIndexChanged(EntityItem* item){
    //qCritical() << "CHILD CHANGED INDEX";
    RecalculateCells();
}

StackNodeItem::PersistentCellInfo& StackNodeItem::SetupCellInfo(int row, int col){
    CellIndex index(row, col);

    if(cell_info.contains(index)){
        return cell_info[index];
    }else{
        PersistentCellInfo& info = cell_info[index];
        info.index = index;
        info.orientation = orientation;
        info.margin = getDefaultCellMargin();
        info.spacing = getDefaultCellSpacing();

        return info;
    }
}



QMarginsF StackNodeItem::getDefaultCellMargin() const{
    return QMarginsF(getGridSize(), getGridSize(), getGridSize(), getGridSize());
}

QMarginsF StackNodeItem::getCellMargin(const CellIndex& index) const{
    auto grid_size = getGridSize();
    if(cell_info.contains(index)){
        return cell_info[index].margin;
    }
    return getDefaultCellMargin();
}

Qt::Orientation StackNodeItem::getCellOrientation(const CellIndex& index) const{
    if(cell_info.contains(index)){
        return cell_info[index].orientation;
    }
    return orientation;
}

qreal StackNodeItem::getDefaultCellSpacing() const{
    return getGridSize();
}

qreal StackNodeItem::getCellSpacing(const CellIndex& index) const{
    auto grid_size = getGridSize();

    if(cell_info.contains(index)){
        return cell_info[index].spacing;
    }
    return getDefaultCellSpacing();
}

void StackNodeItem::SetRenderCellText(int row, int col, bool render, QString label, QColor color){
    auto& cell_info = SetupCellInfo(row, col);

    cell_info.render_text = render && label.length();
    
    if(cell_info.render_text){
        if(!cell_info.text_item){
            cell_info.text_item = new StaticTextItem(Qt::AlignHCenter | Qt::AlignTop);
        }
    }else{
        if(cell_info.text_item){
            delete cell_info.text_item;
            cell_info.text_item = 0;
        }
    }
    cell_info.text_item->setText(label);
    cell_info.text_color = color;
}

void StackNodeItem::SetRenderCellArea(int row, int col, bool render, QColor color){
    auto& cell_info = SetupCellInfo(row, col);

    cell_info.render_rect = render;

    if(cell_info.render_rect){
        cell_info.rect_color = color;
    }
}

void StackNodeItem::SetCellOrientation(int row, int col, Qt::Orientation orientation){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.orientation = orientation;
}
void StackNodeItem::SetCellSpacing(int row, int col, int spacing){
    auto& cell_info = SetupCellInfo(row, col);

    cell_info.spacing = spacing;
}


void StackNodeItem::SetRenderCellIcons(int row, int col, bool render, QString icon_path, QString icon_name, QSize icon_size){
    auto& cell_info = SetupCellInfo(row, col);

    cell_info.render_icons = render;
    cell_info.icon = qMakePair(icon_path, icon_name);
    cell_info.icon_size = icon_size;
}

QPointF StackNodeItem::getStemAnchorPoint() const
{
    //QPointF offset;
    return gridRect().topLeft();
}

int StackNodeItem::GetHorizontalGap() const{
    return 2 * getGridSize();
}
int StackNodeItem::GetVerticalGap() const{
    return getGridSize();
}

QRectF StackNodeItem::getRowRect(int row) const{

    QRectF rect;

    for(auto& cell : cells){
        if(cell.index.first == row){
            rect |= cell.bounding_rect;
        }
    }
    return rect;
}

void StackNodeItem::updateCells(){
    if(sub_areas_dirty){
        cells.clear();

        //Get Our Children
        bool got_prev_index = false;
        CellIndex prev_index;

        QMap<int, CellIndex> last_row_cell;
        QMultiMap<int, CellIndex> row_cols;
        
        for(auto child : getSortedChildNodes()){
            auto index = GetCellIndex(child);
            
            bool inserted = !cells.contains(index);
            auto& cell = cells[index];
            auto& cell_br = cell.bounding_rect;
            auto& cell_cr = cell.child_rect;
            

            if(inserted){
                //Insert this into the row lookup
                row_cols.insert(index.first, index);
                
                cell.index = index;
                cell.orientation = orientation;


                QPointF cell_br_top_left;
                
                if(got_prev_index){
                    //If we are in a different cell, get this cells position from the previous
                    if(prev_index != index){
                        auto& prev_cell = cells[prev_index];
                        auto& prev_cell_br = prev_cell.bounding_rect;

                        auto prev_cell_margin = getCellMargin(prev_index);

                        //Adjust the Previous Cells Bounding Rect to add some padding between cells
                        prev_cell_br = prev_cell.child_rect.marginsAdded(prev_cell_margin);

                        //If our rows are the same
                        if(index.first == prev_index.first){
                            if(orientation == Qt::Vertical){
                                //Vertical stacked items means columns stack on the bottom
                                cell_br_top_left = QPointF(prev_cell_br.left(), prev_cell_br.bottom());
                            }else if(orientation == Qt::Horizontal){
                                //Vertical stacked items means columns stack on the Right
                                cell_br_top_left = QPointF(prev_cell_br.right(), prev_cell_br.top());
                            }
                        }else{
                            //Inser the last index as the last cell in the row
                            last_row_cell.insert(prev_index.first, prev_index);
                            //If our rows are different, use the previous row to calculate our offset
                            auto prev_row_rect = getRowRect(prev_index.first);

                            if(orientation == Qt::Vertical){
                                cell_br_top_left = QPointF(prev_row_rect.right(), prev_row_rect.top());
                            }else if(orientation == Qt::Horizontal){
                                cell_br_top_left = QPointF(prev_row_rect.left(), prev_row_rect.bottom());
                            }
                        }
                    }
                }else{
                    //If we don't have a previous rect, use the top left of the grid Rect
                    cell_br_top_left = GetGridAlignedTopLeft();
                }

                auto cell_margin = getCellMargin(index);
                cell.orientation = getCellOrientation(index);

                auto cell_cr_top_left = cell_br_top_left + QPointF(cell_margin.left(), cell_margin.top());
                cell_cr = QRectF(cell_cr_top_left, cell_cr_top_left);
            }

            auto cell_spacing = getCellSpacing(index);

            cell_spacing = inserted ? 0 : cell_spacing;
            
            auto child_rect = child->currentRect();

            QPointF child_pos;

            if(cell.orientation == Qt::Vertical){
                child_pos = QPointF(cell_cr.left(), cell_cr.bottom());
                child_pos.ry() += cell_spacing;
            }else if(cell.orientation == Qt::Horizontal){
                child_pos = QPointF(cell_cr.right(), cell_cr.top());
                child_pos.rx() += cell_spacing;
            }
            
            child_rect.moveTopLeft(child_pos);

            cell_cr |= child_rect;



            
            cell.children.append(child);
            cell.child_offsets[child] = child_pos;
            child->setPos(child_pos);
            prev_index = index;
            got_prev_index = true;
        }
        
        //Set the last index'd rectangle
        if(got_prev_index){
            last_row_cell.insert(prev_index.first, prev_index);
            auto& cell = cells[prev_index];
            auto& cell_br = cell.bounding_rect;
            auto& cell_cr = cell.child_rect;
            //Adjust the Previous Cells Bounding Rect to add some padding between cells
            cell_br = cell_cr.marginsAdded(getCellMargin(prev_index));
        }

        //Keep Track of the overall rect
        QRectF overall_rect;

        //Make each column in each row match height/width depending on orientation
        for(auto row : row_cols.keys()){
            QRectF prev_header_rect;
            for(const auto& index : row_cols.values(row)){
                auto& cell = cells[index];
                auto& child_rect = cell.child_rect;

                auto row_rect = getRowRect(index.first);
                auto margin = getCellMargin(index);

                //Calculate what the cells bounding rect would be, so we can 
                auto cell_br = cell.child_rect.marginsAdded(margin);

                if(orientation == Qt::Vertical){
                    auto adjustment = row_rect.width() - cell_br.width();
                    child_rect.setWidth(child_rect.width() + adjustment);
                }else if(orientation == Qt::Horizontal){
                    auto adjustment = row_rect.height() - cell_br.height();
                    child_rect.setHeight(child_rect.height() + adjustment);
                }

                cell.bounding_rect = child_rect.marginsAdded(margin);

                overall_rect |= cell.bounding_rect;

                //Calculate the gap rectangles
                QRectF prev_header_rect;
                for(auto child : cell.children){
                    auto header_rect = child->headerRect();
                    auto child_pos = cell.child_offsets[child];
                    header_rect.moveTopLeft(child_pos);
                    //header_rect.setTopRight(child_pos - header_rect.topRight());
                    
                    if(prev_header_rect.isValid()){
                        QRectF gap_rect;
                        if(cell.orientation == Qt::Horizontal){
                            gap_rect.setTopLeft(prev_header_rect.topRight());
                            gap_rect.setBottomRight(header_rect.bottomLeft());
                        }else if(cell.orientation == Qt::Vertical){
                            gap_rect.setTopLeft(prev_header_rect.bottomLeft());
                            gap_rect.setBottomRight(header_rect.topRight());
                        }
                        gap_rect = gap_rect.normalized();
                        if(gap_rect.isValid()){
                            cell.child_gap_rects.append(gap_rect);
                        }
                    }
                    prev_header_rect = header_rect;
                }
            }
        }

        //Make the last column in each row stretch
        for(auto row : last_row_cell.keys()){
            auto& index = last_row_cell[row];
            auto& cell = cells[index];
            
            auto& child_rect = cell.child_rect;
            auto margin = getCellMargin(index);
            
            //Adjust the Previous Cells Bounding Rect to add some padding between cells
            auto cell_br = cell.child_rect.marginsAdded(margin);

            if(orientation == Qt::Vertical){
                auto adjustment = overall_rect.bottom() - cell_br.bottom();
                child_rect.setHeight(child_rect.height() + adjustment);
            }else if(orientation == Qt::Horizontal){
                auto adjustment = overall_rect.right() - cell_br.right();
                child_rect.setWidth(child_rect.width() + adjustment);
            }

            cell.bounding_rect = child_rect.marginsAdded(margin);
        }


        NodeItem::childPosChanged(0);
        sub_areas_dirty = false;
    }
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

    return item_offset - QPointF(getGridSize(), getGridSize());
}

CellIndex StackNodeItem::GetCellIndex(NodeItem* child){
    return CellIndex(child->getSortOrderRow(), child->getSortOrderRowSubgroup());
}

QPointF StackNodeItem::getElementPosition(BasicNodeItem *child)
{
    auto index = GetCellIndex(child);
    return cells[index].child_offsets[child];
}



void StackNodeItem::childPosChanged(EntityItem* child)
{
    return;
}


void StackNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    BasicNodeItem::paintBackground(painter, option, widget);

    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);
    painter->setClipRect(option->exposedRect);

    

    if(state >= RENDER_STATE::BLOCK && isExpanded()){
        painter->setPen(Qt::NoPen);
        
        for(auto& p_cell : cell_info.values()){
            const auto& index = p_cell.index;
            if(cells.contains(index)){
                const auto& cell = cells[index];
                if(p_cell.render_rect){
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(p_cell.rect_color);
                    painter->drawRect(cell.bounding_rect);
                }
                if(p_cell.render_text && p_cell.text_item){
                    auto text_rect = cell.bounding_rect;
                    text_rect.setHeight(p_cell.margin.top());
                    if(text_rect.height()){
                        painter->setPen(p_cell.text_color);
                        p_cell.text_item->RenderText(painter, getRenderState(lod), text_rect);
                    }
                }
                for(const auto& rect : cell.child_gap_rects){
                    QRectF icon_rect;
                    icon_rect.setSize(p_cell.icon_size);
                    icon_rect.moveCenter(rect.center());
                    paintPixmap(painter, lod, icon_rect, p_cell.icon.first, p_cell.icon.second);
                }
            }
        }
    }

    NodeItem::paint(painter, option, widget);
}