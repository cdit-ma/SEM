#ifndef STATICTEXTITEM_H
#define STATICTEXTITEM_H

#include <QStaticText>
#include <QRectF>
#include <QFont>
#include <QPainter>

//Forward Declare
enum class RENDER_STATE;
class StaticTextItem{
    public:
        StaticTextItem();
        void RenderText(QPainter* painter, RENDER_STATE state, QRectF rect, QString text);
    private:
        void UpdateText(QPainter* painter, QRectF rect, QString text);
        QStaticText text_item;
        QRectF bounding_rect;
        QRectF text_rect;
        QString text;
        QFont font;
        QPointF top_left;

        int max_size = 10;
        int min_size = 4;
};

#endif //STATICTEXTITEM_HT