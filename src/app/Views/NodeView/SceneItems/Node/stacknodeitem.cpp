#include "stacknodeitem.h"

#include <QDebug>
#include <QDateTime>
#include <cmath>
#include <iterator>

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

    cell_spacing = getGridSize();
    RecalculateCells();
}

void StackNodeItem::setAlignment(Qt::Orientation orientation){
    this->orientation = orientation;
    RecalculateCells();
}

void StackNodeItem::RecalculateCells(){
    sub_areas_dirty = true;
    updateCells();
}

void StackNodeItem::ChildCountChanged(){
    RecalculateCells();
}

void StackNodeItem::ChildSizeChanged(EntityItem* item){
    RecalculateCells();
}

void StackNodeItem::ChildIndexChanged(EntityItem* item){
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
    return QMarginsF(getDefaultCellSpacing(), getDefaultCellSpacing(), getDefaultCellSpacing(), getDefaultCellSpacing());
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
    return cell_spacing;
}

void StackNodeItem::setDefaultCellSpacing(qreal spacing){
    cell_spacing = spacing;
}

qreal StackNodeItem::getCellSpacing(const CellIndex& index) const{
    auto grid_size = getGridSize();

    if(cell_info.contains(index)){
        return cell_info[index].spacing;
    }
    return getDefaultCellSpacing();
}

void StackNodeItem::SetRenderCellText(int row, int col, bool render, QString label){
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
}

void StackNodeItem::SetRenderCellArea(int row, int col, bool render, bool use_alt_color){
    auto& cell_info = SetupCellInfo(row, col);

    cell_info.render_rect = render;

    if(cell_info.render_rect){
        cell_info.use_alt_color = use_alt_color;
    }
}

QRectF StackNodeItem::childrenRect() const
{
    return cell_rect;
}

void StackNodeItem::SetCellOrientation(int row, int col, Qt::Orientation orientation){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.orientation = orientation;
}
void StackNodeItem::SetCellSpacing(int row, int col, int spacing){
    auto& cell_info = SetupCellInfo(row, col);

    cell_info.spacing = spacing;
}

void StackNodeItem::SetCellMargins(int row, int col, QMarginsF margins){
    auto& cell_info = SetupCellInfo(row, col);

    cell_info.margin = margins;
}

void StackNodeItem::SetCellMinimumSize(int row, int col, qreal width, qreal height){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.minimum_width = width;
    cell_info.minimum_height = height;
}


void StackNodeItem::SetRenderCellIcons(int row, int col, bool render, QString icon_path, QString icon_name){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.render_all_icons = render;
    cell_info.icon = qMakePair(icon_path, icon_name);
}

void StackNodeItem::SetRenderCellHoverIcons(int row, int col, QString icon_path, QString icon_name){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.hovered_icon = qMakePair(icon_path, icon_name);
}

void StackNodeItem::SetRenderCellFirstIcon(int row, int col, QString icon_path, QString icon_name){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.render_first_icon = true;
    cell_info.first_icon = qMakePair(icon_path, icon_name);
}

