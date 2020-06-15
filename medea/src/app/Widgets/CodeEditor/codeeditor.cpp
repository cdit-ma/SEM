#include <QtWidgets>
#include <QFont>

#include "codeeditor.h"
#include "../../theme.h"

CodeEditor::CodeEditor(QWidget *parent) 
	: QPlainTextEdit(parent)
{
    //Construct areas.
    lineNumberArea = new LineNumberArea(this);
    highlighter = new SyntaxHighlighter(document());

    auto gotoLine_action = new QAction("Go to Line", this);
    gotoLine_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    connect(gotoLine_action, SIGNAL(triggered(bool)), this, SLOT(gotoLine()));
	
	//setTabStopDistance(24);
	
    QFont font("courier");
    font.setPointSize(12);
    setFont(font);

    addAction(gotoLine_action);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::matchParentheses);

    updateLineNumberAreaWidth();

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

int CodeEditor::lineNumberAreaWidth()
{
    QString lines = QString::number(blockCount() + 1);
    return 10 + fontMetrics().horizontalAdvance(lines);
}

void CodeEditor::updateLineNumberAreaWidth()
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::gotoLine()
{
    bool ok = false;
    int number = QInputDialog::getInt(this, "Goto Line", "Please input a Line number:", 0, 0, blockCount(),1, &ok);
    if(ok){
        QTextCursor cursor(document()->findBlockByNumber(number - 1));
        setTextCursor(cursor);
    }
}

// walk through and check that we don't exceed 80 chars per line
void CodeEditor::matchParentheses()
{
    highlights.clear();

    QTextBlock currentBlock = textCursor().block();
    auto data = dynamic_cast<TextBlockData *>(currentBlock.userData());
    if (data) {
        QVector<ParenthesisInfo *> brackets = data->parentheses();

        int blockPos = textCursor().block().position();
        int currentPos = textCursor().position() - blockPos;

        for(int i = 0; i < brackets.length(); i++){
            ParenthesisInfo* b = brackets.value(i, nullptr);
            if(b && (currentPos - 1 == b->position || currentPos == b->position)){
                createParenthesisSelection(currentBlock.position() + b->position);
                matchOtherParenthesis(b, currentBlock, i);
                break;
            }
        }
    }

    setExtraSelections(highlights);
}

void CodeEditor::themeChanged()
{
    Theme* t = Theme::theme();
    setStyleSheet(t->getWidgetStyleSheet("QPlainTextEdit")
                  +
                  "QInputDialog {"
                  "background:" + t->getBackgroundColorHex() + ";"
                  "color:" + t->getTextColorHex() + ";"
                  "}"
                  "QInputDialog QLabel {"
                  "background:" + t->getBackgroundColorHex() + ";"
                  "color:" + t->getTextColorHex() + ";"
                  "}"
                  "QInputDialog QPushButton {"
                  "padding: 5px;"
                  "}"
                  );

    textColor = t->getTextColor();
    selectedTextColor = t->getTextColor(ColorRole::SELECTED);
    altBackgroundColor = t->getAltBackgroundColor();
    selectedColor = t->getHighlightColor();
    highlighter->rehighlight();
}

bool CodeEditor::matchOtherParenthesis(ParenthesisInfo *bracket, QTextBlock currentBlock, int index, int openCount)
{
	// TODO: This looks very similar to matchParentheses; check both functions and see how they can be improved
    auto data = dynamic_cast<TextBlockData *>(currentBlock.userData());
    QVector<ParenthesisInfo *> brackets = data->parentheses();

    if(index == -1){
        index = bracket->isOpening() ? 0 : brackets.size() -1;
    }

    //Look forward if we have an open bracket, else backwards
    for(int i = index;(i >= 0 && i < brackets.size()); i += bracket->isOpening() ? 1 : -1){
        ParenthesisInfo* b = brackets.value(i, nullptr);
        if(bracket != b){
            if(bracket->character == b->character){
                //Got the same bracket, we need to match this first.
                openCount ++;
            }else if(bracket->getMatching() == b->character){
                //If we have a matching bracket, we need to be at 0 open brackets left to accept it.
                if(openCount == 0){
                    createParenthesisSelection(currentBlock.position() + b->position);
                    return true;
                }else{
                    openCount --;
                }
            }
        }
    }

    if(bracket->isOpening()){
        currentBlock = currentBlock.next();
    }else{
        currentBlock = currentBlock.previous();
    }

    if(currentBlock.isValid()){
        return matchOtherParenthesis(bracket, currentBlock, -1, openCount);
    }
    return false;
}

void CodeEditor::createParenthesisSelection(int pos)
{
    QTextEdit::ExtraSelection selection;
    QTextCharFormat format = selection.format;
    format.setBackground(selectedColor);
    format.setForeground(selectedTextColor);

    QFont boldFont;
    boldFont.setBold(true);
    format.setFont(boldFont);
    selection.format = format;

    QTextCursor cursor = textCursor();
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    selection.cursor = cursor;

    highlights.append(selection);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);

    painter.setPen(Qt::NoPen);
    painter.setBrush(altBackgroundColor);
    painter.drawRect(event->rect());

    QFont font = this->font();
    QTextBlock block = firstVisibleBlock();

    while (block.isValid()){
        QString number = QString::number(block.blockNumber() + 1);

        QRectF br = blockBoundingGeometry(block);
        br.setWidth(lineNumberAreaWidth());
        br.translate(contentOffset());

        if(block.blockNumber() == textCursor().blockNumber()){
            font.setBold(true);
            painter.setPen(selectedTextColor);
        } else {
            font.setBold(false);
            painter.setPen(textColor);
        }
        painter.setFont(font);
        painter.drawText(br, Qt::AlignCenter, number);
        block = block.next();
    }
}