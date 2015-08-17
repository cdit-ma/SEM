#ifndef SEARCHITEMBUTTON_H
#define SEARCHITEMBUTTON_H

#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QMainWindow>

#include "../View/GraphicsItems/graphmlitem.h"
#include "../View/GraphicsItems/nodeitem.h"

//class MedeaWindow;

class SearchItemButton : public QPushButton
{
    Q_OBJECT
public:
    explicit SearchItemButton(GraphMLItem* item, QWidget *parent = 0);

    void connectToWindow(QMainWindow* window);

signals:
    void searchItem_centerOnItem(QString ID);

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

    QString ID;
    bool selected;
    bool triggeredWithin;
};

#endif // SEARCHITEMBUTTON_H
