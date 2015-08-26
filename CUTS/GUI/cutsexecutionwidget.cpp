#include "cutsexecutionwidget.h"
#include "../CUTS.h"


#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <QProgressBar>
#include <QAction>
#include <QMenu>
#include <QProgressDialog>
#include <QDebug>

#include <QFileDialog>


CUTSExecutionWidget::CUTSExecutionWidget(QWidget *parent, CUTS *cutsManager)
{
    this->cutsManager = cutsManager;
    graphmlPathEdit = 0;
    outputPathEdit = 0;

    setWindowTitle("Launch CUTS Execution");
    setWindowIcon(QIcon(":/Resources/Icons/jenkins_build.png"));
    setupLayout("MODEL");


    setStyleSheet("font-family: Helvetica, Arial, sans-serif; background-color:white;  font-size: 13px; color: #333;");

    //Turn off the Other Buttons.
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));

    connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

CUTSExecutionWidget::~CUTSExecutionWidget()
{

}

void CUTSExecutionWidget::setGraphMLPath(QString path)
{
    //Check file.
    QFile file(path);
    QFileInfo fileInfo(file);

    bool isGraphML = false;
    bool isFile = fileInfo.isFile();
    if(path.endsWith(".graphml")){
        isGraphML = true;
    }

    if(isGraphML && isFile){
        graphmlPathEdit->setText(fileInfo.absoluteFilePath());
        setIconSuccess(graphmlPathIcon, true);
    }else{
        if(fileInfo.isDir()){
            graphmlPathEdit->setText("");
        }else{
            graphmlPathEdit->setText(fileInfo.absoluteFilePath());
        }

        setIconSuccess(graphmlPathIcon, false);
    }
}

void CUTSExecutionWidget::setOutputPath(QString outputPath)
{
    //Check file.
    QFile file(outputPath);
    QFileInfo fileInfo(file);

    QString path = fileInfo.absoluteFilePath();
    if(!path.endsWith("/")){
        path += "/";
    }
    outputPathEdit->setText(path);
    if(fileInfo.isDir()){
        setIconSuccess(outputPathIcon, true);
    }else{

        setIconSuccess(outputPathIcon, false);
    }
}

void CUTSExecutionWidget::outputPathEdited()
{
    setOutputPath(outputPathEdit->text());
}

void CUTSExecutionWidget::graphmlPathEdited()
{
    setGraphMLPath(graphmlPathEdit->text());
}

void CUTSExecutionWidget::selectGraphMLPath()
{
    QString directory = getDirectory(graphmlPathEdit->text());
    QString graphmlFile = QFileDialog::getOpenFileName(this, "Select GraphML File.", directory, "GraphML Documents (*.graphml)");
    setGraphMLPath(graphmlFile);
}

void CUTSExecutionWidget::selectOutputPath()
{
    QString directory = getDirectory(outputPathEdit->text());
    QString path = QFileDialog::getExistingDirectory(this, "Select Output Directory.", directory, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    setOutputPath(path);

}

void CUTSExecutionWidget::setupLayout(QString modelName)
{
    //Construct a Vertical Layout.
    QVBoxLayout* verticalLayout = new QVBoxLayout(this);

    //Setup Title.
    titleWidget = new QWidget();
    QHBoxLayout* titleLayout = new QHBoxLayout(titleWidget);
    verticalLayout->addWidget(titleWidget);

    //Set up a QLabel for the name of the Jenkins Job
    QLabel* jobLabel = new QLabel("CUTS - " + modelName);
    QLabel* iconLabel = new QLabel();

    iconLabel->setPixmap(QPixmap::fromImage(QImage(":/Resources/Icons/jenkins_build.png")));

    jobLabel->setStyleSheet("font-family: Helvetica, Arial, sans-serif; font-size: 18px;  font-weight: bold;");

    titleLayout->addWidget(iconLabel);
    titleLayout->addWidget(jobLabel);
    titleLayout->addStretch();


    QHBoxLayout* modelLayout = new QHBoxLayout();
    QLabel* modelLabel = new QLabel("Model Path");
    graphmlPathIcon = new QLabel();
    graphmlPathIcon->setFixedSize(25,25);
    graphmlPathIcon->setScaledContents(true);
    graphmlPathEdit = new QLineEdit();
    setIconSuccess(graphmlPathIcon, false);

    QPushButton* modelSelector = new QPushButton("...");
    connect(modelSelector, SIGNAL(clicked()), this, SLOT(selectGraphMLPath()));
    modelLayout->addWidget(graphmlPathIcon);
    modelLayout->addWidget(modelLabel);
    modelLayout->addWidget(graphmlPathEdit, 1);
    connect(graphmlPathEdit, SIGNAL(editingFinished()), this, SLOT(graphmlPathEdited()));
    modelLayout->addWidget(modelSelector);



    verticalLayout->addLayout(modelLayout);

    QHBoxLayout* outputLayout = new QHBoxLayout();
    QLabel* outputLabel = new QLabel("Output Path");
    outputPathIcon = new QLabel();
    outputPathIcon->setFixedSize(25,25);
    outputPathIcon->setScaledContents(true);
    outputPathEdit = new QLineEdit();
    setIconSuccess(outputPathIcon, false);

    QPushButton* outputSelector = new QPushButton("...");
    connect(outputSelector, SIGNAL(clicked()), this, SLOT(selectOutputPath()));
    outputLayout->addWidget(outputPathIcon);
    outputLayout->addWidget(outputLabel);
    outputLayout->addWidget(outputPathEdit, 1);
    connect(outputPathEdit, SIGNAL(editingFinished()), this, SLOT(outputPathEdited()));
    outputLayout->addWidget(outputSelector);



    verticalLayout->addLayout(outputLayout);

}

QString CUTSExecutionWidget::getDirectory(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo(file);
    return fileInfo.absolutePath();
}

void CUTSExecutionWidget::setIconSuccess(QLabel *label, bool success)
{
    QString fileName = "cross";
    if(success){
        fileName = "tick";
    }
    if(label){
        label->setPixmap(QPixmap::fromImage(QImage(":/Resources/Icons/" + fileName + ".png")));
    }
}
