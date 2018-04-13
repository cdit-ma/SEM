#include "statictextitem.h"
#include "entityitem.h"
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
        //Get the font from the painter
        auto font = painter->font();
        auto current_size = max_size;

        //Don't let the font be taller than the height
        if(current_size > rect.height()){
            current_size = rect.height();
        }

        auto test_br = rect.toAlignedRect();

        QRect text_bounding_rect;
        int topLineWidth = 0;

        while(current_size >= min_size){
            font.setPixelSize(current_size);
            auto fm = QFontMetrics(font);
            auto valid_bounding_rect = fm.boundingRect(rect.toAlignedRect(), Qt::TextWrapAnywhere | text_align, text);

            if(test_br.contains(valid_bounding_rect)){
                text_bounding_rect = valid_bounding_rect;
                break;
            }else{
                qCritical() << " == " << bounding_rect;
                qCritical() << " BR: " << bounding_rect;
                qCritical() << " TBR: " << valid_bounding_rect;
                qCritical() << " TEXT: " << text << " SIZE: " << current_size;
                current_size --;
            }
        }

        

        //Use the font
        this->font = font;

        QPointF offset;

        //Center 
        if(Qt::AlignVCenter & text_align){
            qCritical() << text << ": TEXT IS AlignVCenter ALIGNED";
            offset = QPointF(0, (rect.height() - text_bounding_rect.height()) / 2.0);
        }

        //Update the text bounding rect
        text_rect = QRectF(text_bounding_rect);
        top_left = rect.topLeft() + offset;
    }
}
