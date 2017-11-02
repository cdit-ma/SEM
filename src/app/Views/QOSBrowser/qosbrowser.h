#ifndef QOSBROWSER_H
#define QOSBROWSER_H

#include <QObject>
#include <QFrame>
#include <QWidget>
#include <QTreeView>
#include <QListView>
#include <QPushButton>
#include <QSplitter>

#include "qosprofilemodel.h"
#include "../Table/datatableview.h"
#include "../../Controllers/ViewController/viewcontroller.h"

class QOSBrowser : public QFrame
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

    QWidget* mainWidget;

    QListView* profileView;
    QTreeView* elementView;
    QSplitter* horizontalSplitter;
    QAction* removeSelection;
    QToolBar* toolbar;

    QPushButton* profileLabelButton;
    QPushButton* policyLabelButton;
    QPushButton* attributeLabelButton;

    QItemSelectionModel* elementViewSelectionModel;
    DataTableView* tableView;
    ViewController* vc;
    QOSProfileModel* qosModel;
};

#endif // QOSBROWSER_H
