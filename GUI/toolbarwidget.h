#ifndef TOOLBARWIDGET_H
#define TOOLBARWIDGET_H

#include "nodeitem.h"

#include <QWidget>

class ToolbarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ToolbarWidget(NodeItem* item, QWidget *parent = 0);

signals:

public slots:

private:
    void setupToolBar();

    NodeItem* nodeItem;

};

#endif // TOOLBARWIDGET_H
