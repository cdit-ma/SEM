#ifndef SEARCHSUGGESTCOMPLETION_H
#define SEARCHSUGGESTCOMPLETION_H

#include <QObject>
#include <QLineEdit>
#include <QTreeWidget>

class SearchSuggestCompletion : public QObject
{
    Q_OBJECT

public:
    explicit SearchSuggestCompletion(QLineEdit* parent = 0);
    ~SearchSuggestCompletion();

    bool eventFilter(QObject* obj, QEvent* ev) Q_DECL_OVERRIDE;
    void setSize(qreal width = 0, qreal height = 0, int sizeKind = 0);

public slots:
    void doneCompletion();
    void showCompletion(const QStringList &choices);

private:
    QLineEdit* editor;
    QTreeWidget* popup;

    QSize maxSize;
    QSize minSize;

};

#endif // SEARCHSUGGESTCOMPLETION_H
