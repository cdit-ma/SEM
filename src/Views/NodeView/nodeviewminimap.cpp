#include "nodeviewminimap.h"
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include "../../theme.h"

NodeViewMinimap::NodeViewMinimap(QObject*)
{
    setMouseTracking(true);

    zoom_icon = Theme::theme()->getImage("Icons", "zoom", zoomIcon().size(), Qt::white);
    
    setMinimumWidth(200);
    QTimer* updateTimer = new QTimer(this);
    updateTimer->setInterval(1000);
    updateTimer->start();
    connect(updateTimer, &QTimer::timeout, this, &NodeViewMinimap::updateScene);

    //Set font
    zoom_font.setBold(true);
    zoom_font.setPixelSize(12);
}


void NodeViewMinimap::setMinimapPanning(bool pan)
{
    if(pan != isPanning()){
        is_panning = pan;

        if(isPanning()){
            setCursor(Qt::ClosedHandCursor);
        }else{
            unsetCursor();
        }
    }
}

void NodeViewMinimap::viewport_changed(QRectF viewport, double zoom_factor)
{
    viewport_rect = viewport;
    zoom_str = QString::number(zoom_factor * 100, 'f', 2 ) + "%";
    updateViewport();
}

QRect NodeViewMinimap::zoomIcon() const
{
    QRect rect;
    rect.setWidth(16);
    rect.setHeight(16);
    rect.moveTopLeft(infoBox().topLeft() + QPoint(2, 2));
    return rect;
}

QRect NodeViewMinimap::zoomText() const
{
    auto rect = QRect(zoomIcon().topRight(), (infoBox().bottomRight() - QPoint(4,2)));
    return rect;
}

QRect NodeViewMinimap::infoBox() const
{
    QRect rect;
    rect.setWidth(85);
    rect.setHeight(20);

    rect.moveBottomRight(this->rect().bottomRight());
    return rect;
}


bool NodeViewMinimap::viewportContainsPoint(QPoint pos)
{
    return translated_viewport_rect.contains(pos);
}

void NodeViewMinimap::setBackgroundColor(QColor color)
{
    background_color = color;
}

void NodeViewMinimap::setScene(QGraphicsScene *scene)
{
    this->scene = scene;
    updateScene();
}

void NodeViewMinimap::updateViewport(){
    //Calculate the 
    auto w_mult = viewport_rect.width() / scene_rect.width();
    auto h_mult = viewport_rect.height() / scene_rect.height();

    auto x_pos = viewport_rect.x() - scene_rect.x();
    auto y_pos = viewport_rect.y() - scene_rect.y();

    auto area_rect = renderArea();
    //Calculate the offset for the top-left of the rectangle
    auto x = x_pos * (area_rect.width() / scene_rect.width());
    auto y = y_pos * (area_rect.height() / scene_rect.height());

    //Calculate the proportional width/height
    auto width = area_rect.width() * w_mult;
    auto height = area_rect.height() * h_mult;

    translated_viewport_rect = QRect(x, y, width, height);
    update();
}

void NodeViewMinimap::updateScene(){
    if(scene){
        auto view_rect = renderArea();
        //scene_rect = scene->sceneRect();;//;scene->itemsBoundingRect();
        scene_rect = scene->itemsBoundingRect();

        auto ratio = view_rect.width() / (double) view_rect.height();

        auto width = scene_rect.width();
        auto height = width / ratio;

        if(height < scene_rect.height()){
            //Doesn't contain
            height = scene_rect.height();
            width = height * ratio;
        }

        auto render_rect = QRectF(0, 0, width, height);
        render_rect.moveCenter(scene_rect.center());

        scene_rect = render_rect;

        QPixmap pix(view_rect.size());
        
        if(!pix.isNull()){
            pix.fill(Qt::transparent);
            QPainter painter(&pix);
            //Paint into the pixmap
            scene->render(&painter, QRectF(), scene_rect);
        }
        pixmap.swap(pix);
        updateViewport();
    }else{
        update();
    }
}

bool NodeViewMinimap::isPanning() const
{
    return is_panning;
}

