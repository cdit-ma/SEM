#ifndef MEDEAWINDOW_H
#define MEDEAWINDOW_H

#include <QMainWindow>
#include "GUI/projectwindow.h"
#include "GUI/attributetablemodel.h"

namespace Ui {
class MedeaWindow;
}

class MedeaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MedeaWindow(QWidget *parent = 0);
    ~MedeaWindow();


signals:
    void view_PasteData(QString value);
    void view_ActionTriggered(QString name);

private slots:
    void on_actionImport_GraphML_triggered();

    void on_actionNew_Project_triggered();

    void windowClosed(QObject* window);

    void projectWindowSelected(QMdiSubWindow* window);
    void setAttributeModel(AttributeTableModel* model);

    void updateUndoStates(QStringList list);
    void updateRedoStates(QStringList list);
    void setClipboard(QString value);

    void on_actionPaste_triggered();



private:

    void setSelectedProject(ProjectWindow* newSelected);
    Ui::MedeaWindow *ui;
    QVector<ProjectWindow*> projectWindows;
    ProjectWindow* getProjectWindow(QObject *object);
    ProjectWindow* selectedProject;
};

#endif // MEDEAWINDOW_H
