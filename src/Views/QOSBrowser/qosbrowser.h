#ifndef QOSBROWSER_H
#define QOSBROWSER_H

#include <QObject>
#include <QWidget>
#include <QTreeView>
#include <QListView>
#include <QPushButton>
#include <QSplitter>

#include "qosprofilemodel.h"
#include "../Table/datatableview.h"
#include "../../Controllers/ViewController/viewcontroller.h"

class QOSBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit QOSBrowser(ViewController *vc, QWidget *parent = 0);

private slots:
    void themeChanged();
    void profileSelected(QModelIndex, QModelIndex);
    void settingSelected(QModelIndex, QModelIndex);

    void removeSelectedProfile();

private:
    void setupLayout();

    QListView* profileView;
    QTreeView* elementView;
    //QListView* profileView;
    //QListView* elementView;
    QSplitter* horizontalSplitter;
    QAction* removeSelection;

    QPushButton* profileLabelButton;
    QPushButton* policyLabelButton;
    QPushButton* attributeLabelButton;

    QItemSelectionModel* elementViewSelectionModel;
    DataTableView* tableView;
    ViewController* vc;
    QOSProfileModel* qosModel;
};

#endif // QOSBROWSER_H
