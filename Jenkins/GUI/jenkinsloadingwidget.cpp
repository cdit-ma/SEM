#include "jenkinsloadingwidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <QProgressBar>

/**
 * @brief JenkinsLoadingWidget::JenkinsLoadingWidget Construct a Widget which contains the Jenkins Logo and a Loading Jenkins Progress Bar.
 * @param parent The parent this widget is attached to.
 */
JenkinsLoadingWidget::JenkinsLoadingWidget(QWidget *parent):QWidget(parent)
{    
    //Construct a Layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    //Construct a Widget for the Icons.
    iconBar = new QWidget();
    iconBar->setStyleSheet("background-color:#333;margin:2px;");

    QHBoxLayout* titleLayout = new QHBoxLayout(iconBar);
    titleLayout->setSpacing(1);
    titleLayout->setMargin(1);

    //Get the Jenkins Logo and scale.
    QImage jenkinsLogo(":/Resources/Icons/jenkins.png");
    QImage jenkinsText(":/Resources/Icons/jenkins_label.png");


    //Set the size to match the Jenkins Webpage Sizes.
    jenkinsLogo = jenkinsLogo.scaled(36,36, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    jenkinsText = jenkinsText.scaled(139,36, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    //Setup a QLabel to hold the pixmap for the Logo
    QLabel* jenkinsIcon = new QLabel();
    jenkinsIcon->setPixmap(QPixmap::fromImage(jenkinsLogo));

    //Setup a QLabel to hold the pixmap for the Text
    QLabel* jenkinsLabel = new QLabel();
    jenkinsLabel->setPixmap(QPixmap::fromImage(jenkinsText));

    //Add QLabels to the title layout.
    titleLayout->addWidget(jenkinsIcon);
    titleLayout->addWidget(jenkinsLabel);
    titleLayout->addStretch(0);

    layout->addWidget(iconBar);

    loadingBar = new QWidget();
    QVBoxLayout* loadingLayout = new QVBoxLayout(loadingBar);
    //Add the title Widget to the Vertical Layout.

    //Set up a Label.
    loadingLabel = new QLabel();
    loadingLabel->setStyleSheet("font-weight: bold;");

    //Set up a Progress Bar
    QProgressBar* progressBar = new QProgressBar();
    progressBar->setTextVisible(false);
    progressBar->setMinimum(0);
    progressBar->setMaximum(0);


    //Add them to this Widget.
    loadingLayout->addWidget(loadingLabel,0, Qt::AlignCenter);
    loadingLayout->addWidget(progressBar);
    layout->addWidget(loadingBar);
    layout->addStretch();
    //Set waiting for the Validation
    setWaiting(false);
}

void JenkinsLoadingWidget::setWaiting(bool waiting)
{
    if(waiting){
        loadingLabel->setText("Waiting for Jenkins CLI...");
    }else{
        loadingLabel->setText("Validating Jenkins Settings...");
    }

}

/**
 * @brief JenkinsLoadingWidget::hideLoadingBar Hide the Loading bar.
 */
void JenkinsLoadingWidget::hideLoadingBar()
{
    if(loadingBar){
        loadingBar->hide();
    }
}

void JenkinsLoadingWidget::authenticationFinished()
{
    setWaiting(true);
}
