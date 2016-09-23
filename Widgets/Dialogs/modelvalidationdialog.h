#ifndef MODELVALIDATIONDIALOG_H
#define MODELVALIDATIONDIALOG_H

#include <QDialog>
#include <QtWidgets>
#include <QMessageBox>

class QDialogButtonBox;
class QTableWidget;
class MedeaWindow;

class ModelValidationDialog : public QDialog
{
    Q_OBJECT

public:
    ModelValidationDialog(QWidget *parent = 0);



    void gotResults(QString filePath);
signals:
    void searchItem_centerOnItem(int ID);
    void revalidate_Model();

private slots:
    void cellSelected(int nRow, int nCol);
    void revalidate();
private:
    void setupItemsTable(QStringList items);

    QStringList itemsID;
    QTableWidget * itemsTable;
    QHBoxLayout* buttonLayout;
    QHBoxLayout* titleLayout;

    QPushButton* revalidateButton;
    QLabel* label;
    QLabel* statusIcon;
    QMovie* spinning;
};

#endif // MODELVALIDATIONDIALOG_H
