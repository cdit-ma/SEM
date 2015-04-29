#ifndef VALIDATIONDIALOG_H
#define VALIDATIONDIALOG_H

#include <QDialog>
#include <QtWidgets>
#include <QMessageBox>

class QDialogButtonBox;
class QTableWidget;
class MedeaWindow;

class ValidateDialog : public QDialog
{
    Q_OBJECT

public:
    ValidateDialog(QWidget *parent = 0);

    void connectToWindow(QMainWindow* window);

    void setupItemsTable(QStringList *items = 0);

signals:
    void searchItem_centerOnItem(QString id);

private slots:
    void cellSelected(int nRow, int nCol);

private:

    QStringList itemsID;
    QTableWidget * itemsTable;
    QDialogButtonBox *buttonBox;
};

#endif // VALIDATIONDIALOG_H
