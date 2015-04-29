#ifndef DOCKNODEITEM_H
#define DOCKNODEITEM_H

#include "../nodeview.h"

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

    void setParentDockNodeItem(DockNodeItem* parentItem);
    DockNodeItem* getParentDockNodeItem();

    void addChildDockItem(DockNodeItem* dockItem);
    void removeChildDockItem(DockNodeItem* dockItem);
    QList<DockNodeItem*> getChildrenDockItems();

    void setHidden(bool hideItem);
    bool isHidden();

    bool isFileLabel();
    bool isExpanded();

protected:
    void paintEvent(QPaintEvent* e);

signals:
    void dockItem_clicked();
    void dockItem_fileClicked(bool show);
    void dockItem_relabelled(DockNodeItem* dockItem);
    void dockItem_removeFromDock(DockNodeItem* dockItem);

    void dockItem_hidden();

public slots:
    void clicked();
    void parentDockItemClicked(bool show);
    void deleteLater();

    void updateData();
    void setOpacity(double opacity);

    void childDockItemHidden();

    void highlightDockItem(Node* node);

private:
    void setupLayout();
    void connectToNodeItem();
    void updateTextLabel();
    void updateStyleSheet();
    QPixmap getScaledPixmap(QImage* img);

    NodeItem* nodeItem;
    DockNodeItem* parentDockItem;
    QList<DockNodeItem*> childrenDockItems;

    QString kind;
    QString label;

    QLabel* textLabel;
    QLabel* imageLabel;

    QPixmap defaultPixmap;
    QPixmap highlightPixmap;

    bool fileLabel;
    bool expanded;
    bool hidden;
    bool highlighted;

};

#endif // DOCKNODEITEM_H
