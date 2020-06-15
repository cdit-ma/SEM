#include "codebrowser.h"

CodeBrowser::CodeBrowser(QWidget *parent)
	: QWidget(parent)
{
	setupLayout();
}

void CodeBrowser::closeTab(int tabID)
{
    QWidget* content = tabWidget->widget(tabID);
    tabWidget->removeTab(tabID);
    if (content) {
        auto editor = qobject_cast<CodeEditor*>(content);
        if (editor) {
        	const auto& filename = codeEditorLookup.key(editor, "");
        	codeEditorLookup.remove(filename);
        	editor->deleteLater();
        } else {
            delete content;
        }
    }
    // Hide the parent widget
    if (parentWidget() && tabWidget->count() == 0) {
        parentWidget()->hide();
    }
}

void CodeBrowser::showCode(const QString& fileName, const QString& content, bool editable)
{
    CodeEditor* codeEditor = codeEditorLookup.value(fileName, nullptr);
    if (codeEditor == nullptr) {
        codeEditor = new CodeEditor(this);
        codeEditor->setPlainText(content);
        tabWidget->addTab(codeEditor, fileName);
        codeEditorLookup.insert(fileName, codeEditor);
    }

	codeEditor->setReadOnly(!editable);
	codeEditor->setPlainText(content);
}

void CodeBrowser::setupLayout()
{
	tabWidget = new QTabWidget(this);
	
	layout = new QVBoxLayout(this);
    layout->addWidget(tabWidget, 1);
    
    tabWidget->setTabsClosable(true);
    tabWidget->setElideMode(Qt::ElideMiddle);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &CodeBrowser::closeTab);
}