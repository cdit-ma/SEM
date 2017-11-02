#include "codebrowser.h"


CodeBrowser::CodeBrowser(QWidget *parent):QWidget(parent)
{
	setupLayout();
}

void CodeBrowser::closeTab(int tabID)
{
    QWidget* content = tabWidget->widget(tabID);
    tabWidget->removeTab(tabID);
    if(content){
        CodeEditor* editor = qobject_cast<CodeEditor*>(content);
        if(editor){
            QString fileName = getCodeEditorFileName(editor);
            codeEditorLookup.remove(fileName);
            editor->deleteLater();
        }else{
            delete content;
        }
    }

    //Hide!
    if(parentWidget() && tabWidget->count() == 0){
        parentWidget()->hide();
    }
}

void CodeBrowser::showCode(QString fileName, QString content, bool editable)
{
    CodeEditor* codeEditor = getCodeEditor(fileName);

    if(!codeEditor){
        codeEditor = new CodeEditor(this);
        //make tab width mode civilized
        codeEditor->setPlainText(content);
        
        tabWidget->addTab(codeEditor, fileName);
        codeEditorLookup.insert(fileName, codeEditor);
    }
    if(codeEditor){
        codeEditor->setReadOnly(!editable);
        codeEditor->setPlainText(content);
    }
}

void CodeBrowser::setupLayout()
{
    layout = new QVBoxLayout(this);

    tabWidget = new QTabWidget(this);
    layout->addWidget(tabWidget, 1);
    tabWidget->setTabsClosable(true);
    tabWidget->setElideMode(Qt::ElideMiddle);

    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &CodeBrowser::closeTab);
}

CodeEditor *CodeBrowser::getCodeEditor(QString fileName)
{
    return codeEditorLookup.value(fileName, 0);
}

QString CodeBrowser::getCodeEditorFileName(CodeEditor *editor)
{
    foreach(QString keyName, codeEditorLookup.keys()){
        if(editor && codeEditorLookup.value(keyName,0) == editor){
            return keyName;
        }
    }
    return QString();
}

