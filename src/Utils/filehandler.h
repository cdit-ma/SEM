#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <QObject>
#include <QFileDialog>
#include "../Controllers/NotificationManager/notificationmanager.h"

class FileHandler : public QObject
{
    Q_OBJECT
public:
    FileHandler();

    static QString selectFile(QString windowTitle, QFileDialog::FileMode fileMode, bool write, QString nameFilter = "", QString defaultSuffix = "", QString initialFile = "");
    static QStringList selectFiles(QString windowTitle, QFileDialog::FileMode fileMode, bool write, QString nameFilter = "", QString defaultSuffix = "",  QString initialFile = "");

    static QString readTextFile(QString filePath);

    static bool isFileReadable(QString filePath);
    static bool writeTextFile(QString filePath, QString fileData);
    static QString writeTempTextFile(QString fileData, QString extension="");

    static QString getTempFileName(QString suffix);
    static QString sanitizeFilePath(QString filePath);

public:
    static FileHandler* getFileHandler();
private:

    static void _notification(NOTIFICATION_SEVERITY severity, QString notificationText, QString iconPath, QString iconName);

    static bool ensureDirectory(QString path);

    static QString getTimestamp();

    static QFileDialog* getFileDialog();
    QFileDialog* _getFileDialog();

    QFileDialog* fileDialog;
private:
    static FileHandler* handlerSingleton;
};

#endif // FILEHANDLER_H
