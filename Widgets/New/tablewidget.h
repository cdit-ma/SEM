#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QTableView>
#include <QVBoxLayout>
#include <QToolBar>
#include <QTableView>
#include <QLabel>
#include <QAction>
#include "../../View/viewitem.h"

class TableWidget : public QWidget
{
    Q_OBJECT
public:
    TableWidget(QWidget *parent = 0);

private slots:
    void activeSelectedItemChanged(ViewItem* item, bool isActive);
    void cyclePressed();
    void themeChanged();

signals:
    void cycleActiveItem(bool forward);

private:
    void setupLayout();

    QVBoxLayout * layout;
    ViewItem* activeItem;

    QSize iconSize;
    QTableView* tableView;
    QToolBar* toolbar;
    QLabel* iconLabel;
    QLabel* label;
    QAction* cycleBackwardAction;
    QAction* cycleForwardAction;

};

#endif // TABLEWIDGET_H
