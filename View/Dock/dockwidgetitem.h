#ifndef DOCKWIDGETITEM_H
#define DOCKWIDGETITEM_H

#include <QToolButton>
#include "../theme.h"

class DockWidgetItem : public QToolButton
{
    Q_OBJECT

public:
    explicit DockWidgetItem(QString text, QWidget* parent = 0);

    void setText(QString text);
    QString getText();
    QString getDisplayedText();

    void displayToolButtonText(bool show);

signals:
    void textChanged();
    void displayedTextChanged(QString);

public slots:
    void themeChanged();

protected:
    void resizeEvent(QResizeEvent* event);

private:
    void setupLayout();
    void updateTextLabel();

    QString fullText;
    QString currentText;
    bool displayText;
};

#endif // DOCKWIDGETITEM_H
