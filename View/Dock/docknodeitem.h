#ifndef DOCKNODEITEM_H
#define DOCKNODEITEM_H

#include "../nodeview.h"

#include <QPushButton>
#include <QLabel>

class DockScrollArea;

class DockNodeItem : public QPushButton
{
    Q_OBJECT
public:
    explicit DockNodeItem(QString kind = "", EntityItem* item = 0, QWidget* parent = 0, bool isLabel = false);

    EntityItem* getNodeItem();
    QString getKind();
    QString getID();

    void setLabel(QString newLabel);
    QString getLabel();

    void setParentDockNodeItem(DockNodeItem* parentItem);
    DockNodeItem* getParentDockNodeItem();

    void addChildDockItem(DockNodeItem* dockItem);
    void removeChildDockItem(DockNodeItem* dockItem);
    QList<DockNodeItem*> getChildrenDockItems();

    void setImage(QString prefix, QString image);

    void setForceHidden(bool hide);
    bool isForceHidden();

    void setHidden(bool hide);
    bool isHidden();

    bool isFileLabel();
    bool isExpanded();

    void setReadOnlyState(bool on);

signals:
    void dockItem_clicked();
    void dockItem_fileClicked(bool show);
    void dockItem_relabelled(DockNodeItem* dockItem);
    void dockItem_hidden();

public slots:
    void clicked();
    void parentDockItemClicked(bool show);

    void labelChanged(QString label);
    void iconChanged();

    void childDockItemHidden();
    void highlightDockItem(NodeItem* nodeItem);

private:
    void setupLayout();
    void updateTextLabel();
    void updateStyleSheet();

    void setDockItemVisible(bool visible, bool forceChange = false);
    bool isDockItemVisible();

    DockScrollArea* parentDock;
    EntityItem* nodeItem;
    DockNodeItem* parentDockItem;
    QList<DockNodeItem*> childrenDockItems;

    QString kind;
    QString label;
    QString strID;

    QLabel* textLabel;
    QLabel* imageLabel;

    QString highlightColor;

    bool fileLabel;
    bool expanded;
    bool hidden;
    bool forceHidden;

    int state;
};

#endif // DOCKNODEITEM_H
