#ifndef ALPHABET_DIALOG_H
#define ALPHABET_DIALOG_H

#include "core/system_manager.h"
#include <QDialog>

namespace Ui {
class AlphabetDialog;
}

class AlphabetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlphabetDialog(SystemManager &manager, QWidget *parent = nullptr);
    ~AlphabetDialog();

private:
    Ui::AlphabetDialog *ui;
    SystemManager &system_manager;

    void refresh_list(const QString &select = {});
    void on_selection_changed();
    void on_new();
    void on_save();
    void on_remove();
    void show_error(const std::string &message);
};

#endif // ALPHABET_DIALOG_H
