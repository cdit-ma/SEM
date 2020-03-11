#ifndef STATICTEXTITEM_H
#define STATICTEXTITEM_H

#include <QStaticText>
#include <QRectF>
#include <QFont>
#include <QPainter>
#include <QTextOption>

//Forward Declare
enum class RENDER_STATE;
class StaticTextItem
{
public:
    explicit StaticTextItem(Qt::Alignment text_align = Qt::AlignCenter);
    
    void setAlignment(Qt::Alignment alignment);
    void setText(QString text);
    
    void RenderText(QPainter* painter, RENDER_STATE state, QRectF rect, const QString& text);
    void RenderText(QPainter* painter, RENDER_STATE state, QRectF rect);
    
private:
    void UpdateText(QPainter* painter, QRectF rect, const QString& text);
    
    QStaticText text_item;
    
    QRectF bounding_rect;
    QRectF text_rect;
    
    QString text_;
    QString elided_text_;
    
    QFont font;
    QPointF top_left;
    Qt::Alignment text_align;
    QTextOption option;
    
    bool is_elided_ = false;
    bool dirty_ = false;
    
    int max_size = 12;
    int min_size = 3;
};

#endif // STATICTEXTITEM_HT