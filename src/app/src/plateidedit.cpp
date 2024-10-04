#include "plateidedit.h"

PlateIdEdit::PlateIdEdit(QWidget *parent) : QLineEdit(parent) { }

void PlateIdEdit::setCompleter(QCompleter *completer) {
    if (c)
        c->disconnect(this);

    c = completer;

    if (!c)
        return;

    c->setWidget(this);
    c->setCompletionMode(QCompleter::PopupCompletion);
    c->setCaseSensitivity(Qt::CaseInsensitive);
}

QCompleter *PlateIdEdit::completer() const {
    return c;
}

void PlateIdEdit::focusInEvent(QFocusEvent *e) {
    if (c)
        c->setWidget(this);
    c->complete();
    QTextEdit::focusInEvent(e);
}

void PlateIdEdit::inputMethodEvent(QInputMethodEvent *e) {
    Q_D(QLineEdit);
    if (d->control->isReadOnly()) {
        e->ignore();
        return;
    }

    if (echoMode() == PasswordEchoOnEdit && !d->control->passwordEchoEditing()) {
        // Clear the edit and reset to normal echo mode while entering input
        // method data; the echo mode switches back when the edit loses focus.
        // ### changes a public property, resets current content.
        d->updatePasswordEchoEditing(true);
        clear();
    }

#ifdef QT_KEYPAD_NAVIGATION
    // Focus in if currently in navigation focus on the widget
    // Only focus in on preedits, to allow input methods to
    // commit text as they focus out without interfering with focus
    if (QApplication::keypadNavigationEnabled()
        && hasFocus() && !hasEditFocus()
        && !e->preeditString().isEmpty())
        setEditFocus(true);
#endif

    d->control->processInputMethodEvent(e);

#ifndef QT_NO_COMPLETER
    if (e->commitString().isEmpty()) {
        d->control->completer()->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    } else {
        d->control->completer()->setCompletionMode(QCompleter::PopupCompletion);
    }
    d->control->complete(Qt::Key_unknown);
#endif
}
