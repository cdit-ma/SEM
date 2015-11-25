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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/Actions/MEDEA.png"));

    QCoreApplication::setApplicationName("MEDEA");
    QCoreApplication::setApplicationVersion("19");
    QCoreApplication::setOrganizationName("Defence Information Group");
    QCoreApplication::setOrganizationDomain("http://blogs.adelaide.edu.au/dig/");

    int fontID = QFontDatabase::addApplicationFont(":/Resources/Fonts/OpenSans-Regular.ttf");

    QString fontName = QFontDatabase::applicationFontFamilies(fontID).at(0);

    QFont font = QFont(fontName);
    //QFont font = QFont("Verdana");
    font.setPointSizeF(8.5);
    a.setFont(font);

    QString GraphMLFile = "";
    if (argc == 2) {
        GraphMLFile = QString(argv[1]);
    }

    MedeaWindow *w = new MedeaWindow(GraphMLFile);
    w->show();

    //ModelTester* t = new ModelTester();

   //return 0;
    return a.exec();
}
