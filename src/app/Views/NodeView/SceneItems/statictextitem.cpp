#include "statictextitem.h"
#include "entityitem.h"
#include <QDebug>

StaticTextItem::StaticTextItem(){
    text_item.setPerformanceHint(QStaticText::AggressiveCaching);
    QTextOption text_options;
    //text_options.setWrapMode(QTextOption::WrapAnywhere);
    text_options.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    text_item.setTextOption(text_options);
}

void StaticTextItem::RenderText(QPainter* painter, RENDER_STATE state, QRectF rect, QString text){
    painter->save();
    {
        UpdateText(painter, rect, text);
        painter->setClipRect(rect);

        switch(state){
        case RENDER_STATE::BLOCK:
            break;
        case RENDER_STATE::MINIMAL:{
            painter->setPen(Qt::NoPen);
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

    bool recalculate = false;
    if(this->text != text){
        this->text = text;
        text_item.setText(text);
        recalculate = true;
    }

    if(this->bounding_rect != rect){
        this->bounding_rect = rect;
        text_item.setTextWidth(rect.width());
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

        QRect text_bounding_rect;
        QFontMetrics usedFM(font);
        int topLineWidth = 0;

        while(current_size >= min_size){
            font.setPixelSize(current_size);
            auto fm = QFontMetrics(font);

            text_bounding_rect = fm.boundingRect(rect.toAlignedRect(), Qt::TextWrapAnywhere, text);

            if(bounding_rect.contains(text_bounding_rect)){
                topLineWidth = text_bounding_rect.width();
                usedFM = fm;
                break;
            }else{
                current_size --;
            }
        }

        /*
         *  Added this to move word after "::" to the next line when wrapped
         *
        QRect newTextRect;
        QString truncatedText = text;
        QString newText = "";

        while ((newTextRect = usedFM.boundingRect(rect.toAlignedRect(), Qt::TextWordWrap, truncatedText)).width() > topLineWidth) {
            if (!truncatedText.contains("::")) {
                break;
            }
            QStringList list = truncatedText.split("::");
            truncatedText = truncatedText.left(truncatedText.lastIndexOf("::"));
            newText = ":: " + list.last() + newText;
        }

        newText = truncatedText + newText;
        //foreach (QString s, newText.split(" ")) {
        //    if (usedFM.width(s) > topLineWidth) {
                  // truncate string
        //    }
        //}

        if (newText != text) {
            this->text = newText;
            text_item.setText(newText);
        }
        */

        //Use the font
        this->font = font;
        //Update the top left corner of the text()
        top_left = rect.topLeft() + QPointF(0, (rect.height() - text_bounding_rect.height()) / 2.0);
        //Update the text bounding rect
        text_rect = QRectF(text_bounding_rect);
        text_rect.moveTopLeft(top_left);
    }
}
