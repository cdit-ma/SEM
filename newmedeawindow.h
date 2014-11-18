#ifndef NEWMEDEAWINDOW_H
#define NEWMEDEAWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableView>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

namespace Ui {
class NewMedeaWindow;
}


class NewMedeaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NewMedeaWindow(QWidget *parent = 0);
    ~NewMedeaWindow();

private:
    Ui::NewMedeaWindow *ui;
    QGraphicsView *view;
    QGraphicsScene *scene;
    QGroupBox *box;
    QLabel *projectName;
    QLineEdit *searchBar;
    QPushButton *searchButton;
    QPushButton *assemblyButton;
    QPushButton *workloadButton;
    QPushButton *definitionsButton;
    QTableView *dataTable;
};

#endif // NEWMEDEAWINDOW_H
