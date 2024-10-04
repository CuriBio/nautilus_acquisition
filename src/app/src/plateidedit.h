#include <QLineEdit>
#include <QCompleter>
#include <QKeyEvent>
#include <QInputMethodEvent>
#include <QWidget>


class PlateIdEdit : public QLineEdit {
    Q_OBJECT

    public:
        explicit PlateIdEdit(QWidget *parent = nullptr);
        ~PlateIdEdit() {}

        void setCompleter(QCompleter *c);
        QCompleter *completer() const;

    protected:
        void keyPressEvent(QKeyEvent *e) override;
        void inputMethodEvent(QInputMethodEvent *e) override;

    private:
        QCompleter *c = nullptr;
}
