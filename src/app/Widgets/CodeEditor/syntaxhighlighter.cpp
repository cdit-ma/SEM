#include <QtGui>

#include "syntaxhighlighter.h"
#include "../../theme.h"

TextBlockData::TextBlockData()
{
    // Nothing to do
}

QVector<ParenthesisInfo *> TextBlockData::parentheses()
{
    return m_parentheses;
}

void TextBlockData::insert(ParenthesisInfo *info)
{
    int i = 0;
    while (i < m_parentheses.size() &&
        info->position > m_parentheses.at(i)->position)
        ++i;

    m_parentheses.insert(i, info);
}


//asdasd
SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    connect(Theme::theme(), &Theme::theme_Changed, this, &SyntaxHighlighter::themeChanged);

    themeChanged();
}

void SyntaxHighlighter::highlightBlock(const QString &text)
{
    for(const auto& rule : highlightingRules) {
        if(rule.pattern.isValid()){
            auto itt = rule.pattern.globalMatch(text);
            while (itt.hasNext()) {
                const auto match = itt.next();
                if(match.hasMatch()){
                    //Always highlight the first capture group
                    const auto& length = match.capturedLength(1);
                    const auto& index = match.capturedStart(1);
                    setFormat(index, length, rule.format);
                }
            }
        }
    }

    highlightParenthesis(text);
}

void SyntaxHighlighter::highlightParenthesis(const QString &text) {
    TextBlockData *data = new TextBlockData;


    QRegExp rx("\\(|\\)|\\{|\\}|\\[|\\]|\\<|\\>");
    int pos = 0;
    while(true){
        pos = text.indexOf(rx, pos);
        if(pos != -1){
            ParenthesisInfo* info = new ParenthesisInfo();
            info->character = text.at(pos).toLatin1();
            //qCritical() << info->character;
            //qCritical() << pos;
            info->position = pos;

            data->insert(info);
            pos ++;
        }else{
            break;
        }
    }
    setCurrentBlockUserData(data);
}

void SyntaxHighlighter::themeChanged()
{
    highlightingRules.clear();

    commentFormat.setForeground(Theme::theme()->getTextColor(ColorRole::DISABLED));
    commentFormat.setFontItalic(true);

    functionFormat.setForeground(Theme::theme()->getHighlightColor());
    functionFormat.setFontItalic(true);
    keywordFormat.setForeground(Theme::theme()->getHighlightColor());
    keywordFormat.setFontWeight(QFont::Bold);
    templateFormat.setForeground(Theme::theme()->getAltTextColor());
    quotationFormat.setForeground(Theme::theme()->getAltTextColor());
    

    HighlightingRule rule;
    QStringList keywordPatterns;
    keywordPatterns << "char" << "class" << "const"
                    << "double" << "enum" << "explicit"
                    << "friend" << "inline" << "int"
                    << "long" << "namespace" << "operator"
                    << "private" << "protected" << "public"
                    << "short" << "signals" << "signed"
                    << "slots" << "static" << "struct"
                    << "template" << "typedef" << "typename"
                    << "union" << "unsigned" << "virtual"
                    << "void" << "volatile" << "float" << "extern"
                    << "#include" << "virtual"
                    << "this" << "const";
    
    for (const auto& pattern : keywordPatterns) {
        rule.pattern = QRegularExpression("(" + pattern + ")");
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    
    

    {
        auto str_reg = QRegularExpression(R"((".*?"))");
        rule.pattern = str_reg;
        rule.format = quotationFormat;
        highlightingRules.append(rule);
    }

    {
        rule.pattern = QRegularExpression(R"((\w+[\w|\d]*(?:<.*?>)?)\()");
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }

    {
        auto angle_reg = QRegularExpression("<(.*?)>");
        rule.pattern = angle_reg;
        rule.format = templateFormat;
        highlightingRules.append(rule);
    }

    {
        rule.pattern = QRegularExpression("(//.*)");
        rule.format = commentFormat;
        highlightingRules.append(rule);
    }
}

bool ParenthesisInfo::isOpening() const
{
    return character == '(' || character == '{' || character == '[' || character == '<';

}

char ParenthesisInfo::getMatching() const
{
    switch(character){
    case '(':
        return ')';
    case ')':
        return '(';
    case '[':
        return ']';
    case ']':
        return '[';
    case '{':
        return '}';
    case '}':
        return '{';
    case '<':
        return '>';
    case '>':
        return '<';
    default:
        return character;
    }
}




