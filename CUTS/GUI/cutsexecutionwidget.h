#ifndef CUTSEXECUTIONWIDGET_H
#define CUTSEXECUTIONWIDGET_H
#include "../cutsmanager.h"

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMovie>
#include <QGroupBox>
#include <QTabWidget>
#include <QScrollArea>
#include <QTextBrowser>
#include <QTime>
struct FileExtension{
    QString extension;
    QStringList files;
    QStringList filesGenerated;

    QHBoxLayout* layout;
    QLabel* icon;
    QLabel* finishedLabel;
    QLabel* totalLabel;
    QLabel* slashLabel;
    QLabel* label;
    bool allSuccess;
};
enum CUTS_EXECUTION_STATES {INITIAL, PARAMETERS_OKAY, RUN_XSL, RAN_XSL, RUN_MWC, RAN_MWC, RUN_MAKE, RAN_MAKE, RUN_EXECUTION, RAN_EXECUTION, RERUN_EXECUTION};

//Todo
//Deal with failure of states better.
//Start logging servers

class CUTSExecutionWidget: public QDialog
{
    Q_OBJECT
public:
    CUTSExecutionWidget(QWidget *parent = 0, CUTSManager* cutsManager=0);
    ~CUTSExecutionWidget();




public slots:
    void stateFinished(int code);
    void selectGraphMLPath();
    void selectOutputPath();
    void setGraphMLPath(QString path);
    void setOutputPath(QString outputPath);
    void runGeneration();

    void nextButtonPressed();
    void fileToBeGenerated(QString filePath);
    void fileGenerated(QString filePath, bool success);

    void generationFinished(bool success, QString errorString);
    void gotLiveMWCOutput(QString data);
    void gotLiveMakeOutput(QString data);
    void gotLiveCUTSOutput(QString data);

    void outputPathEdited();
    void graphmlPathEdited();

    void tick();
private:
    QWidget* setupGenerateWidget();
    QWidget* setupBuildWidget();
    QWidget* setupExecuteWidget();

    QString getExtension(QString filePath);
    void updateButtons();

    void stepState();


    void setState(CUTS_EXECUTION_STATES newState);

    void addFileToLayout(QString filePath);





    void setupLayout();
    QString getDirectory(QString filePath);

    void setIconSuccess(QLabel* label, bool success);
    void updateTimeText(QTime time);
    void setIconLoading(QLabel* label);


    //Top Section is contained in a titleWidget.
    QWidget* titleWidget;
    QWidget* buttonWidget;

    bool graphmlPathOk;
    bool outputPathOk;


    QHash<QString, FileExtension> fileExtensionLayouts;
    QStringList files;
    QStringList filesGenerated;

    QLineEdit* graphmlPathEdit;
    QPushButton* graphmlPathButton;
    QPushButton* outputPathButton;
    QLineEdit* outputPathEdit;
    QLabel* graphmlPathIcon;
    QLabel* outputPathIcon;



    //Used to store the buttons
    QPushButton* nextButton;
    QPushButton* cancelButton;

    QTabWidget* tabWidget;

    //Used to store the Widgets of the page.
    QScrollArea* generateScrollArea;
    QWidget* generateWidget;
    QScrollArea* buildScrollArea;
    QWidget* buildWidget;
    QScrollArea* executeScrollArea;
    QWidget* executeWidget;

    CUTS_EXECUTION_STATES state;

    QMovie* loadingMovie;
    QVBoxLayout* generateLayout;
    QTextBrowser* executeText;
    CUTSManager* cutsManager;

    QLabel* jobLabel;

    QLabel* mwcIcon;
    QLabel* mwcLabel;
    QTime  mwcStartTime;
    QTextBrowser* mwcTextBrowser;

    QLabel* makeIcon;
    QLabel* makeLabel;
    QTextBrowser* makeTextBrowser;


    QLabel* currentTaskTime;

    QTime currentTime;
    QTimer* currentTimer;
};



#endif // CUTSEXECUTIONWIDGET_H
