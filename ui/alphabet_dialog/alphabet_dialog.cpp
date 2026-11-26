#include "alphabet_dialog/alphabet_dialog.h"
#include "ui_alphabet_dialog.h"

#include <QMessageBox>
#include <QRegularExpression>

namespace {

std::vector<Symbol> parse_symbols(const QString &text)
{
    static const QRegularExpression separators(QStringLiteral("[,\\s]+"));

    std::vector<Symbol> symbols;
    for (const QString &symbol : text.split(separators, Qt::SkipEmptyParts)) {
        symbols.push_back(symbol.toStdString());
    }

    return symbols;
}

QString join_symbols(const std::vector<Symbol> &symbols)
{
    QStringList list;
    for (const Symbol &symbol : symbols) {
        list << QString::fromStdString(symbol);
    }

    return list.join(QStringLiteral(", "));
}

} // namespace

AlphabetDialog::AlphabetDialog(SystemManager &manager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AlphabetDialog)
    , system_manager(manager)
{
    ui->setupUi(this);

    connect(ui->alphabetList, &QListWidget::itemSelectionChanged, this, &AlphabetDialog::onSelectionChanged);
    connect(ui->newButton, &QPushButton::clicked, this, &AlphabetDialog::onNew);
    connect(ui->saveButton, &QPushButton::clicked, this, &AlphabetDialog::onSave);
    connect(ui->removeButton, &QPushButton::clicked, this, &AlphabetDialog::onRemove);

    refreshList();
    onNew();
}

AlphabetDialog::~AlphabetDialog()
{
    delete ui;
}

void AlphabetDialog::refreshList(const QString &select)
{
    QSignalBlocker blocker(ui->alphabetList);
    ui->alphabetList->clear();

    for (const auto &alphabet : system_manager.alphabets()) {
        const QString name = QString::fromStdString(alphabet->name());
        auto *item = new QListWidgetItem(name, ui->alphabetList);

        if (system_manager.is_alphabet_in_use(alphabet->name())) {
            item->setToolTip(tr("Em uso por um autômato"));
        }

        if (name == select) {
            item->setSelected(true);
            ui->alphabetList->setCurrentItem(item);
        }
    }

    blocker.unblock();
    onSelectionChanged();
}

void AlphabetDialog::onSelectionChanged()
{
    const auto selected = ui->alphabetList->selectedItems();
    if (selected.isEmpty()) {
        ui->removeButton->setEnabled(false);
        return;
    }

    const auto alphabet = system_manager.find_alphabet(selected.first()->text().toStdString());
    if (alphabet == nullptr) {
        return;
    }

    ui->nameEdit->setText(QString::fromStdString(alphabet->name()));
    ui->symbolsEdit->setText(join_symbols(alphabet->symbols()));
    ui->removeButton->setEnabled(true);
}

void AlphabetDialog::onNew()
{
    ui->alphabetList->clearSelection();
    ui->nameEdit->clear();
    ui->symbolsEdit->clear();
    ui->nameEdit->setFocus();
}

void AlphabetDialog::onSave()
{
    auto alphabet = Alphabet::Create(ui->nameEdit->text().trimmed().toStdString(),
                                     parse_symbols(ui->symbolsEdit->text()));
    if (!alphabet) {
        showError(alphabet.error());
        return;
    }

    const QString name = QString::fromStdString((*alphabet)->name());
    const auto selected = ui->alphabetList->selectedItems();

    const auto result = selected.isEmpty()
                            ? system_manager.add_alphabet(*alphabet)
                            : system_manager.replace_alphabet(selected.first()->text().toStdString(), *alphabet);
    if (!result) {
        showError(result.error());
        return;
    }

    refreshList(name);
}

void AlphabetDialog::onRemove()
{
    const auto selected = ui->alphabetList->selectedItems();
    if (selected.isEmpty()) {
        return;
    }

    const QString name = selected.first()->text();
    const auto answer = QMessageBox::question(this, tr("Remover alfabeto"),
                                              tr("Remover o alfabeto \"%1\"?").arg(name));
    if (answer != QMessageBox::Yes) {
        return;
    }

    if (const auto result = system_manager.remove_alphabet(name.toStdString()); !result) {
        showError(result.error());
        return;
    }

    refreshList();
    onNew();
}

void AlphabetDialog::showError(const std::string &message)
{
    QMessageBox::warning(this, tr("Alfabeto inválido"), QString::fromStdString(message));
}
