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

bool StackNodeItem::allowIcons(){
    if(isReadOnly()){
        return false;
    }else{
        if(getNodeViewItem()->isInstance()){
            static const QSet<NODE_KIND> valid_kinds({NODE_KIND::FUNCTION, NODE_KIND::CALLBACK_FNC_INST});
            return valid_kinds.contains(getNodeKind());
        }else{
            return true;
        }
    }
}

bool StackNodeItem::getCellRenderPrefixIcon(const CellIndex& index){
    if(allowIcons()){
        return cell_infos.contains(index) && cell_infos[index].render_prefix_icon;
    }
    return false;
}

bool StackNodeItem::getCellRenderSuffixIcon(const CellIndex& index){
    if(allowIcons()){
        return cell_infos.contains(index) && cell_infos[index].render_suffix_icon;
    }
    return false;
}

bool StackNodeItem::getCellRenderGapIcon(const CellIndex& index){
    if(allowIcons()){
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
        for(const auto& row : rows){
            auto columns = row_cols.values(row);
            std::sort(columns.begin(), columns.end());
            for(const auto& col : columns){
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
                            cell_br_top_left = prev_cell_br.bottomLeft();
                        }else{
                            //Horizontal stacked items means columns stack on the Right
                            cell_br_top_left = prev_cell_br.topRight();
                        }
                    }else{
                        //If our rows are different, use the previous row to calculate our offset
                        auto prev_row_rect = getRowRect(prev_index.first);

                        if(orientation == Qt::Vertical){
                            cell_br_top_left = prev_row_rect.topRight();
                        }else{
                            cell_br_top_left = prev_row_rect.bottomLeft();
                        }
                    }
                }else{
                    //If we don't have a previous rect, use the top left of the grid Rect
                    cell_br_top_left = GetGridAlignedTopLeft();
                }

                const auto child_count = cell.children.count();
                const auto cell_margin = getCellMargin(index);
                const auto cell_spacing = getCellSpacing(index);
                const auto& column_count = getCellColumnCount(index);
                bool render_suffix = getCellRenderSuffixIcon(index);
                bool render_prefix = getCellRenderPrefixIcon(index);

                if(child_count == 0 && render_prefix && render_suffix){
                    render_prefix = false;
                }

                //Set whether the cell is being used
                cell.allocated = child_count > 0 || cell_info.render_always();

                auto cell_cr_top_left = cell_br_top_left;

                if(cell.allocated){
                    //If the cell is being used, add a gap from the last rect
                    cell_cr_top_left.rx() += cell_margin.left();
                    cell_cr_top_left.ry() += cell_margin.top();

                    if(render_prefix){
                         //Add an allowance to at the start if we need to render a prefix icon
                        if(cell.orientation == Qt::Vertical || column_count > 0){
                            cell_cr_top_left.ry() += cell_spacing;
                        }else if(cell.orientation == Qt::Horizontal){
                            cell_cr_top_left.rx() += cell_spacing;
                        }
                    }
                }

                cell_cr = QRectF(cell_cr_top_left, cell_cr_top_left);
                auto cell_row_cr = cell_cr;

                int current_cell_row = 0;
                int current_cell_row_count = 0;

                //Deal with the children within the cell
                for(int i = 0; i < child_count; i++, current_cell_row_count++){
                    auto child = cell.children[i];
                    auto child_rect = child->currentRect();
                    
                    QPointF child_pos;

                    if(cell.orientation == Qt::Vertical){
                        child_pos = cell_row_cr.bottomLeft();

                        if(current_cell_row_count != 0){
                            //Add a vertical gap between each child
                            child_pos.ry() += cell_spacing;
                        }

                        if(current_cell_row_count == 0 && i > 0){
                            //Add a horizontal gap between each row
                            child_pos.rx() += cell_spacing;
                        }
                    }else if(cell.orientation == Qt::Horizontal){
                        child_pos = cell_row_cr.topRight();

                        if(current_cell_row_count != 0){
                            //Add a horizontal gap between each child
                            child_pos.rx() += cell_spacing;
                        }

                        if(current_cell_row_count == 0 && i > 0){
                            //Add a vertical gap between each row
                            child_pos.ry() += cell_spacing;
                        }
                    }

                    child_rect.moveTopLeft(child_pos); 
                    //Update the current cell row
                    cell_row_cr |= child_rect;

                    //If this is the last element in the row
                    if(column_count > 0 && current_cell_row_count + 1 == column_count){
                        QRectF prev_cell_row_cr = cell_row_cr;

                        if(cell.orientation == Qt::Vertical){
                            cell_row_cr = QRectF(prev_cell_row_cr.topRight(), prev_cell_row_cr.topRight());
                        }else if(cell.orientation == Qt::Horizontal){
                            cell_row_cr = QRectF(prev_cell_row_cr.bottomLeft(), prev_cell_row_cr.bottomLeft());
                        }
                        
                        //Update the cell_cr with the previous cell row rect
                        cell_cr |= prev_cell_row_cr;
                        //current_cell_row_count is incremented in loop.
                        current_cell_row_count = -1;
                        current_cell_row ++;
                    }
                    cell.child_offsets[child] = child_pos;
                    child->setPos(child_pos);
                }

                //Update the cell
                cell_cr |= cell_row_cr;

                if(cell.allocated && render_prefix){
                    //Take into account the extra space used by the prefix icon, if we have one
                    if(cell.orientation == Qt::Vertical || column_count > 0){
                        cell_cr += QMarginsF(0, cell_spacing, 0, 0);
                    }else if(cell.orientation == Qt::Horizontal){
                        cell_cr += QMarginsF(cell_spacing, 0, 0, 0);
                    }
                }
                
                if(cell.allocated && render_suffix){
                    //Allocate the cell spacing even if we don't have any children (Allows the rectangle to be drawn)
                    auto alt_offset = child_count == 0 ? cell_spacing : 0;

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
        for(const auto& row : last_cell_in_row.keys()){
            //Check for only one row
            const auto& index = last_cell_in_row[row];
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
        }

        for(const auto& row : rows){
            auto columns = row_cols.values(row);
            std::sort(columns.begin(), columns.end());
            for(const auto& col : columns){
                const auto& index = CellIndex({row, col});

                auto& cell = cells[index];
                if(!cell.allocated){
                    //Ignore non used cells
                    continue;
                }

                auto& cell_info = SetupCellInfo(index.first, index.second);
                auto row_rect = getRowRect(index.first);
                const auto& column_count = getCellColumnCount(index);

                auto& cell_br = cell.bounding_rect;
                auto& cell_cr = cell.child_rect;

                //Update the cell's bounding Rect to stretch to fill the row_rect
                if(orientation == Qt::Vertical){
                    cell_br.setWidth(row_rect.width());
                }else if(orientation == Qt::Horizontal){
                    cell_br.setHeight(row_rect.height());
                }
                
                //Recalculate the cell's child rect to reflect changes in the bounding rect
                cell_cr = cell_br - cell_info.margin;

                //Update the overall rect
                overall_rect |= cell_br;

                //Reset the gap rects
                cell.child_gap_rects.clear();

                auto cell_gap_suitable = cell_info.spacing >= smallIconSize().width();
                //If the gap fits our icon, use the minimal
                const auto& boundary_rect = cell_gap_suitable ? cell_cr : cell_br;

                const auto& child_count = cell.children.size();
                auto gap_count = child_count;
                if(cell_info.render_prefix_icon || cell_info.render_suffix_icon){
                    gap_count ++;
                }
                cell.prefix_gap_index = cell_info.render_prefix_icon ? 0 : -1;
                cell.suffix_gap_index = cell_info.render_suffix_icon ? gap_count -1 : -1;
                
                QRectF last_rect = QRectF(boundary_rect.topLeft(), boundary_rect.topLeft());
                QRectF current_rect = boundary_rect;

                const auto use_vertical_layout = cell.orientation == Qt::Vertical || column_count > 0;
                
                for(int i = 0; i < gap_count; i++){
                    QRectF next_rect;
                    CellIconRect cell_icon_rect;
                    
                    bool is_last = i == child_count;
                    bool is_first = i == 0;
                    
                    if(is_last){
                        current_rect = QRectF(boundary_rect.bottomRight(), boundary_rect.bottomRight());
                        next_rect = current_rect;
                        if(!is_first && cell.orientation == Qt::Horizontal){
                            //In horizontal stretch land
                            current_rect.setBottomRight(last_rect.bottomRight());
                        }
                    }else{
                        const auto& child = cell.children.at(i);
                        auto header_rect = child->headerRect();
                        auto child_rect = child->currentRect();
                        header_rect.moveTopLeft(cell.child_offsets[child]);
                        child_rect.moveTopLeft(cell.child_offsets[child]);
                        current_rect = header_rect;
                        cell_icon_rect.index = child->getSortOrder();

                        if(use_vertical_layout){
                            next_rect = child_rect;
                        }else{
                            next_rect = header_rect;
                        }
                    }

                    //If we are first or last, update the current_rect to be full width
                    if((is_first || is_last) && use_vertical_layout){
                        current_rect.setLeft(boundary_rect.left());
                        current_rect.setRight(boundary_rect.right());
                    }

                    //If we are last, update the previous_rect to be full width
                    if(is_last && use_vertical_layout){
                        last_rect.setLeft(boundary_rect.left());
                        last_rect.setRight(boundary_rect.right());
                    }

                    auto& gap_rect = cell_icon_rect.gap_rect;
                    gap_rect = GetGapIconRect(index, i, last_rect, current_rect);
                    if(gap_rect.isValid()){
                        auto& icon_rect = cell_icon_rect.icon_rect;
                        icon_rect.setSize(smallIconSize());
                        icon_rect.moveCenter(gap_rect.center());
                        cell.child_gap_rects.append(cell_icon_rect);
                    }
                    last_rect = next_rect;
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
                        auto is_suffix = is_prefix ? false : cell.suffix_gap_index == i;

                        const auto& cell_icon_rect = cell.child_gap_rects[i];
                        const auto& icon_rect = cell_icon_rect.icon_rect;

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

                        auto& icon = is_prefix ? p_cell.prefix_icon : is_suffix ? p_cell.suffix_icon : p_cell.gap_icon;

                        auto is_hovered = cell_icon_rect.hovered;
                        const auto& icon_path = is_hovered && p_cell.render_hover_icon ? p_cell.hovered_icon : icon;
                        const auto icon_color = is_hovered ? getHighlightTextColor() : getTextColor();
                        const auto background_color = p_cell.use_alt_color ? getBodyColor() : getAltBodyColor();

                        painter->setBrush(is_hovered ? getHighlightColor() : background_color);
                        painter->setPen(Qt::NoPen);
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
    if(column_count > 0 && (child_index % column_count == 0 || child_index == sibling_count)){
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