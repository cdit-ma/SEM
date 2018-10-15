#ifndef ENTITYAXIS_H
#define ENTITYAXIS_H

#include <QWidget>
#include <QVBoxLayout>

class EntitySet;
class EntityAxis : public QWidget
{
    Q_OBJECT

public:
    explicit EntityAxis(QWidget* parent = 0);

    int appendEntity(EntitySet* entitySet);
    int insertEntity(EntitySet* prevEntitySet, EntitySet* entitySet);
    int removeEntity(EntitySet* entitySet);

    int getEntityAxisIndex(EntitySet* entitySet);

    void setAxisLineVisible(bool visible);
    void setTickVisible(bool visible);

signals:
    void sizeChanged(QSizeF size);

public slots:
    void themeChanged();
    void resizeEvent(QResizeEvent* event);

private:
    QVBoxLayout* mainLayout;

    int minimumEntityHeight = 0;
    int maximumEntityWidth = 0;

    bool axisLineVisible = true;
    bool tickVisible = true;

};

#endif // ENTITYAXIS_H
