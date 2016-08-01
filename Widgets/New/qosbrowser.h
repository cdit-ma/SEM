#ifndef QOSBROWSER_H
#define QOSBROWSER_H

#include <QObject>
#include <QWidget>
#include <QTreeView>
#include <QTableView>
#include "Controller/viewcontroller.h"
#include "qosprofilemodel.h"


class QOSBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit QOSBrowser(ViewController *vc, QWidget *parent = 0);

signals:

public slots:

private:
    void setupLayout();
    QTreeView* profileView;
    QTreeView* elementView;
    QTableView* tableView;
    ViewController* vc;
    QOSProfileModel* qosModel;
};

#endif // QOSBROWSER_H
