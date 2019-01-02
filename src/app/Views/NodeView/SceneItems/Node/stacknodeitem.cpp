#include "stacknodeitem.h"

#include <QDebug>
#include <QDateTime>
#include <cmath>
#include <iterator>

StackNodeItem::StackNodeItem(NodeViewItem *viewItem, NodeItem *parentItem, Qt::Orientation orientation):
    BasicNodeItem(viewItem, parentItem)
{
    setSortOrdered(true);
    addRequiredData("column_count");

    reloadRequiredData();
    this->orientation = orientation;

    disconnect(this, &NodeItem::childSizeChanged, this, &NodeItem::childPosChanged);
    disconnect(this, &NodeItem::childPositionChanged, this, &NodeItem::childPosChanged);

    connect(this, &NodeItem::childSizeChanged, this, &StackNodeItem::ChildSizeChanged);
    connect(this, &NodeItem::childIndexChanged, this, &StackNodeItem::ChildIndexChanged);
    connect(this, &NodeItem::childCountChanged, this, &StackNodeItem::ChildCountChanged);

    addHoverFunction(EntityRect::BODY, std::bind(&StackNodeItem::bodyHover, this, std::placeholders::_1, std::placeholders::_2));

    cell_spacing = getGridSize();
    RecalculateCells();
}

void StackNodeItem::dataChanged(const QString& key_name, const QVariant& data){
    if(isDataRequired(key_name)){
        if(key_name == "column_count"){
            SetColumnCount(data.toInt());
        }
    }

    BasicNodeItem::dataChanged(key_name, data);
}

void StackNodeItem::SetColumnCount(int column_count){
    column_count_ = column_count;
    RecalculateCells();
}

