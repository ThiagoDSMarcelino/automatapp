#include "main_window.h"
#include "ui_main_window.h"
#include <ranges>

MainWindow::MainWindow(std::unique_ptr<SystemManager> manager, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , system_manager(std::move(manager))
{
    ui->setupUi(this);

    ui->mainSplitter->setStretchFactor(0, 3);
    ui->mainSplitter->setStretchFactor(1, 2);

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

    ui->alphabetCombo->clear();
    ui->alphabetCombo->addItems(names);
}
