#include "main_window/main_window.h"
#include "ui_main_window.h"
#include "alphabet_dialog/alphabet_dialog.h"
#include <ranges>

MainWindow::MainWindow(std::unique_ptr<SystemManager> manager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , system_manager(std::move(manager))
{
    ui->setupUi(this);

    ui->mainSplitter->setStretchFactor(0, 3);
    ui->mainSplitter->setStretchFactor(1, 2);

    connect(ui->manageAlphabetsButton, &QPushButton::clicked, this, &MainWindow::openAlphabetDialog);

    refreshAlphabetCombo();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshAlphabetCombo()
{
    const auto &alphabets = system_manager->alphabets();

    auto view = alphabets | std::views::transform([](const std::shared_ptr<Alphabet> &a) {
                    return QString::fromStdString(a->name());
                });

    QStringList names(view.begin(), view.end());

    const QString current = ui->alphabetCombo->currentText();

    ui->alphabetCombo->clear();
    ui->alphabetCombo->addItems(names);

    if (const int index = ui->alphabetCombo->findText(current); index >= 0) {
        ui->alphabetCombo->setCurrentIndex(index);
    }
}

void MainWindow::openAlphabetDialog()
{
    AlphabetDialog dialog(*system_manager, this);
    dialog.exec();

    refreshAlphabetCombo();
}
