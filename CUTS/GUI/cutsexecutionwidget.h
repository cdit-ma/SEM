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

class CUTSExecutionWidget: public QDialog
{
    Q_OBJECT
public:
    CUTSExecutionWidget(QWidget *parent = 0, CUTSManager* cutsManager=0);
    ~CUTSExecutionWidget();




public slots:
    void selectGraphMLPath();
    void selectOutputPath();
    void setGraphMLPath(QString path);
    void setOutputPath(QString outputPath);
    void runGeneration();

    void fileToBeGenerated(QString filePath);
    void fileGenerated(QString filePath, bool success);




    void outputPathEdited();
    void graphmlPathEdited();
private:
    QString getExtension(QString filePath);
    void updateButtons();

    void addFileToLayout(QString filePath);



    void enableGenerateButton(bool enabled);

    void setupLayout(QString modelName);
    QString getDirectory(QString filePath);

    void setIconSuccess(QLabel* label, bool success);


    //Top Section is contained in a titleWidget.
    QWidget* titleWidget;
    QWidget* buttonWidget;

    bool graphmlPathOk;
    bool outputPathOk;


    QHash<QString, FileExtension> fileExtensionLayouts;
    QStringList files;
    QStringList filesGenerated;

    QLineEdit* graphmlPathEdit;
    QLineEdit* outputPathEdit;
    QLabel* graphmlPathIcon;
    QLabel* outputPathIcon;
    QPushButton* generateButton;
    QGroupBox* generatedFilesBox;

    QMovie* loadingMovie;
    QVBoxLayout* fileLayout;
    CUTSManager* cutsManager;
};



#endif // CUTSEXECUTIONWIDGET_H
