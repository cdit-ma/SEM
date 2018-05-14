#include "statictextitem.h"
#include "entityitem.h"
#include <cmath>
#include <QDebug>

StaticTextItem::StaticTextItem(Qt::Alignment text_align){
    text_item.setPerformanceHint(QStaticText::AggressiveCaching);
    setAlignment(text_align);
}

void StaticTextItem::setAlignment(Qt::Alignment text_align){
    dirty_ = true;
    this->text_align = text_align;
    option = QTextOption(text_align);
    option.setWrapMode(QTextOption::WrapAnywhere);
    text_item.setTextOption(option);
}

void StaticTextItem::setText(QString text){
    this->text = text;
}

void StaticTextItem::RenderText(QPainter* painter, RENDER_STATE state, QRectF rect, QString text){
    painter->save();
    {
        if(text == ""){
            text = this->text;
        }

        
        
        UpdateText(painter, rect, text);
        painter->setClipRect(rect);

        /*
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(255,0,0,100));
        painter->drawRect(rect);
        painter->setBrush(QColor(0,0,255,100));
        painter->drawRect(text_rect);
        painter->restore();*/

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

void StaticTextItem::UpdateText(QPainter* painter, QRectF rect, QString text){

    bool recalculate = dirty_;
    
    if(this->text != text || dirty_){
        this->text = text;
        text_item.setText(text);
        recalculate = true;
        dirty_ = false;
    }


    if(this->bounding_rect != rect){
        this->bounding_rect = rect;
        text_item.setTextWidth(rect.width());
        text_item.setTextOption(option);
        recalculate = true;
    }

    if(recalculate){
        //qCritical() << "RECALCULATING FOR: " << text;
        //Get the font from the painter
        auto font = painter->font();
        int current_size = max_size;
        
        //Don't let the font be taller than the height
        if(current_size > rect.height()){
            current_size = rect.height();
        }

        QRect text_bounding_rect;

        bool elided = false;
        bool valid = false;
        QString elided_text;

        //qCritical() << "Bounding Rect: " << rect;

        while(current_size >= min_size){
            font.setPixelSize(current_size);
            auto fm = QFontMetrics(font);

            auto text_br = fm.boundingRect(rect.toAlignedRect(), Qt::TextWrapAnywhere | text_align, text);

            if(text_br.width() <= rect.width() && text_br.height() <= rect.height()){
                text_bounding_rect = text_br;
                break;
            }

            if(current_size == min_size){
                //Try Eliding;
                int line_height = fm.lineSpacing() + fm.height();
                int max_lines = ceil(rect.height() / line_height);
                int max_width = (floor(rect.width()) * max_lines) - fm.width("..");

                /*qCritical() << "line_height:" << line_height;
                qCritical() << "max_lines:" << max_lines;
                qCritical() << "max_width:" << max_width;*/

                auto truncated_text = fm.elidedText(text, Qt::ElideMiddle, max_width);
                text_bounding_rect = fm.boundingRect(rect.toAlignedRect(), Qt::TextWrapAnywhere | text_align, truncated_text);
                elided_text = truncated_text;
                elided = true;
            }
            current_size --;
        }

        bool update_text = false;

        if(elided_text_ != elided_text){
            elided_text_ = elided_text;
            update_text = true;
        }

        if(is_elided_ != elided){
            is_elided_ = elided;
            update_text = true;
        }

        if(update_text){
            text_item.setText(is_elided_ ? elided_text : text);
        }
        

        //Use the font
        this->font = font;

        QPointF offset;
        //Center 
        if(Qt::AlignVCenter & text_align){
            offset.ry() += (rect.height() - text_bounding_rect.height()) / 2.0;
        }

        //Update the text bounding rect
        text_rect = QRectF(text_bounding_rect);
        top_left = rect.topLeft() + offset;
    }
}