void StackNodeItem::SetUseColumnCount(int row, int col, bool use_column_count){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.use_column_count = use_column_count;
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

    if(cell_infos.contains(index)){
        return cell_infos[index];
    }else{
        PersistentCellInfo& info = cell_infos[index];
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

QMarginsF StackNodeItem::getCellMargin(const CellIndex& index){
    if(cell_infos.contains(index)){
        return cell_infos[index].margin;
    }
    return getDefaultCellMargin();
}


int StackNodeItem::getCellColumnCount(const CellIndex& index){
    if(cell_infos.contains(index) && cell_infos[index].use_column_count){
        return column_count_;
    }
    return -1;
}

Qt::Orientation StackNodeItem::getCellOrientation(const CellIndex& index){
    if(cell_infos.contains(index)){
        return cell_infos[index].orientation;
    }
    return orientation;
}

bool StackNodeItem::getCellRenderPrefixIcon(const CellIndex& index){
    if(!isReadOnly()){
        return cell_infos.contains(index) && cell_infos[index].render_prefix_icon;
    }
    return false;
}

bool StackNodeItem::getCellRenderSuffixIcon(const CellIndex& index){
    if(!isReadOnly()){
        return cell_infos.contains(index) && cell_infos[index].render_suffix_icon;
    }
    return false;
}
bool StackNodeItem::getCellRenderGapIcon(const CellIndex& index){
    if(!isReadOnly()){
        return cell_infos.contains(index) && cell_infos[index].render_gap_icons;
    }
    return false;
}

qreal StackNodeItem::getDefaultCellSpacing() const{
    return cell_spacing;
}

void StackNodeItem::setDefaultCellSpacing(qreal spacing){
    cell_spacing = spacing;
}

qreal StackNodeItem::getCellSpacing(const CellIndex& index){
    auto grid_size = getGridSize();

    if(cell_infos.contains(index)){
        return cell_infos[index].spacing;
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


void StackNodeItem::SetRenderCellGapIcons(int row, int col, bool render, QString icon_path, QString icon_name){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.render_gap_icons = render;
    cell_info.gap_icon = qMakePair(icon_path, icon_name);
}

void StackNodeItem::SetRenderCellHoverIcons(int row, int col, bool render, QString icon_path, QString icon_name){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.render_hover_icon = render;
    cell_info.hovered_icon = qMakePair(icon_path, icon_name);
}

void StackNodeItem::SetRenderCellPrefixIcon(int row, int col, bool render, QString icon_path, QString icon_name){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.render_prefix_icon = render;
    cell_info.prefix_icon = qMakePair(icon_path, icon_name);
}

void StackNodeItem::SetRenderCellSuffixIcon(int row, int col, bool render, QString icon_path, QString icon_name){
    auto& cell_info = SetupCellInfo(row, col);
    cell_info.render_suffix_icon = render;
    cell_info.suffix_icon = qMakePair(icon_path, icon_name);
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
        //Clear the cells 
        cells.clear();

        QHash<int, CellIndex> last_cell_in_row;
        QMultiMap<int, int> row_cols;

        //Put all of the children in their appropriate Row/Column.
        for(const auto& child : getSortedChildNodes()){
            auto index = GetCellIndex(child);
            bool inserted = !cells.contains(index);
            auto& cell = cells[index];
            cell.children.append(child);

            if(inserted){
                row_cols.insert(index.first, index.second);
                cell.index = index;
                cell.orientation = getCellOrientation(index);
            }
        }

        //Add the cells that we have persistent information for, most of these should be added already if we have children
        for(auto& cell_info : cell_infos.values()){
            auto& index = cell_info.index;
            bool inserted = !cells.contains(index);
            auto& cell = cells[index];
            if(inserted){
                row_cols.insert(index.first, index.second);
                cell.index = index;
                cell.orientation = getCellOrientation(index);
            }
        }
        //Keep Track of the overall rect
        QRectF overall_rect;
        
        bool got_prev_index = false;
        CellIndex prev_index;

        //Sort the rows
        auto rows = row_cols.uniqueKeys();
        std::sort(rows.begin(), rows.end());
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
                QRectF cell_row_cr = cell_cr;

                auto cell_spacing = getCellSpacing(index);
                const auto& column_count = getCellColumnCount(index);
                
                int current_cell_row = 0;
                int current_cell_row_count = 0;
                //Deal with the children within the cell
                for(int i = 0; i < child_count; i++, current_cell_row_count++){
                    auto child = cell.children[i];
                    auto child_rect = child->currentRect();
                    
                    auto cell_offset = cell_spacing;

                    if(i == 0 && current_cell_row_count == 0 && !getCellRenderPrefixIcon(index)){
                        cell_offset = 0;
                    }
                    
                    QPointF child_pos;
                    if(cell.orientation == Qt::Vertical){
                        child_pos = QPointF(cell_row_cr.left(), cell_row_cr.bottom());
                        child_pos.ry() += cell_offset;
                    }else if(cell.orientation == Qt::Horizontal){
                        child_pos = QPointF(cell_row_cr.right(), cell_row_cr.top());

                        if(current_cell_row_count == 0 && column_count > 0 && current_cell_row == 0){
                            child_pos.ry() += cell_offset;
                        }else{
                            child_pos.rx() += cell_offset;
                        }
                    }

                    child_rect.moveTopLeft(child_pos);
                    cell_row_cr |= child_rect;

                    if(column_count > 0 && current_cell_row_count + 1 == column_count){
                        QRectF prev_rect =  cell_row_cr;

                        if(cell.orientation == Qt::Vertical){
                            QPointF new_row_rect_pos;
                            new_row_rect_pos.setX(prev_rect.right() + cell_spacing);
                            new_row_rect_pos.setY(prev_rect.top() - cell_spacing);
                            
                            cell_row_cr = QRectF(new_row_rect_pos, new_row_rect_pos);
                        }else if(cell.orientation == Qt::Horizontal){
                            QPointF new_row_rect_pos;
                            new_row_rect_pos.setX(prev_rect.left() - cell_spacing);
                            new_row_rect_pos.setY(prev_rect.bottom() + cell_spacing);
                            
                            cell_row_cr = QRectF(new_row_rect_pos, new_row_rect_pos);
                        }

                        cell_cr |= prev_rect;
                        //Drop the row
                        current_cell_row_count = -1;
                        current_cell_row ++;
                    }
                    cell.child_offsets[child] = child_pos;
                    child->setPos(child_pos);
                }
                
                cell_cr |= cell_row_cr;

                if(child_count && getCellRenderPrefixIcon(index)){
                    //Add an extra space at the start if we need to render
                    if(cell.orientation == Qt::Vertical || column_count > 0){
                        cell_cr += QMarginsF(0, cell_spacing, 0, 0);
                    }else if(cell.orientation == Qt::Horizontal){
                        cell_cr += QMarginsF(cell_spacing, 0, 0, 0);
                    }
                }
                
                if(getCellRenderSuffixIcon(index)){
                    //Add an extra space at the end if we need to render
                    auto alt_offset = child_count ? 0 : cell_spacing;

                    if(cell.orientation == Qt::Vertical || column_count > 0){
                        cell_cr += QMarginsF(0, 0, alt_offset, cell_spacing);
                    }else if(cell.orientation == Qt::Horizontal){
                        cell_cr += QMarginsF(0, 0, cell_spacing, alt_offset);
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
                    last_cell_in_row.insert(row, index);
                }

                
                prev_index = index;
                got_prev_index = true;

                overall_rect |= cell_br;
            }
        }

        if(overall_rect.width() < getContractedWidth()){
            overall_rect.setWidth(getContractedWidth());
        }

        auto row_count = last_cell_in_row.count();
        
        //Make the last column in each row stretch
        for(auto row : last_cell_in_row.keys()){
            //Check for only one row
            auto& index = last_cell_in_row[row];
            auto& cell = cells[index];
            const auto& column_count = getCellColumnCount(index);

            
            auto& cell_br = cell.bounding_rect;

            

            if(orientation == Qt::Vertical){
                cell_br.setBottom(overall_rect.bottom());
                if(row_count == 1){
                    cell_br.setRight(overall_rect.right());    
                }
            }else if(orientation == Qt::Horizontal){
                cell_br.setRight(overall_rect.right());
            }
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
                const auto& column_count = getCellColumnCount(index);

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

                auto icon_size = smallIconSize();
                bool cell_gap_suitable = cell_info.spacing >= icon_size.width();

                auto& boundary_rect = cell_gap_suitable ? cell_cr : cell_br;

                //Calculate the gap rectangles
                QRectF prev_header_rect = cell_cr;
                
                //Anchor on the left or the top of the cell rect
                if(cell.orientation == Qt::Vertical || column_count != -1){
                    prev_header_rect.setTop(boundary_rect.top());
                    prev_header_rect.setBottom(boundary_rect.top());
                }else if(cell.orientation == Qt::Horizontal){
                    prev_header_rect.setLeft(boundary_rect.left());
                    prev_header_rect.setRight(boundary_rect.left());
                }

                if(cell_info.render_icons()){
                    auto cell_count = cell.children.count();
                    const auto& column_count = getCellColumnCount(index);
                    int current_cell_row = 0;
                    int current_cell_row_count = 0;

                    //Calculate The gap icons to the left of the item
                    for(int i = 0; i < cell_count ; i++, current_cell_row_count++){
                        auto child = cell.children[i];
                        auto header_rect = child->headerRect();
                        header_rect.moveTopLeft(cell.child_offsets[child]);

                        auto render_icon_gap = getCellRenderGapIcon(index);

                        if(current_cell_row_count == 0){
                            if(i == 0){
                                render_icon_gap = getCellRenderPrefixIcon(index);
                            }
                            
                            if(cell.orientation == Qt::Vertical || column_count != -1){
                                prev_header_rect.setLeft(cell.child_rect.left());
                                prev_header_rect.setRight(cell.child_rect.right());

                                header_rect = cell.child_rect;
                                header_rect.setTop(cell.child_offsets[child].y());
                            }else if(cell.orientation == Qt::Horizontal){
                                prev_header_rect.setTop(header_rect.top());
                                prev_header_rect.setBottom(header_rect.bottom());
                            }

                            if(i == 0 && render_icon_gap){
                                cell.prefix_gap_index = 0;
                            }
                        }
                        
                        if(render_icon_gap){
                            CellIconRect cell_icon_rect;
                            cell_icon_rect.index = child->getSortOrder();
                            
                            auto& gap_rect = cell_icon_rect.gap_rect;
                            gap_rect = GetGapIconRect(index, i, prev_header_rect, header_rect);

                            if(gap_rect.isValid()){
                                auto& icon_rect = cell_icon_rect.icon_rect;
                                icon_rect.setSize(smallIconSize());
                                icon_rect.moveCenter(gap_rect.center());
                                cell.child_gap_rects.append(cell_icon_rect);
                            }
                        }

                        if(column_count > 0 && current_cell_row_count + 1 == column_count){
                            current_cell_row_count = -1;
                            current_cell_row ++;
                        }

                        if(current_cell_row_count <= 0){
                            header_rect = child->headerRect();
                            header_rect.moveTopLeft(cell.child_offsets[child]);
                        }
                        prev_header_rect = header_rect;
                    }

                    if(getCellRenderSuffixIcon(index)){
                        CellIconRect cell_icon_rect;
                        cell_icon_rect.index = -1;
                        QRectF header_rect = prev_header_rect;
                        

                        //Lock it to the right
                        if(cell.orientation == Qt::Vertical || column_count > 0){
                            auto bottom_offset_multiplier = cell_count == 0 ? 3 : 2;
                            prev_header_rect.setBottom(boundary_rect.bottom() - (bottom_offset_multiplier * getCellSpacing(cell.index)));
                            prev_header_rect.setLeft(boundary_rect.left());

                            header_rect.setTop(boundary_rect.bottom());
                            header_rect.setBottom(boundary_rect.bottom());
                            header_rect.setRight(boundary_rect.right());
                            
                        }else if(cell.orientation == Qt::Horizontal){
                            header_rect.setRight(boundary_rect.right());
                            header_rect.setLeft(boundary_rect.right());
                        }
                        
                        auto& gap_rect = cell_icon_rect.gap_rect;
                        gap_rect = GetGapIconRect(index, cell_count, prev_header_rect, header_rect);

                        if(gap_rect.isValid()){
                            
                            auto& icon_rect = cell_icon_rect.icon_rect;
                            icon_rect.setSize(icon_size);
                            icon_rect.moveCenter(gap_rect.center());
                            cell.suffix_gap_index = cell.child_gap_rects.size();
                            cell.child_gap_rects.append(cell_icon_rect);
                        }
                    }
                }
            }
        }
        

        sub_areas_dirty = false;
        if(cell_rect != overall_rect){
            cell_rect = overall_rect;
            prepareGeometryChange();
            emit sizeChanged();
        }
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
        
        for(auto& p_cell : cell_infos.values()){
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

                if(p_cell.render_text && p_cell.text_item){
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

                bool render_prefix = getCellRenderPrefixIcon(index);
                bool render_gap = getCellRenderGapIcon(index);
                bool render_suffix = getCellRenderSuffixIcon(index);

                if(render_prefix || render_gap || render_suffix){
                    auto cell_icon_count = cell.child_gap_rects.count();
                    for(int i = 0; i < cell_icon_count; i ++){
                        auto is_prefix = cell.prefix_gap_index == i;
                        auto is_suffix = cell.suffix_gap_index == i;

                        //Skip the gaps we don't need to render
                        if(is_suffix){
                            if(!render_suffix){
                                continue;
                            }
                        }else if(is_prefix){
                            if(!render_prefix){
                                continue;
                            }
                        }else if(!render_gap){
                            continue;
                        }

                        const auto& cell_icon_rect = cell.child_gap_rects[i];
                        const auto& icon_rect = cell_icon_rect.icon_rect;

                        auto& icon = is_prefix ? p_cell.prefix_icon : is_suffix ? p_cell.suffix_icon : p_cell.gap_icon;

                        auto is_hovered = cell_icon_rect.hovered;
                        const auto& icon_path = is_hovered && p_cell.render_hover_icon ? p_cell.hovered_icon : icon;
                        const auto icon_color = is_hovered ? getHighlightTextColor() : getTextColor();
                        const auto background_color = p_cell.use_alt_color ? getBodyColor() : getAltBodyColor();

                        painter->setBrush(is_hovered ? getHighlightColor() : background_color);
                        painter->setPen(is_hovered ? getPen() : getDefaultPen());
                        painter->drawEllipse(icon_rect);

                        paintPixmap(painter, lod, icon_rect, icon_path.first, icon_path.second, icon_color);
                    }
                }
            }
        }
    }

    NodeItem::paint(painter, option, widget);
}

void StackNodeItem::bodyHover(bool hovered, const QPointF& pos){
    bool should_update = false;
    
    for(auto& p_cell : cell_infos.values()){
        if(p_cell.render_icons()){
            const auto& index = p_cell.index;
            if(cells.contains(index)){
                auto& cell = cells[index];
                
                for (auto& cell_icon_rect : cell.child_gap_rects){
                    auto is_hovered = hovered && cell_icon_rect.icon_rect.contains(pos);
                    if(cell_icon_rect.hovered != is_hovered){
                        cell_icon_rect.hovered = is_hovered;
                        should_update = true;

                    }
                    if(is_hovered){
                        AddTooltip("Click to show Add Menu");
                    }
                }
            }
        }
    }


    if(should_update){
        update();
    }
}


void StackNodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event){
    NodeItem::mousePressEvent(event);
}

void StackNodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    
    for(auto& p_cell : cell_infos.values()){
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

QRectF StackNodeItem::GetGapIconRect(const CellIndex& index, int child_index, const QRectF& prev_rect, const QRectF& current_rect){
    QRectF gap_rect;
    
    const auto& orientation = getCellOrientation(index);
    const auto& column_count = getCellColumnCount(index);
    auto sibling_count = cells.contains(index) ? cells[index].children.size() : 0;

    bool use_vertical_layout = false;
    if(column_count != -1 && (child_index % column_count == 0 || child_index == sibling_count)){
        use_vertical_layout = true;
    }
                            
    if(orientation == Qt::Vertical || use_vertical_layout){
        gap_rect.setTopLeft(prev_rect.bottomLeft());
        gap_rect.setBottomRight(current_rect.topRight());
    }else if(orientation == Qt::Horizontal){
        gap_rect.setTopLeft(prev_rect.topRight());
        gap_rect.setBottomRight(current_rect.bottomLeft());
    }

    gap_rect = gap_rect.normalized();

    return gap_rect;
}