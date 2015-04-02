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

protected:
    void paintEvent(QPaintEvent* e);

signals:
    void dockItem_clicked();
    void dockItem_fileClicked(bool show);
    void dockItem_fileRelabelled(DockNodeItem* fileItem);
    void dockItem_removeFromDock(DockNodeItem* dockItem);

public slots:
    void clicked();
    void parentDockItemClicked(bool show);
    void deleteLater();

    void updateData();
    void setOpacity(double opacity);

private:
    void setupLayout();
    void connectToNodeItem();
    void updateTextLabel();

    NodeItem* nodeItem;
    DockNodeItem* parentDockItem;

    QString kind;
    QString label;

    QLabel* textLabel;

    bool fileLabel;
    bool expanded;

};

#endif // DOCKNODEITEM_H
