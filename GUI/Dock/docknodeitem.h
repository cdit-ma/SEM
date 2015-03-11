#ifndef DOCKNODEITEM_H
#define DOCKNODEITEM_H

#include "nodeview.h"

#include <QPushButton>
#include <QLabel>


class DockNodeItem : public QPushButton
{
    Q_OBJECT
public:
    explicit DockNodeItem(QString kind = "", NodeItem* item = 0, QWidget* parent = 0);

    NodeItem* getNodeItem();
    QString getKind();

    void setLabel(QString newLabel);
    QString getLabel();

protected:
    void paintEvent(QPaintEvent* e);

signals:
    void dockItem_clicked();
    void dockItem_removeFromDock(DockNodeItem* dockItem);

public slots:
    void clicked();
    void deleteLater();

    void updateData();
    void setOpacity(double opacity);

private:
    void setupLayout();
    void connectToNodeItem();

    NodeItem* nodeItem;

    QString kind;
    QString label;

    QLabel* textLabel;

};

#endif // DOCKNODEITEM_H
