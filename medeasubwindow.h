#ifndef MEDEASUBWINDOW_H
#define MEDEASUBWINDOW_H

#include <QDialog>
#include <QVBoxLayout>
#include <QTableView>
#include <QGroupBox>
#include "View/Table/attributetablemodel.h"

class NodeView;

class MedeaSubWindow : public QDialog
{
    Q_OBJECT
public:
    explicit MedeaSubWindow(QWidget *parent = 0);
    void setNodeView(NodeView* view);

private:
    void setupDataTable();
    void setupLayout();
    void updateDataTable();
    void updateWidgetMask(QWidget* widget, QWidget* maskWidget);
signals:
    void window_Resized();

public slots:
    void setAttributeModel(AttributeTableModel* model=0);


    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event);

private:
     QHBoxLayout* mainLayout;
     NodeView* nodeView;
     QTableView* dataTable;
     QWidget* dataTableBox;
};

#endif // MEDEASUBWINDOW_H
