#ifndef CODEBROWSER_H
#define CODEBROWSER_H

#include "codeeditor.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget>

class CodeBrowser : public QWidget
{
    Q_OBJECT
    
public:
    explicit CodeBrowser(QWidget *parent = nullptr);
    
public slots:
    void showCode(const QString& fileName, const QString& content, bool editable);

private slots:
    void closeTab(int tabID);

private:
    void setupLayout();
    
    QVBoxLayout* layout = nullptr;
    QTabWidget* tabWidget = nullptr;
    
    QHash<QString, CodeEditor*> codeEditorLookup;
};

#endif // CODEBROWSER_H