#include <QApplication>
#include <QtDebug>

#include "mainwindow.h"

#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FreeConsole();
    MainWindow* window = new MainWindow();
    window->show();
    return a.exec();
}
