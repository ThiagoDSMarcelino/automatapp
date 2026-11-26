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

    void refreshList(const QString &select = {});
    void onSelectionChanged();
    void onNew();
    void onSave();
    void onRemove();
    void showError(const std::string &message);
};

#endif // ALPHABET_DIALOG_H
