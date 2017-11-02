#ifndef XMITREEVIEWDIALOG_H
#define XMITREEVIEWDIALOG_H
#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QLayout>
#include <QTreeView>
#include "../xmitreemodel.h"

class XMITreeViewDialog: public QDialog{
    Q_OBJECT
public:
    XMITreeViewDialog(XMITreeModel* treeModel, QWidget* parent = 0);

signals:
    void importClasses(QStringList classIDs);

private slots:
    void selectionChanged(Qt::CheckState overallState);
    void importSelection();
private:
    void setupLayout();

    //Graphical Items
    QTreeView* treeView;
    QPushButton* button_selectAll;
    QPushButton* button_selectNone;
    QPushButton* button_importSelection;
    QPushButton* button_cancelImport;
    QHBoxLayout* layout_buttons;
    QVBoxLayout* layout_main;

    QWidget* parentWidget;
    XMITreeModel* treeModel;
};

#endif //XMITREEVIEWDIALOG_H
