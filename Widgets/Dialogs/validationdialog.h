#ifndef VALIDATIONDIALOG_H
#define VALIDATIONDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QMovie>

class ValidationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ValidationDialog(QWidget *parent = 0);

    void gotResults(QString filePath);
    void showDialog();

signals:
    void centerOnItem(int ID);
    void revalidateModel();

public slots:
    void themeChanged();
    void revalidate();
    void cellSelected(int nRow, int nCol);

private:
    void setupLayout();
    void setupItemsTable(QStringList items);

    QStringList itemsID;
    QTableWidget* itemsTable;

    QPushButton* revalidateButton;
    QLabel* label;
    QLabel* statusIcon;
    QMovie* loadingMovie;
};

#endif // VALIDATIONDIALOG_H
