#ifndef QOSBROWSER_H
#define QOSBROWSER_H

#include <QObject>
#include <QWidget>
#include <QTreeView>
#include <QListView>
#include <QPushButton>
#include <QSplitter>
#include "../../View/Table/attributetableview.h"
#include "Controller/viewcontroller.h"
#include "qosprofilemodel.h"

class QOSBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit QOSBrowser(ViewController *vc, QWidget *parent = 0);
private slots:
    void themeChanged();
    void profileSelected(QModelIndex, QModelIndex);
    void settingSelected(QModelIndex, QModelIndex);
private:
    void setupLayout();

    QListView* profileView;
    QTreeView* elementView;
    //QListView* profileView;
    //QListView* elementView;
    QSplitter* horizontalSplitter;

    QItemSelectionModel* elementViewSelectionModel;
    AttributeTableView* tableView;
    ViewController* vc;
    QOSProfileModel* qosModel;
};

#endif // QOSBROWSER_H
