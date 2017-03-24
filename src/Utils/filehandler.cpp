#include "filehandler.h"

#include <QDateTime>
#include <QTextStream>
#include <QStringBuilder>
#include <QDebug>

#include "../Controllers/SettingsController/settingscontroller.h"
#include "../theme.h"

FileHandler* handler = 0;

FileHandler::FileHandler():QObject()
{
    fileDialog = new QFileDialog(0);
    fileDialog->setModal(true);
    //Get Path.
    QString directory = SettingsController::settings()->getSetting(SK_GENERAL_MODEL_PATH).toString();
    fileDialog->setDirectory(directory);

}

QString FileHandler::selectFile(QString windowTitle, QFileDialog::FileMode fileMode, bool write, QString nameFilter, QString defaultSuffix, QString initialFile)
{
    QString file;
    QStringList files = selectFiles(windowTitle, fileMode, write, nameFilter, defaultSuffix, initialFile);
    if(files.length() == 1){
        file = files.at(0);
    }
    return file;
}

QStringList FileHandler::selectFiles(QString windowTitle, QFileDialog::FileMode fileMode, bool write, QString nameFilter, QString defaultSuffix, QString initialFile)
{
    QStringList files;
    QFileDialog* fd = getFileDialog();

    fd->setWindowTitle(windowTitle);
    fd->setNameFilter(nameFilter);
    fd->setFileMode(fileMode);
    fd->setConfirmOverwrite(write);
    fd->selectFile(initialFile);

    if(write){
        fd->setAcceptMode(QFileDialog::AcceptSave);
    }else{
        fd->setAcceptMode(QFileDialog::AcceptOpen);
    }


    if(fd->exec()){
        foreach(QString file, fd->selectedFiles()){
            if(!file.endsWith(defaultSuffix)){
                file.append(defaultSuffix);
            }
            if(!files.contains(file)){
                files.append(file);
            }
        }
    }
    return files;
}

QString FileHandler::readTextFile(QString filePath)
{
    QString fileData = "";
    QFile file(filePath);
    QFileInfo fileInfo(file);

    if(file.exists()){
        if(file.open(QFile::ReadOnly | QFile::Text)){
            QTextStream fileStream(&file);
            fileData = fileStream.readAll();
            file.close();
        }else{
            _notification(NS_ERROR, "File: '" % fileInfo.absoluteFilePath() % "' cannot be read!", "Icons", "file");
        }
    }else{
        _notification(NS_ERROR, "File: '" % fileInfo.absoluteFilePath() % "' cannot be read!", "Icons", "file");
    }
    return fileData;
}

bool FileHandler::isFileReadable(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo(file);
    return fileInfo.exists() && fileInfo.isReadable();
}

QString FileHandler::writeTempTextFile(QString fileData, QString extension)
{
    QString path = getTempFileName(extension);

    if(!writeTextFile(path, fileData)){
        path = "";
    }
    return path;
}

bool FileHandler::writeTextFile(QString filePath, QString fileData)
{
    QFile file(filePath);
    QFileInfo fileInfo(file);
    if(ensureDirectory(filePath)){
        if(file.open(QFile::WriteOnly | QFile::Text)){
            //Create stream to write the data.
            QTextStream out(&file);
            out << fileData;
            file.close();
        }else{
            _notification(NS_ERROR, "File: '" % fileInfo.absoluteFilePath() % "' cannot be written! Permission denied.", "Icons", "floppyDisk");
            return false;
        }
    }else{
        return false;
    }
    _notification(NS_INFO, "File: '" % fileInfo.absoluteFilePath() % "' written!", "Icons", "floppyDisk");
    return true;
}

bool FileHandler::ensureDirectory(QString path)
{
    QFile file(path);
    QFileInfo fileInfo(file);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if(dir.mkpath(".")){
            _notification(NS_INFO, "Dir: '" % dir.absolutePath() % "' constructed!", "Icons", "folder");
        }else{
            _notification(NS_ERROR, "Dir: '" % dir.absolutePath() % "' cannot be constructed!", "Icons", "folder");
            return false;
        }
    }
    return true;
}

QString FileHandler::getTempFileName(QString suffix)
{
    return QDir::tempPath() + "/" + getTimestamp() + suffix;
}

QString FileHandler::sanitizeFilePath(QString filePath)
{
    filePath = filePath.replace("\\","/");
    return filePath;
}

QString FileHandler::getTimestamp()
{
    QDateTime currentTime = QDateTime::currentDateTime();
    return currentTime.toString("yyMMdd-hhmmss");
}

QFileDialog *FileHandler::getFileDialog()
{
    return getFileHandler()->_getFileDialog();
}

QFileDialog *FileHandler::_getFileDialog()
{
    return fileDialog;
}

FileHandler *FileHandler::getFileHandler()
{
    if(!handler){
        handler = new FileHandler();
    }
    return handler;
}

void FileHandler::_notification(NOTIFICATION_SEVERITY severity, QString notificationText, QString iconPath, QString iconName)
{
    NotificationManager::manager()->displayNotification(notificationText, iconPath, iconName, -1, severity, NT_MODEL, NC_FILE);
}
