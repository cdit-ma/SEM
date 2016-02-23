#include <QApplication>
#include <QtDebug>
#include <QObject>

#include <QFont>
#include "medeawindow.h"
#include "modeltester.h"

#include <QString>
#include <QRegExp>
#include <QDebug>
#include <QProcess>

#include <QList>
#include <QPair>
#include <string>

//ICON_LINK
//https://www.google.com/design/icons/index.html

/**
 * @brief main Main Yo
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    //Construct a QApplication
    //QApplication a(argc, argv);

    //QString graphmlFilePath = "";
    //if (argc == 2) {
    //    graphmlFilePath = QString(argv[1]);
    //}

    //MedeaWindow *w = new MedeaWindow(graphmlFilePath);
    //a.setActiveWindow(w);
    ModelTester *t = new ModelTester();

    return 0;
    //return a.exec();
}
