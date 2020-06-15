#ifndef QOSBROWSER_H
#define QOSBROWSER_H

#include <QFrame>
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
    explicit QOSBrowser(ViewController *vc, QWidget *parent = nullptr);

private slots:
    void themeChanged();

    void profileSelected(QModelIndex, QModelIndex);
    void settingSelected(QModelIndex, QModelIndex);
    void removeSelectedProfile();

private:
    void setupLayout();
    
    ViewController* vc = nullptr;
    
    QWidget* mainWidget = nullptr;
    
    QListView* profileView = nullptr;
    QTreeView* elementView = nullptr;
    DataTableView* tableView = nullptr;
    
    QToolBar* toolbar = nullptr;
    QAction* removeSelection = nullptr;
    QSplitter* horizontalSplitter = nullptr;
    
    QPushButton* profileLabelButton = nullptr;
    QPushButton* policyLabelButton = nullptr;
    QPushButton* attributeLabelButton = nullptr;
    
    QOSProfileModel* qosModel = nullptr;
    QItemSelectionModel* elementViewSelectionModel = nullptr;
};

#endif // QOSBROWSER_H