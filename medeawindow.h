#ifndef MEDEAWINDOW_H
#define MEDEAWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include "GUI/projectwindow.h"
#include "GUI/attributetablemodel.h"
#include <QProcess>
#include "GUI/filterbutton.h"
namespace Ui {
class MedeaWindow;
}

class MedeaWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MedeaWindow(QString graphMLFile=0, QWidget *parent = 0);
    ~MedeaWindow();


signals:
    void view_PasteData(QString value);

    void view_TriggerAction(QString name);
    void enableGUI(bool isEnable);
    void view_AddFilter(QString filter);
    void view_AddAspect(QString aspect);
    void view_ClearFilters();
    void view_ClearAspects();
    void view_AspectsVisible(QStringList aspects);
    void view_ImportGraphML(QStringList files);
    void view_ExportGraphML(QString fileName);

private slots:
    void setupJenkinsSettings();
    void updateStatusText(QString statusText);

    void updateFilterButtons(QVector<FilterButton*> buttons);
    void updateAspectButtons(QVector<FilterButton*> buttons);

    void writeExportedGraphMLData(QString fileName, QString data);

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


    void getJenkinsNodes();
    void on_actionPaste_triggered();
    void setEnableGUI(bool enable);

    void on_deleteData_clicked();

    void on_actionExport_GraphML_triggered();

    void on_nodeIDCombo_activated(const QString &arg1);
    void loadJenkinsData(int code);

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

private:
    void importGraphMLFiles(QStringList filePaths);
    QProcess *myProcess;
    QString DEPGEN_ROOT;
    QString JENKINS_ADDRESS;
    QString JENKINS_USERNAME;
    QString JENKINS_PASSWORD;

    QProgressBar* currentOperationBar;
    void setSelectedProject(ProjectWindow* newSelected);
    Ui::MedeaWindow *ui;
    QVector<ProjectWindow*> projectWindows;
    ProjectWindow* getProjectWindow(QObject *object);
    ProjectWindow* selectedProject;
    bool isEnabled;
};

#endif // MEDEAWINDOW_H