void StackNodeItem::SetRenderCellLastIcon(int row, int col, QString icon_path, QString icon_name){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.render_last_icon = true;
    cell_info.last_icon = qMakePair(icon_path, icon_name);
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

        

        QMap<int, CellIndex> last_row_cell;
        QMultiMap<int, int> row_cols;

        //Put all of the children in their appropriate homes.
        for(auto child : getSortedChildNodes()){
            auto index = GetCellIndex(child);
            bool inserted = !cells.contains(index);
            auto& cell = cells[index];
            cell.children.append(child);

            if(inserted){
                //Insert this into the row lookup
                row_cols.insert(index.first, index.second);
                cell.index = index;
                cell.orientation = getCellOrientation(index);
            }
        }

        //Add in the cell's which we have info for.
        for(auto& cell_info : cell_info.values()){
            auto& index = cell_info.index;
            bool inserted = !cells.contains(index);
            auto& cell = cells[index];
            if(inserted){
                //Insert this into the row lookup
                row_cols.insert(index.first, index.second);
                cell.index = index;
                cell.orientation = getCellOrientation(index);
            }
        }

        const auto& origin = GetGridAlignedTopLeft();
        //Get Our Children
        bool got_prev_index = false;
        CellIndex prev_index;

        auto rows = row_cols.uniqueKeys();
        std::sort(rows.begin(), rows.end());

        //Keep Track of the overall rect
        QRectF overall_rect;


        for(auto row : rows){
            auto columns = row_cols.values(row);
            std::sort(columns.begin(), columns.end());
            for(auto col : columns){
                const auto& index = CellIndex({row, col});
                auto& cell = cells[index];
                auto& cell_br = cell.bounding_rect;
                auto& cell_cr = cell.child_rect;
                auto& cell_info = SetupCellInfo(index.first, index.second);
                

                QPointF cell_br_top_left;

                //Setup our top left corner
                if(got_prev_index){
                    auto& prev_cell = cells[prev_index];
                    const auto& prev_cell_br = prev_cell.bounding_rect;
                    
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
                        //If our rows are different, use the previous row to calculate our offset
                        auto prev_row_rect = getRowRect(prev_index.first);

                        if(orientation == Qt::Vertical){
                            cell_br_top_left = QPointF(prev_row_rect.right(), prev_row_rect.top());
                        }else if(orientation == Qt::Horizontal){
                            cell_br_top_left = QPointF(prev_row_rect.left(), prev_row_rect.bottom());
                        }
                    }
                }else{
                    //If we don't have a previous rect, use the top left of the grid Rect
                    cell_br_top_left = GetGridAlignedTopLeft();
                }

                auto child_count = cell.children.count();
                auto cell_margin = getCellMargin(index);
                
                cell.allocated = child_count > 0 || cell_info.render_always();

                auto cell_cr_top_left = cell_br_top_left;

                if(cell.allocated){
                    cell_cr_top_left += QPointF(cell_margin.left(), cell_margin.top());
                }
                
                cell_cr = QRectF(cell_cr_top_left, cell_cr_top_left);

                auto cell_spacing = getCellSpacing(index);
                
                //Deal with the children within the cell
                for(int i = 0; i < child_count; i++){
                    auto child = cell.children[i];
                    auto child_rect = child->currentRect();
                    
                    auto cell_offset = cell_spacing;

                    if(i == 0){
                        if(!cell_info.render_first_icon){
                            cell_offset = 0;
                        }
                    }

                    QPointF child_pos;

                    if(cell.orientation == Qt::Vertical){
                        child_pos = QPointF(cell_cr.left(), cell_cr.bottom());
                        child_pos.ry() += cell_offset;
                    }else if(cell.orientation == Qt::Horizontal){
                        child_pos = QPointF(cell_cr.right(), cell_cr.top());
                        child_pos.rx() += cell_offset;
                    }
                    
                    child_rect.moveTopLeft(child_pos);
                    cell_cr |= child_rect;
                    cell.child_offsets[child] = child_pos;
                    child->setPos(child_pos);
                }

                if(cell_info.render_last_icon){
                    //Add an extra space at the end if we need to render
                    if(cell.orientation == Qt::Vertical){
                        cell_cr += QMarginsF(0, 0, 0, cell_spacing);
                    }else if(cell.orientation == Qt::Horizontal){
                        cell_cr += QMarginsF(0, 0, cell_spacing, 0);
                    }
                }

                if(cell_cr.width() < cell_info.minimum_width){
                    cell_cr.setWidth(cell_info.minimum_width);
                }

                if(cell_cr.height() < cell_info.minimum_height){
                    cell_cr.setHeight(cell_info.minimum_height);
                }

                
                cell_br = cell_cr;
                
                //Should we use this space?
                if(cell.allocated){
                    cell_br += cell_margin;
                    last_row_cell.insert(row, index);
                }

                
                prev_index = index;
                got_prev_index = true;

                overall_rect |= cell_br;
            }
        }

        if(overall_rect.width() < getMinimumWidth()){
            overall_rect.setWidth(getMinimumWidth());
        }

        

        auto row_count = last_row_cell.count();
        qCritical() << "ROW_COUNT" << row_count;
        //Make the last column in each row stretch
        for(auto row : last_row_cell.keys()){
            //Check for only one row
            auto& index = last_row_cell[row];
            auto& cell = cells[index];

            
            auto& cell_br = cell.bounding_rect;

            

            if(orientation == Qt::Vertical){
                cell_br.setBottom(overall_rect.bottom());
                if(row_count == 1){
                    cell_br.setRight(overall_rect.right());    
                }
            }else if(orientation == Qt::Horizontal){
                cell_br.setRight(overall_rect.right());
            }

            qCritical() << "ROW: " << row << cell_br;
        }
        
        

        for(auto row : rows){
            auto columns = row_cols.values(row);
            std::sort(columns.begin(), columns.end());
            for(auto col : columns){
                const auto& index = CellIndex({row, col});

                auto& cell = cells[index];
                if(!cell.allocated){
                    continue;
                }
                
                auto& cell_info = SetupCellInfo(index.first, index.second);
                auto row_rect = getRowRect(index.first);
                auto margin = getCellMargin(index);

                auto& cell_br = cell.bounding_rect;
                auto& cell_cr = cell.child_rect;

                if(orientation == Qt::Vertical){
                    auto adjustment = row_rect.width() - cell_br.width();
                    cell_br.setWidth(cell_br.width() + adjustment);
                }else if(orientation == Qt::Horizontal){
                    auto adjustment = row_rect.height() - cell_br.height();
                    cell_br.setHeight(cell_br.height() + adjustment);
                }
                //Re-Adjust
                cell_cr = cell_br - cell_info.margin;

                overall_rect |= cell_br;

                cell.child_gap_rects.clear();

                //Calculate the gap rectangles
                QRectF prev_header_rect;

                if(cell.orientation == Qt::Horizontal){
                    prev_header_rect.setLeft(cell_cr.left());
                    prev_header_rect.setRight(cell_cr.left());
                }else if(cell.orientation == Qt::Vertical){
                    prev_header_rect.setTop(cell_cr.top());
                    prev_header_rect.setBottom(cell_cr.top());
                }

                

                if(cell_info.render_icons()){
                    auto cell_count = cell.children.count();
                    //Calculate The gap icons for inbetween items 
                    for(int i = 0; i < cell_count ; i++){
                        auto child = cell.children[i];
                        auto is_first = i == 0;
                        
                        auto header_rect = child->headerRect();
                        auto child_pos = cell.child_offsets[child];
                        header_rect.moveTopLeft(child_pos);

                        if(is_first){
                            if(cell.orientation == Qt::Horizontal){
                                prev_header_rect.setTop(header_rect.top());
                                prev_header_rect.setBottom(header_rect.bottom());
                            }else if(cell.orientation == Qt::Vertical){
                                prev_header_rect.setLeft(header_rect.left());
                                prev_header_rect.setRight(header_rect.right());
                            }
                        }
                        
                        auto should_render = cell_info.render_all_icons;
                        if(should_render){
                            CellIconRect cell_icon_rect;
                            cell_icon_rect.index = child->getSortOrder();
                            auto& gap_rect = cell_icon_rect.gap_rect;
                            
                            if(cell.orientation == Qt::Horizontal){
                                gap_rect.setTopLeft(prev_header_rect.topRight());
                                gap_rect.setBottomRight(header_rect.bottomLeft());
                            }else if(cell.orientation == Qt::Vertical){
                                gap_rect.setTopLeft(prev_header_rect.bottomLeft());
                                gap_rect.setBottomRight(header_rect.topRight());
                            }
                            gap_rect = gap_rect.normalized();

                            if(gap_rect.isValid()){
                                auto& icon_rect = cell_icon_rect.icon_rect;
                                icon_rect.setSize(smallIconSize());
                                icon_rect.moveCenter(gap_rect.center());

                                cell.child_gap_rects.append(cell_icon_rect);
                            }
                        }
                    }
                    auto should_render = cell_info.render_all_icons || cell_info.render_last_icon;
                    if(should_render){
                        CellIconRect final_icon_rect;
                        final_icon_rect.index = -1;
                        auto& gap_rect = final_icon_rect.gap_rect;


                        if(cell_count > 0){
                            if(cell.orientation == Qt::Horizontal){
                                gap_rect.setLeft(prev_header_rect.right());
                                gap_rect.setRight(cell.bounding_rect.right());
                                gap_rect.setTop(prev_header_rect.top());
                                gap_rect.setBottom(prev_header_rect.bottom());
                            }else if(cell.orientation == Qt::Vertical){
                                gap_rect.setLeft(prev_header_rect.left());
                                gap_rect.setRight(prev_header_rect.right());
                                gap_rect.setTop(prev_header_rect.bottom());
                                gap_rect.setBottom(cell.bounding_rect.bottom());
                            }
                        }else{
                            gap_rect = cell.bounding_rect - cell_info.margin;
                            qCritical() << "FINAL GAP RECT: " << gap_rect;
                        }

                        gap_rect = gap_rect.normalized();

                        if(gap_rect.isValid()){
                            auto& icon_rect = final_icon_rect.icon_rect;
                            icon_rect.setSize(smallIconSize());
                            icon_rect.moveCenter(gap_rect.center());

                            cell.child_gap_rects.append(final_icon_rect);
                        }
                    }
                }
            }
        }
        

        sub_areas_dirty = false;
        cell_rect = overall_rect;
        resizeToChildren();
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
                    
                    if(p_cell.use_alt_color){
                        painter->setBrush(getAltBodyColor());
                        painter->setPen(painter->brush().color());
                    }else{
                        painter->setBrush(Qt::NoBrush);
                        painter->setPen(Qt::NoPen);
                    }
                    
                    painter->drawRect(cell.bounding_rect);
                }
                if(p_cell.render_text && p_cell.text_item){//} && cell.children.count()){
                    auto text_rect = cell.bounding_rect;
                    text_rect.setHeight(p_cell.margin.top());
                    if(text_rect.height()){
                        if(p_cell.use_alt_color){
                            painter->setPen(getAltTextColor());
                        }else{
                            painter->setPen(getTextColor());    
                        }
                        
                        p_cell.text_item->RenderText(painter, getRenderState(lod), text_rect);
                    }
                }

                if(p_cell.render_icons()){
                    auto cell_icon_count = cell.child_gap_rects.count();

                    for(int i = 0; i < cell_icon_count; i ++){
                        const auto& cell_icon_rect = cell.child_gap_rects[i];
                        auto is_last = (i + 1) == cell_icon_count;
                        auto is_first = !is_last && i == 0;
                        auto& icon_rect = cell_icon_rect.icon_rect;
                        auto& is_hovered = cell_icon_rect.hovered;
                        auto& icon = is_first ? p_cell.first_icon : is_last ? p_cell.last_icon : p_cell.icon;
                        auto& icon_path = is_hovered ? p_cell.hovered_icon : icon;
                        
                        auto icon_color = is_hovered ? getHighlightTextColor() : getTextColor();
                        painter->setBrush(is_hovered ? getHighlightColor() :  Qt::transparent);
                        painter->setPen(is_hovered ? getPen() : getDefaultPen());
                        painter->drawEllipse(icon_rect);

                        paintPixmap(painter, lod, icon_rect, icon_path.first, icon_path.second, icon_color);
                        if(is_first){
                            is_first = false;
                        }
                    }
                }
            }
        }
    }

    NodeItem::paint(painter, option, widget);
}

void StackNodeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event){
    bool need_update = false;
    const auto& mouse_pos = event->pos();
    for(auto& p_cell : cell_info.values()){
        if(p_cell.render_icons()){
            const auto& index = p_cell.index;
            if(cells.contains(index)){
                auto& cell = cells[index];
                
                for (auto& cell_icon_rect : cell.child_gap_rects){
                    auto is_hovered = cell_icon_rect.icon_rect.contains(mouse_pos);
                    if(cell_icon_rect.hovered != is_hovered){
                        cell_icon_rect.hovered = is_hovered;
                        need_update = true;
                    }
                }
            }
        }
    }

    if(need_update){
        update();
    }
    NodeItem::hoverMoveEvent(event);
}

void StackNodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    NodeItem::mousePressEvent(event);
}

void StackNodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    
    for(auto& p_cell : cell_info.values()){
        const auto& index = p_cell.index;
        if(cells.contains(index) && p_cell.render_icons()){
            auto& cell = cells[index];

            for (auto& cell_icon_rect : cell.child_gap_rects){
                if(cell_icon_rect.hovered){
                    auto is_contained = cell_icon_rect.icon_rect.contains(event->pos());
                    if(is_contained){
                        emit req_addNodeMenu(event->scenePos(), cell_icon_rect.index);
                    }
                }
            }
        }
    }
    NodeItem::mouseReleaseEvent(event);
}