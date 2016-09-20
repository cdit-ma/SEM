#include "filehandler.h"
#include "settingscontroller.h"
#include "../View/theme.h"
#include <QDateTime>
#include <QTextStream>
#include <QStringBuilder>
#include <QDebug>
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
            _notification(NT_CRITICAL, "File Read Error", "File: '" % fileInfo.absoluteFilePath() % "' cannot be read!", "Actions", "File");
        }
    }else{
        _notification(NT_CRITICAL, "File Read Error", "File: '" % fileInfo.absoluteFilePath() % "' doesn't be read!", "Actions", "File");
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
            _notification(NT_CRITICAL, "File Write Error", "File: '" % fileInfo.absoluteFilePath() % "' cannot be written! Permission denied.", "Actions", "Save");
            return false;
        }
    }else{
        return false;
    }
    _notification(NT_CRITICAL, "File Write Error", "File: '" % fileInfo.absoluteFilePath() % "' written!", "Actions", "Save");
    return true;
}

bool FileHandler::ensureDirectory(QString path)
{
    QFile file(path);
    QFileInfo fileInfo(file);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        if(dir.mkpath(".")){
            _notification(NT_INFO, "Directory", "Dir: '" % dir.absolutePath() % "' constructed!", "Actions", "Open");
        }else{
            _notification(NT_CRITICAL, "Directory", "Dir: '" % dir.absolutePath() % "' cannot be constructed!", "Actions", "Open");
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

void FileHandler::_notification(NOTIFICATION_TYPE type, QString notificationTitle, QString notificationText, QString iconPath, QString iconName)
{
    emit getFileHandler()->notification(type, notificationTitle, notificationText, iconPath, iconName, -1);
}
