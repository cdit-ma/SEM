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

    static QString selectFile(QWidget* parent, QString windowTitle, QFileDialog::FileMode fileMode, bool write, QString nameFilter = "", QString defaultSuffix = "", QString initialFile = "");
    static QStringList selectFiles(QWidget* parent, QString windowTitle, QFileDialog::FileMode fileMode, bool write, QString nameFilter = "", QString defaultSuffix = "",  QString initialFile = "");

    static QString readTextFile(QString filePath);

    static bool isFileReadable(QString filePath);
    static bool writeTextFile(QString filePath, QString fileData, bool notify = true);
    static QString writeTempTextFile(QString fileData, QString extension="");

    static QString getTempFileName(QString suffix, bool timestamp = true);
    static void sanitizeFilePath(QString &filePath);

    static bool ensureDirectory(QString path);
    static bool removeDirectory(QString path);
    static bool removeFile(QString path);


    static QString getAutosaveFilePath(QString path);
    static bool isAutosaveFilePath(QString path);
    static QString getFileFromAutosavePath(QString autosave_path);

public:
    static FileHandler* getFileHandler();
private:
    static bool _writeTextFile(QString filePath, QString fileData, bool notify);

    static void _notification(Notification::Severity severity, QString notificationText, QString iconPath, QString iconName, bool toast=false);


    static QString getTimestamp();

    static QFileDialog* getFileDialog();
    QFileDialog* _getFileDialog();

    QFileDialog* fileDialog;
private:
    static FileHandler* handlerSingleton;
};

#endif // FILEHANDLER_H
