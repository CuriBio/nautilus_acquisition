#include <QLineEdit>
#include <QCompleter>
#include <QFocusEvent>
#include <QInputMethodEvent>
#include <QWidget>

#ifndef PLATEIDEDIT_H
#define PLATEIDEDIT_H


class PlateIdEdit : public QLineEdit {
    Q_OBJECT

    public:
        PlateIdEdit(QWidget *parent = nullptr);
        ~PlateIdEdit() {}

        // void setCompleter(QCompleter *c);
        // QCompleter *completer() const;

    protected:
        void focusInEvent(QFocusEvent *e) override;
        // void inputMethodEvent(QInputMethodEvent *e) override;

    private:
        QCompleter *c = nullptr;
};

#endif // PLATEIDEDIT_H