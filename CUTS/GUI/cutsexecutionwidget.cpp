#include "cutsexecutionwidget.h"
#include "../cutsmanager.h"


#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <QLayout>
#include <QProgressBar>
#include <QAction>
#include <QMenu>
#include <QProgressDialog>
#include <QDebug>

#include <QFileDialog>


CUTSExecutionWidget::CUTSExecutionWidget(QWidget *parent, CUTSManager *cutsManager)
{
    this->cutsManager = cutsManager;
    graphmlPathEdit = 0;
    outputPathEdit = 0;
    generateButton = 0;
    graphmlPathOk = false;
    outputPathOk = false;

    setWindowTitle("Launch CUTS Execution");
    setWindowIcon(QIcon(":/Resources/Icons/jenkins_build.png"));
    setupLayout("MODEL");

    connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
    setModal(true);

    setStyleSheet("font-family: Helvetica, Arial, sans-serif; background-color:white;  font-size: 13px; color: #333;");

    //Turn off the Other Buttons.
    setWindowFlags(windowFlags() & (~Qt::WindowContextHelpButtonHint));

    connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
    connect(cutsManager, SIGNAL(generatingFile(QString)), this, SLOT(fileToBeGenerated(QString)));
    connect(cutsManager, SIGNAL(generatedFile(QString,bool)), this, SLOT(fileGenerated(QString, bool)));

    this->setGraphMLPath("/Users/dan/Desktop/model.graphml");
    this->setOutputPath("/Users/dan/Desktop/Test3/");

    loadingMovie = new QMovie(this);
    loadingMovie->setFileName(":/Resources/Icons/jenkins_waiting.gif");
    loadingMovie->start();
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
        graphmlPathOk = true;
    }else{
        if(fileInfo.isDir()){
            graphmlPathEdit->setText("");
        }else{
            graphmlPathEdit->setText(fileInfo.absoluteFilePath());
        }

        setIconSuccess(graphmlPathIcon, false);
        graphmlPathOk = false;
    }

    updateButtons();
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
        outputPathOk = true;
    }else{
        setIconSuccess(outputPathIcon, false);
        outputPathOk = false;
    }
    updateButtons();
}

void CUTSExecutionWidget::runGeneration()
{
    if(cutsManager){
        cutsManager->runTransforms(graphmlPathEdit->text(), outputPathEdit->text());
    }
}

void CUTSExecutionWidget::fileToBeGenerated(QString filePath)
{
    files.append(filePath);
    addFileToLayout(filePath);
}

void CUTSExecutionWidget::fileGenerated(QString filePath, bool success)
{

    FileExtension fE = fileExtensionLayouts[getExtension(filePath)];

    fE.filesGenerated.append(filePath);

    if(!success){
        fE.allSuccess = false;
    }

    if(fE.filesGenerated.size() == fE.files.size()){
        setIconSuccess(fE.icon, fE.allSuccess);
    }

    //Update fileCount
    fE.finishedLabel->setText(QString::number(fE.filesGenerated.count()));

    //Update the Layouts
    fileExtensionLayouts[getExtension(filePath)] = fE;

    //Check
    filesGenerated.append(filePath);
}

void CUTSExecutionWidget::outputPathEdited()
{
    setOutputPath(outputPathEdit->text());
}

void CUTSExecutionWidget::graphmlPathEdited()
{
    setGraphMLPath(graphmlPathEdit->text());
}

QString CUTSExecutionWidget::getExtension(QString filePath)
{
    return filePath.split(".").last();
}

void CUTSExecutionWidget::updateButtons()
{
    enableGenerateButton(graphmlPathOk && outputPathOk);
}

void CUTSExecutionWidget::addFileToLayout(QString filePath)
{
    //Get File Extension
    QString extension = getExtension(filePath);

    FileExtension fE;

    if(!fileExtensionLayouts.contains(extension)){
        fE.allSuccess = true;
        fE.layout = new QHBoxLayout();
        fE.icon = new QLabel();
        fE.finishedLabel = new QLabel("0");
        fE.slashLabel = new QLabel("/");
        fE.totalLabel = new QLabel();
        fE.label = new QLabel("*." + extension + " Files");
        fE.icon = new QLabel();
        fE.icon->setFixedSize(16,16);
        fE.icon->setScaledContents(true);

        fE.icon->setMovie(loadingMovie);

        fE.layout->addWidget(fE.icon);
        fE.layout->addWidget(fE.finishedLabel);
        fE.layout->addWidget(fE.slashLabel);
        fE.layout->addWidget(fE.totalLabel);
        fE.layout->addWidget(fE.label,1);

        fileLayout->addLayout(fE.layout);
        fileExtensionLayouts[extension] = fE;
    }
    fE = fileExtensionLayouts[extension];

    fE.files.append(filePath);
    //Update fileCount
    fE.totalLabel->setText(QString::number(fE.files.count()));
    //Update the Layouts
    fileExtensionLayouts[extension] = fE;
}

void CUTSExecutionWidget::enableGenerateButton(bool enabled)
{
    if(generateButton){
        generateButton->setEnabled(enabled);
    }
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

    generateButton = new QPushButton("Generate Files");
    verticalLayout->addWidget(generateButton);
    connect(generateButton, SIGNAL(clicked()), this, SLOT(runGeneration()));
    enableGenerateButton(false);

    generatedFilesBox = new QGroupBox("Generated Files");
    fileLayout = new QVBoxLayout();
    generatedFilesBox->setLayout(fileLayout);

    verticalLayout->addWidget(generatedFilesBox);


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