void NodeViewMinimap::mousePressEvent(QMouseEvent *event)
{
    if(scene){
        if(event->button() == Qt::LeftButton || event->button() == Qt::RightButton){
            if(viewportContainsPoint(event->pos())){
                previous_pos = getScenePos(event->pos());
                setMinimapPanning(true);
            }
        }else if(event->button() == Qt::MiddleButton){
            emit minimap_CenterView();
        }
    }
}

void NodeViewMinimap::mouseReleaseEvent(QMouseEvent *)
{
    setMinimapPanning(false);
}

void NodeViewMinimap::paintEvent(QPaintEvent *event)
{
    if(!scene){
        return;
    }

    QPainter painter(this);
    
    //Get the area 
    auto widget_rect = event->rect();
    painter.setClipRect(widget_rect);

    painter.save();
    painter.setPen(Qt::NoPen);
    {
        painter.save();
        //Paint the backgound
        painter.setBrush(background_color);
        painter.drawRect(widget_rect);
        painter.restore();
    }

    if(!pixmap.isNull()){
        //Render the minimap pixmap in the space provided
        painter.drawPixmap(renderArea(), pixmap);
    }

    {
        //Paint the darker tinted region
        painter.save();
        //Calculate the area we should tint darker
        QRegion widget_region(widget_rect);
        QRegion view_region(translated_viewport_rect);
        auto masked_region = widget_region.xored(view_region);

        //Mask off the current translated_viewport_rect
        painter.setClipRegion(masked_region);

        //Paint the overlay
        painter.setBrush(QColor(0, 0, 0, 100));
        painter.drawRect(widget_rect);

        painter.restore();
    }

    {
        //Paint the current viewport rectangle
        painter.save();
   
        //Paint the translated_viewport_rect (With no Brush just a Pen)
        QPen pen(Qt::white);
        pen.setCosmetic(true);

        if(isPanning()) {
            pen.setColor(Qt::blue);
        }
        pen.setJoinStyle(Qt::MiterJoin);
        pen.setWidth(2);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(pen);
        painter.drawRect(translated_viewport_rect);

        painter.restore();
    }

    {
        //Paint overlayed info bar
        painter.save();
        painter.setFont(zoom_font);
        painter.resetTransform();

        painter.setBrush(Qt::darkGray);
        painter.drawRect(infoBox());
        painter.setPen(Qt::white);

        painter.drawText(zoomText(), Qt::AlignCenter|Qt::AlignRight, zoom_str);
        QRect imageRect = zoomIcon();
        //painter.drawPixmap(imageRect.x(), imageRect.y(), imageRect.width(), imageRect.height(), zoom_icon);
        painter.drawPixmap(imageRect, zoom_icon);
        
        painter.restore();
    }
    painter.restore();
}

QPointF NodeViewMinimap::getScenePos(QPoint mouse_pos){
    auto r = renderArea();
    auto x_mult = scene_rect.width() / (double)r.width();
    auto y_mult = scene_rect.height() / (double)r.height();
    return QPointF(mouse_pos.x() * x_mult, mouse_pos.y() * y_mult);
}

void NodeViewMinimap::mouseMoveEvent(QMouseEvent *event)
{
    if(scene && isPanning()){
        auto delta = previous_pos - getScenePos(event->pos());
        emit minimap_Pan(delta);
        //Update the previous Scene position after the view has panned, such that we get smooth movement.
        previous_pos = getScenePos(event->pos());
    }
}

void NodeViewMinimap::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(scene &&  event->button() == Qt::LeftButton){
        auto delta = getScenePos(translated_viewport_rect.center()) - getScenePos(event->pos());
        setMinimapPanning(true);
        emit minimap_Pan(delta);
        setMinimapPanning(false);
    }
}

QRect NodeViewMinimap::renderArea() const{
    QRect area = rect();
    area.setHeight(area.height() - infoBox().height());
    return area;
}
void NodeViewMinimap::wheelEvent(QWheelEvent *event)
{
    if(scene){
        emit minimap_Zoom(event->delta());
    }
}

void NodeViewMinimap::resizeEvent(QResizeEvent *)
{
    updateScene();
}
