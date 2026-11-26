#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "core/system_manager.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(std::unique_ptr<SystemManager> manager, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<SystemManager> system_manager;

    void refreshAlphabetCombo();
    void openAlphabetDialog();
};

#endif // MAIN_WINDOW_H
