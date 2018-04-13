#ifndef STATICTEXTITEM_H
#define STATICTEXTITEM_H

#include <QStaticText>
#include <QRectF>
#include <QFont>
#include <QPainter>
#include <QTextOption>


//Forward Declare
enum class RENDER_STATE;
class StaticTextItem{
    public:
        StaticTextItem(Qt::Alignment text_align = Qt::AlignCenter);
        void setText(QString text);
        void RenderText(QPainter* painter, RENDER_STATE state, QRectF rect, QString text = "");
    private:
        void UpdateText(QPainter* painter, QRectF rect, QString text);
        QStaticText text_item;
        QRectF bounding_rect;
        QRectF text_rect;
        QString text;
        QFont font;
        QPointF top_left;
        Qt::Alignment text_align;
        QTextOption option;

        bool setup = true;

        int max_size = 10;
        int min_size = 3;
};

#endif //STATICTEXTITEM_HT