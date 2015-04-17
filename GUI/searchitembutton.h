#ifndef SEARCHITEMBUTTON_H
#define SEARCHITEMBUTTON_H

#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

#include "../View/GraphicsItems/graphmlitem.h"
#include "../View/GraphicsItems/nodeitem.h"


class SearchItemButton : public QPushButton
{
    Q_OBJECT
public:
    explicit SearchItemButton(GraphMLItem* item, QWidget *parent = 0);

    GraphMLItem* getGraphMLItem();
    QString getNodeKind();

signals:
    void searchItem_centerOnItem(GraphMLItem* item);

public slots:
    void itemClicked();
    void itemClicked(SearchItemButton* item);

private:
    void setupLayout();
    void updateColor(bool selected);

    GraphMLItem* graphMLItem;

    QLabel* iconLabel;
    QLabel* objectLabel;
    QLabel* parentLabel;

    bool selected;

};

#endif // SEARCHITEMBUTTON_H
