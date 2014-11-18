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
#include <QTextEdit>
#include <QMenu>
#include <QAction>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

#include <QDebug>
#include <QClipboard>
#include <QItemSelectionModel>

#include "GUI/projectwindow.h"
#include "GUI/attributetablemodel.h"
#include "GUI/filterbutton.h"

namespace Ui {
class NewMedeaWindow;
}

class NewMedeaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NewMedeaWindow(QWidget *parent = 0);
    ~NewMedeaWindow();


signals:
    void view_PasteData(QString value);
    /**
    void view_ActionTriggered(QString name);
    void enableGUI(bool isEnable);
    void view_AddFilter(QString filter);
    void view_AddAspect(QString aspect);
    void view_ClearFilters();
    void view_ClearAspects();
    void view_AspectsVisible(QStringList aspects);
    void view_ImportGraphML(QStringList files);
    void view_ExportGraphML(QString fileName);
    */

private slots:

    void setClipboard(QString value);
    void on_actionPaste_triggered();


/**
protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
*/

private:
    void setSelectedProject(ProjectWindow* newSelected);
    ProjectWindow* getProjectWindow(QObject *object);

    QVector<ProjectWindow*> projectWindows;
    ProjectWindow* selectedProject;
    bool isEnabled;

    Ui::NewMedeaWindow *ui;
    QGraphicsView *view;
    QGraphicsScene *scene;
    QGroupBox *box;
    QLabel *projectName;
    QPushButton *menuButton;
    QLineEdit *searchBar;
    QPushButton *searchButton;
    QPushButton *assemblyButton;
    QPushButton *workloadButton;
    QPushButton *definitionsButton;
    QTableView *dataTable;
    QTextEdit *notificationArea;

    QMenu *menu;
    QAction *menu_newProject;
    QAction *menu_importGraphML;
};

#endif // NEWMEDEAWINDOW_H
