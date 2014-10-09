#ifndef MEDEAWINDOW_H
#define MEDEAWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include "GUI/projectwindow.h"
#include "GUI/attributetablemodel.h"
#include "GUI/filterbutton.h"
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
    void enableGUI(bool isEnable);
    void view_AddFilter(QString filter);
    void view_AddAspect(QString aspect);
    void view_ClearFilters();
    void view_ClearAspects();
    void view_AspectsVisible(QStringList aspects);
private slots:
    void updateFilterButtons(QVector<FilterButton*> buttons);
    void updateAspectButtons(QVector<FilterButton*> buttons);

    void updateProgressBar(int percentage, QString label);

    void on_actionImport_GraphML_triggered();

    void on_actionNew_Project_triggered();

    void windowClosed(QObject* window);

    void appendFilter();
    void appendAspect();
    void clearFilters();

    void projectWindowSelected(QMdiSubWindow* window);
    void setAttributeModel(AttributeTableModel* model);

    void updateUndoStates(QStringList list);
    void updateRedoStates(QStringList list);
    void setClipboard(QString value);

    void on_actionPaste_triggered();
    void setEnableGUI(bool enable);



private:
    QProgressBar* currentOperationBar;
    void setSelectedProject(ProjectWindow* newSelected);
    Ui::MedeaWindow *ui;
    QVector<ProjectWindow*> projectWindows;
    ProjectWindow* getProjectWindow(QObject *object);
    ProjectWindow* selectedProject;
    bool isEnabled;
};

#endif // MEDEAWINDOW_H
