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

signals:
    void searchItem_clicked();
    void searchItem_centerOnItem(int ID);

public slots:
    void itemClicked(SearchItem* item);
    void expandItem();
    void centerOnItem();

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

};

#endif // SEARCHITEM_H
