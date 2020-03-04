#include "statictextitem.h"
#include "entityitem.h"
#include <cmath>

StaticTextItem::StaticTextItem(Qt::Alignment text_align)
{
    text_item.setPerformanceHint(QStaticText::AggressiveCaching);
    setAlignment(text_align);
}

void StaticTextItem::setAlignment(Qt::Alignment alignment)
{
    dirty_ = true;
    text_align = alignment;
    option = QTextOption(alignment);
    option.setWrapMode(QTextOption::WrapAnywhere);
    text_item.setTextOption(option);
}

void StaticTextItem::setText(QString text)
{
    text_ = std::move(text);
}

void StaticTextItem::RenderText(QPainter* painter, RENDER_STATE state, QRectF rect)
{
    RenderText(painter, state, rect, text_);
}

void StaticTextItem::RenderText(QPainter* painter, RENDER_STATE state, QRectF rect, const QString& text)
{
    painter->save();
    {
        UpdateText(painter, rect, text);
        painter->setClipRect(rect);
        switch(state){
        case RENDER_STATE::BLOCK:
            break;
        case RENDER_STATE::MINIMAL:{
            //Get the text colour 
            auto color = painter->pen().color();
            painter->setPen(Qt::NoPen);
            painter->setBrush(color);
            painter->drawRect(text_rect);
            break;
        }
        default:
            painter->setFont(font);
            painter->drawStaticText(top_left, text_item);
            break;
        }
    }
    painter->restore();
}

void StaticTextItem::UpdateText(QPainter* painter, QRectF rect, const QString& text)
{
    bool recalculate = dirty_;
    
    if (text_ != text || dirty_) {
        text_ = text;
        text_item.setText(text);
        recalculate = true;
        dirty_ = false;
    }

    if (bounding_rect != rect) {
        bounding_rect = rect;
        text_item.setTextWidth(rect.width());
        text_item.setTextOption(option);
        recalculate = true;
    }

    if (recalculate) {
        //Get the font from the painter
        auto current_font = painter->font();
        int current_size = max_size;
        
        //Don't let the font be taller than the height
        if (current_size > rect.height()) {
            current_size = rect.height();
        }

        QRect text_bounding_rect;
        QString elided_text;
        bool elided = false;

        while (current_size >= min_size) {
        
            current_font.setPixelSize(current_size);
        
            auto fm = QFontMetrics(current_font);
            auto text_br = fm.boundingRect(rect.toAlignedRect(), Qt::TextWrapAnywhere | text_align, text);
            if (text_br.width() <= rect.width() && text_br.height() <= rect.height()) {
                text_bounding_rect = text_br;
                break;
            }

            if (current_size == min_size) {
                //Try Eliding;
                int line_height = fm.lineSpacing() + fm.height();
                int max_lines = ceil(rect.height() / line_height);
                int max_width = (floor(rect.width()) * max_lines);
                if (max_lines > 1) {
                    max_width -= (max_lines * fm.horizontalAdvance("."));
                }
                auto truncated_text = fm.elidedText(text, Qt::ElideMiddle, max_width);
                text_bounding_rect = fm.boundingRect(rect.toAlignedRect(), Qt::TextWrapAnywhere | text_align, truncated_text);
                elided_text = truncated_text;
                elided = true;
            }
            
            current_size --;
        }

        bool update_text = false;
        if (elided_text_ != elided_text) {
            elided_text_ = elided_text;
            update_text = true;
        }
        if (is_elided_ != elided) {
            is_elided_ = elided;
            update_text = true;
        }
        if (update_text) {
            text_item.setText(is_elided_ ? elided_text : text);
        }
        
        //Use the painter's font
        font = current_font;
    
        //Update the text bounding rect
        text_rect = QRectF(text_bounding_rect);
        top_left = rect.topLeft();
    
        //Center
        if (Qt::AlignVCenter & text_align) {
            top_left.ry() += (rect.height() - text_bounding_rect.height()) / 2.0;
        }
    }
}
