#ifndef SEARCHITEM_H
#define SEARCHITEM_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMainWindow>

#include "../View/GraphicsItems/graphmlitem.h"
#include "../View/GraphicsItems/nodeitem.h"

class SearchItem : public QLabel
{
    Q_OBJECT

public:
    explicit SearchItem(GraphMLItem *item, QWidget *parent = 0);
    void connectToWindow(QMainWindow* window);

    QString getKeyValue(QString key);
    int getItemID();
    int getItemWidth();

signals:
    void searchItem_clicked();
    void searchItem_centerOnItem(int ID);

public slots:
    void itemClicked(SearchItem* item);
    void expandItem();
    void centerOnItem();

    void setClickToCenter(bool b);
    void setDoubleClickToExpand(bool b);

protected:
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);

private:
    void setupLayout();
    QLabel* setupDataValueBox(QString key, QLayout* layout = 0, bool storeInHash = true);

    void updateColor();
    void getDataValues();
    QString getItemLocation();

    GraphMLItem* graphMLItem;

    QLabel* iconLabel;
    QLabel* entityLabel;

    QGroupBox* dataBox;
    QLabel* locationLabel;
    QLabel* kindLabel;
    QLabel* typeLabel;
    QLabel* topicLabel;
    QLabel* workerLabel;
    QLabel* descriptionLabel;

    QStringList dataKeys;
    QHash<QString, QGroupBox*> dataValueBoxes;
    QHash<QString, QLabel*> dataValueLabels;

    QPushButton* expandButton;
    QPushButton* centerOnButton;
    QPixmap expandPixmap;
    QPixmap contractPixmap;
    QString fixedStyleSheet;

    int graphMLItemID;
    bool selected;
    bool expanded;
    bool valuesSet;

    float MIN_WIDTH;
    int ITEM_WIDTH;
    bool CLICK_TO_CENTER;
    bool DOUBLE_CLICK_TO_EXPAND;
};

#endif // SEARCHITEM_H
