#ifndef CUTSEXECUTIONWIDGET_H
#define CUTSEXECUTIONWIDGET_H
#include "../CUTS.h"

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>


class CUTSExecutionWidget: public QDialog
{
    Q_OBJECT
public:
    CUTSExecutionWidget(QWidget *parent = 0, CUTS* cutsManager=0);
    ~CUTSExecutionWidget();




public slots:
    void selectGraphMLPath();
    void selectOutputPath();
    void setGraphMLPath(QString path);
    void setOutputPath(QString outputPath);
    void runGeneration();




    void outputPathEdited();
    void graphmlPathEdited();
private:
    void updateButtons();



    void enableGenerateButton(bool enabled);

    void setupLayout(QString modelName);
    QString getDirectory(QString filePath);

    void setIconSuccess(QLabel* label, bool success);


    //Top Section is contained in a titleWidget.
    QWidget* titleWidget;
    QWidget* buttonWidget;

    bool graphmlPathOk;
    bool outputPathOk;

    QLineEdit* graphmlPathEdit;
    QLineEdit* outputPathEdit;
    QLabel* graphmlPathIcon;
    QLabel* outputPathIcon;
    QPushButton* generateButton;
    CUTS* cutsManager;
};



#endif // CUTSEXECUTIONWIDGET_H
