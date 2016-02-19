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
    explicit DockNodeItem(QString kind = "", EntityItem* item = 0, QWidget* parent = 0, bool isLabel = false, QString imageName = "");

    EntityItem* getNodeItem();
    QString getKind();

    void setID(QString strID);
    QString getID();

    void setLabel(QString newLabel);
    QString getLabel();

    void setParentDockNodeItem(DockNodeItem* parentItem);
    DockNodeItem* getParentDockNodeItem();

    void addChildDockItem(DockNodeItem* dockItem);
    void removeChildDockItem(DockNodeItem* dockItem);
    QList<DockNodeItem*> getChildrenDockItems();

    void setImage(QString prefix, QString image);

    void setDockItemVisible(bool visible);
    bool isDockItemVisible();

    void setHidden(bool hide);
    bool isHidden();

    void setForceHidden(bool hide);
    bool isForceHidden();

    void setReadOnlyState(bool on);

    bool isDockItemLabel();
    bool isExpanded();

signals:
    void dockItem_clicked();
    void dockItem_fileClicked(bool show);
    void dockItem_relabelled(DockNodeItem* dockItem);
    void dockItem_hiddenStateChanged();

public slots:
    void dataChanged(QString keyName, QVariant data);
    void clicked();
    void parentDockItemClicked(bool show);

    void labelChanged(QString label);
    void iconChanged();
    void childVisibilityChanged();

    void changeVectorHiddenState();
    void highlightDockItem(NodeItem* nodeItem);

private:
    void setupLayout();
    void setImageLabelPixmap();

    void updateTextLabel();
    void updateStyleSheet();

    void toggleDockItemExpanded();
    bool hasVisibleChildren();

    DockScrollArea* parentDock;
    EntityItem* nodeItem;
    DockNodeItem* parentDockItem;
    QList<DockNodeItem*> childrenDockItems;

    QString kind;
    QString label;
    QString strID;
    QString imageName;

    QLabel* textLabel;
    QLabel* imageLabel;

    QString highlightColor;

    bool dockItemLabel;
    bool dockItemVisible;

    bool expanded;
    bool hidden;
    bool forceHidden;

    int state;
    int labelPadding;
    int MAX_LABEL_LENGTH;
};

#endif // DOCKNODEITEM_H
