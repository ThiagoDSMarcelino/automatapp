#include "main_window/main_window.h"
#include "alphabet_dialog/alphabet_dialog.h"
#include "ui_main_window.h"

#include <QCloseEvent>
#include <QInputDialog>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTimer>
#include <algorithm>
#include <ranges>

namespace {

const QColor ErrorBackground(220, 50, 50, 70);
const QColor ErrorText(200, 40, 40);
const QColor OkText(30, 140, 60);
const QColor WarningText(190, 120, 0);

QString to_qt(const std::string &text) { return QString::fromStdString(text); }

bool contains(const std::vector<std::string> &items, const std::string &item) {
  return std::ranges::find(items, item) != items.end();
}

std::vector<std::string> parse_targets(const QString &text) {
  static const QRegularExpression separators(QStringLiteral("[,\\s]+"));

  std::vector<std::string> targets;
  for (const QString &target : text.split(separators, Qt::SkipEmptyParts)) {
    if (!contains(targets, target.toStdString())) {
      targets.push_back(target.toStdString());
    }
  }

  return targets;
}

QString join_targets(const std::vector<std::string> &targets) {
  QStringList list;
  for (const auto &target : targets) {
    list << to_qt(target);
  }

  return list.join(QStringLiteral(", "));
}

TransitionSpec *find_transition(AutomataSpec &spec, const std::string &from,
                                const Symbol &symbol) {
  auto it =
      std::ranges::find_if(spec.transitions, [&](const TransitionSpec &t) {
        return t.from == from && t.symbol == symbol;
      });

  return it == spec.transitions.end() ? nullptr : &*it;
}

QListWidgetItem *add_message(QListWidget *list, const QString &text,
                             const QColor &color) {
  auto *item = new QListWidgetItem(text, list);
  item->setForeground(color);
  return item;
}

} // namespace

MainWindow::MainWindow(std::unique_ptr<SystemManager> manager, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
      system_manager(std::move(manager)) {
  ui->setupUi(this);

  ui->mainSplitter->setStretchFactor(0, 3);
  ui->mainSplitter->setStretchFactor(1, 2);

  connect(ui->manageAlphabetsButton, &QPushButton::clicked, this,
          &MainWindow::open_alphabet_dialog);

  connect(ui->libraryList, &QListWidget::currentItemChanged, this,
          &MainWindow::on_library_current_changed);
  connect(ui->filterEdit, &QLineEdit::textChanged, this,
          &MainWindow::apply_library_filter);
  connect(ui->addButton, &QPushButton::clicked, this,
          &MainWindow::on_add_automata);
  connect(ui->renameButton, &QPushButton::clicked, this,
          &MainWindow::on_rename_automata);
  connect(ui->removeButton, &QPushButton::clicked, this,
          &MainWindow::on_remove_automata);

  connect(ui->nameEdit, &QLineEdit::textEdited, this,
          &MainWindow::on_name_edited);
  connect(ui->typeCombo, &QComboBox::currentIndexChanged, this,
          &MainWindow::on_type_changed);
  connect(ui->alphabetCombo, &QComboBox::currentIndexChanged, this,
          &MainWindow::on_alphabet_changed);
  connect(ui->addStateButton, &QPushButton::clicked, this,
          &MainWindow::on_add_state);
  connect(ui->removeStateButton, &QPushButton::clicked, this,
          &MainWindow::on_remove_state);
  connect(ui->stateTable, &QTableWidget::itemChanged, this,
          &MainWindow::on_state_item_changed);
  connect(ui->transitionTable, &QTableWidget::itemChanged, this,
          &MainWindow::on_transition_item_changed);

  refresh_alphabet_combo();
  refresh_library();

  if (ui->libraryList->count() > 0) {
    ui->libraryList->setCurrentRow(0);
  } else {
    clear_editor();
  }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::closeEvent(QCloseEvent *event) {
  if (confirm_discard()) {
    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::refresh_alphabet_combo() {
  const auto &alphabets = system_manager->alphabets();

  auto view =
      alphabets | std::views::transform([](const std::shared_ptr<Alphabet> &a) {
        return to_qt(a->name());
      });

  QStringList names(view.begin(), view.end());

  QSignalBlocker blocker(ui->alphabetCombo);
  ui->alphabetCombo->clear();
  ui->alphabetCombo->addItems(names);
  ui->alphabetCombo->setCurrentIndex(
      ui->alphabetCombo->findText(to_qt(spec.alphabet)));
}

void MainWindow::open_alphabet_dialog() {
  AlphabetDialog dialog(*system_manager, this);
  dialog.exec();

  refresh_alphabet_combo();
  if (has_automata) {
    schedule_refresh();
  }
}

void MainWindow::refresh_library(const std::string &select) {
  QSignalBlocker blocker(ui->libraryList);
  ui->libraryList->clear();

  for (const auto &automata : system_manager->automatas()) {
    auto *item = new QListWidgetItem(to_qt(automata->name()), ui->libraryList);
    if (!select.empty() && automata->name() == select) {
      ui->libraryList->setCurrentItem(item);
    }
  }

  apply_library_filter();
}

void MainWindow::apply_library_filter() {
  const QString filter = ui->filterEdit->text().trimmed();

  for (int i = 0; i < ui->libraryList->count(); ++i) {
    auto *item = ui->libraryList->item(i);
    item->setHidden(!item->text().contains(filter, Qt::CaseInsensitive));
  }
}

void MainWindow::on_library_current_changed(QListWidgetItem *current,
                                            QListWidgetItem *previous) {
  if (current == nullptr || current->text().toStdString() == loaded_name) {
    return;
  }

  if (!confirm_discard()) {
    QSignalBlocker blocker(ui->libraryList);
    ui->libraryList->setCurrentItem(previous);
    return;
  }

  load_automata(current->text().toStdString());
}

void MainWindow::on_add_automata() {
  if (system_manager->alphabets().empty()) {
    QMessageBox::information(this, tr("Novo autômato"),
                             tr("Crie um alfabeto antes, em \"Gerenciar…\"."));
    return;
  }

  if (!confirm_discard()) {
    return;
  }

  std::string name;
  for (int n = 1;
       name.empty() || system_manager->find_automata(name) != nullptr; ++n) {
    name = tr("Autômato %1").arg(n).toStdString();
  }

  auto alphabet = system_manager->find_alphabet(
      ui->alphabetCombo->currentText().toStdString());
  if (alphabet == nullptr) {
    alphabet = system_manager->alphabets().front();
  }

  const AutomataSpec blank{
      name, AutomataType::DFA, alphabet->name(), {"q0"}, "q0", {}, {}};

  auto built = build_automata(blank, alphabet);
  if (built) {
    if (auto added = system_manager->add_automata(*built); !added) {
      built = std::unexpected(added.error());
    }
  }

  if (!built) {
    QMessageBox::warning(this, tr("Novo autômato"), to_qt(built.error()));
    return;
  }

  refresh_library(name);
  load_automata(name);
}

void MainWindow::on_rename_automata() {
  if (!has_automata) {
    return;
  }

  bool ok = false;
  const QString name =
      QInputDialog::getText(this, tr("Renomear autômato"), tr("Novo nome:"),
                            QLineEdit::Normal, to_qt(spec.name), &ok)
          .trimmed();
  if (!ok || name.toStdString() == spec.name) {
    return;
  }

  ui->nameEdit->setText(name);
  on_name_edited(name);
}

void MainWindow::on_remove_automata() {
  auto *item = ui->libraryList->currentItem();
  if (item == nullptr) {
    return;
  }

  const QString name = item->text();
  const auto answer = QMessageBox::question(
      this, tr("Remover autômato"), tr("Remover o autômato \"%1\"?").arg(name));
  if (answer != QMessageBox::Yes) {
    return;
  }

  if (auto removed = system_manager->remove_automata(name.toStdString());
      !removed) {
    QMessageBox::warning(this, tr("Remover autômato"), to_qt(removed.error()));
    return;
  }

  if (name.toStdString() == loaded_name) {
    dirty = false;
    clear_editor();
  }

  refresh_library(loaded_name);

  if (!has_automata && ui->libraryList->count() > 0) {
    ui->libraryList->setCurrentRow(0);
  }
}

bool MainWindow::confirm_discard() {
  if (!dirty) {
    return true;
  }

  const auto answer =
      QMessageBox::question(this, tr("Alterações não salvas"),
                            tr("O autômato \"%1\" tem erros, então as últimas "
                               "alterações não foram salvas.\n"
                               "Descartar essas alterações?")
                                .arg(to_qt(spec.name)));

  return answer == QMessageBox::Yes;
}

void MainWindow::load_automata(const std::string &name) {
  const auto automata = system_manager->find_automata(name);
  if (automata == nullptr) {
    clear_editor();
    return;
  }

  spec = to_spec(*automata);
  loaded_name = name;
  has_automata = true;
  dirty = false;

  ui->statesGroup->setEnabled(true);
  ui->transitionsGroup->setEnabled(true);
  ui->nameEdit->setEnabled(true);
  ui->typeCombo->setEnabled(true);
  ui->alphabetCombo->setEnabled(true);

  render_header();
  render_states();
  render_transitions();
  spec_changed();
}

void MainWindow::clear_editor() {
  spec = {};
  loaded_name.clear();
  has_automata = false;
  dirty = false;

  render_header();
  render_states();
  render_transitions();

  ui->statesGroup->setEnabled(false);
  ui->transitionsGroup->setEnabled(false);
  ui->nameEdit->setEnabled(false);
  ui->typeCombo->setEnabled(false);
  ui->alphabetCombo->setEnabled(false);

  ui->validationList->clear();
  add_message(
      ui->validationList,
      tr("Nenhum autômato selecionado. Use \"Adicionar\" na biblioteca."),
      palette().color(QPalette::PlaceholderText));

  update_window_title();
}

void MainWindow::on_name_edited(const QString &text) {
  spec.name = text.trimmed().toStdString();
  spec_changed();
}

void MainWindow::on_type_changed(int index) {
  spec.type = static_cast<AutomataType>(index);
  schedule_refresh();
}

void MainWindow::on_alphabet_changed(int index) {
  spec.alphabet = index < 0 ? std::string{}
                            : ui->alphabetCombo->itemText(index).toStdString();
  schedule_refresh();
}

void MainWindow::on_add_state() {
  std::string name;
  for (int n = 0; name.empty() || contains(spec.states, name); ++n) {
    name = "q" + std::to_string(n);
  }

  spec.states.push_back(name);
  if (spec.initial.empty()) {
    spec.initial = name;
  }

  schedule_refresh();
}

void MainWindow::on_remove_state() {
  const int row = ui->stateTable->currentRow();
  if (row < 0 || row >= static_cast<int>(spec.states.size())) {
    return;
  }

  const std::string removed = spec.states[row];

  std::erase(spec.states, removed);
  std::erase(spec.finals, removed);
  if (spec.initial == removed) {
    spec.initial.clear();
  }

  for (auto &t : spec.transitions) {
    std::erase(t.to, removed);
  }
  std::erase_if(spec.transitions, [&](const TransitionSpec &t) {
    return t.from == removed || t.to.empty();
  });

  schedule_refresh();
}

void MainWindow::on_state_item_changed(QTableWidgetItem *item) {
  const int row = item->row();
  if (row < 0 || row >= static_cast<int>(spec.states.size())) {
    return;
  }

  const std::string state = spec.states[row];
  const bool checked = item->checkState() == Qt::Checked;

  switch (item->column()) {
  case 0: {
    const std::string renamed = item->text().trimmed().toStdString();
    if (renamed == state) {
      break;
    }

    if (renamed.empty() || contains(spec.states, renamed)) {
      statusBar()->showMessage(
          renamed.empty()
              ? tr("O nome do estado não pode ser vazio.")
              : tr("Já existe um estado \"%1\".").arg(to_qt(renamed)),
          4000);
      break;
    }

    spec.states[row] = renamed;
    if (spec.initial == state) {
      spec.initial = renamed;
    }
    std::ranges::replace(spec.finals, state, renamed);
    for (auto &t : spec.transitions) {
      if (t.from == state) {
        t.from = renamed;
      }
      std::ranges::replace(t.to, state, renamed);
    }
    break;
  }
  case 1:
    if (checked) {
      spec.initial = state;
    } else if (spec.initial == state) {
      spec.initial.clear();
    }
    break;
  case 2:
    if (checked && !contains(spec.finals, state)) {
      spec.finals.push_back(state);
    } else if (!checked) {
      std::erase(spec.finals, state);
    }
    break;
  }

  schedule_refresh();
}

void MainWindow::on_transition_item_changed(QTableWidgetItem *item) {
  const int row = item->row();
  const int column = item->column();
  if (row < 0 || row >= static_cast<int>(spec.states.size()) || column < 0 ||
      column >= static_cast<int>(column_symbols.size())) {
    return;
  }

  const std::string &from = spec.states[row];
  const Symbol &symbol = column_symbols[column];
  const auto targets = parse_targets(item->text());

  if (targets.empty()) {
    std::erase_if(spec.transitions, [&](const TransitionSpec &t) {
      return t.from == from && t.symbol == symbol;
    });
  } else if (auto *existing = find_transition(spec, from, symbol)) {
    existing->to = targets;
  } else {
    spec.transitions.push_back({from, symbol, targets});
  }

  schedule_refresh();
}

void MainWindow::schedule_refresh() {
  if (refresh_pending) {
    return;
  }

  refresh_pending = true;
  QTimer::singleShot(0, this, [this] {
    refresh_pending = false;
    render_header();
    render_states();
    render_transitions();
    spec_changed();
  });
}

void MainWindow::render_header() {
  QSignalBlocker name_blocker(ui->nameEdit);
  QSignalBlocker type_blocker(ui->typeCombo);
  QSignalBlocker alphabet_blocker(ui->alphabetCombo);

  if (ui->nameEdit->text().trimmed().toStdString() != spec.name) {
    ui->nameEdit->setText(to_qt(spec.name));
  }

  ui->typeCombo->setCurrentIndex(static_cast<int>(spec.type));
  ui->alphabetCombo->setCurrentIndex(
      ui->alphabetCombo->findText(to_qt(spec.alphabet)));
}

void MainWindow::render_states() {
  QSignalBlocker blocker(ui->stateTable);

  const int current_row = ui->stateTable->currentRow();
  ui->stateTable->setRowCount(static_cast<int>(spec.states.size()));

  for (int row = 0; row < static_cast<int>(spec.states.size()); ++row) {
    const std::string &state = spec.states[row];

    auto *name = new QTableWidgetItem(to_qt(state));

    auto *initial = new QTableWidgetItem;
    initial->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable |
                      Qt::ItemIsSelectable);
    initial->setCheckState(state == spec.initial ? Qt::Checked : Qt::Unchecked);

    auto *final_state = new QTableWidgetItem;
    final_state->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable |
                          Qt::ItemIsSelectable);
    final_state->setCheckState(contains(spec.finals, state) ? Qt::Checked
                                                            : Qt::Unchecked);

    ui->stateTable->setItem(row, 0, name);
    ui->stateTable->setItem(row, 1, initial);
    ui->stateTable->setItem(row, 2, final_state);
  }

  if (current_row >= 0 && current_row < ui->stateTable->rowCount()) {
    ui->stateTable->setCurrentCell(current_row, 0);
  }
}

std::vector<Symbol> MainWindow::visible_symbols() const {
  std::vector<Symbol> symbols;

  if (auto alphabet = system_manager->find_alphabet(spec.alphabet)) {
    symbols = alphabet->symbols();
  }

  if (spec.type == AutomataType::eNFA) {
    symbols.push_back(Epsilon);
  }

  for (const auto &t : spec.transitions) {
    if (!contains(symbols, t.symbol)) {
      symbols.push_back(t.symbol);
    }
  }

  return symbols;
}

void MainWindow::render_transitions() {
  QSignalBlocker blocker(ui->transitionTable);

  const auto alphabet = system_manager->find_alphabet(spec.alphabet);
  column_symbols = visible_symbols();

  ui->transitionTable->clear();
  ui->transitionTable->setRowCount(static_cast<int>(spec.states.size()));
  ui->transitionTable->setColumnCount(static_cast<int>(column_symbols.size()));

  for (int column = 0; column < static_cast<int>(column_symbols.size());
       ++column) {
    const Symbol &symbol = column_symbols[column];
    auto *header = new QTableWidgetItem(to_qt(symbol));

    const bool foreign =
        symbol == Epsilon ? spec.type != AutomataType::eNFA
                          : alphabet == nullptr || !alphabet->contains(symbol);
    if (foreign) {
      header->setForeground(ErrorText);
      header->setToolTip(symbol == Epsilon ? tr("ε só é permitido em AFN-ε")
                                           : tr("Símbolo fora do alfabeto"));
    }

    ui->transitionTable->setHorizontalHeaderItem(column, header);
  }

  for (int row = 0; row < static_cast<int>(spec.states.size()); ++row) {
    const std::string &state = spec.states[row];

    QString label = to_qt(state);
    if (state == spec.initial) {
      label.prepend(QStringLiteral("→ "));
    }
    if (contains(spec.finals, state)) {
      label.prepend(QStringLiteral("* "));
    }
    ui->transitionTable->setVerticalHeaderItem(row,
                                               new QTableWidgetItem(label));

    for (int column = 0; column < static_cast<int>(column_symbols.size());
         ++column) {
      const Symbol &symbol = column_symbols[column];
      auto *cell = new QTableWidgetItem;
      ui->transitionTable->setItem(row, column, cell);

      const auto *t = find_transition(spec, state, symbol);
      if (t == nullptr) {
        continue;
      }

      cell->setText(join_targets(t->to));

      QStringList problems;
      if (symbol == Epsilon && spec.type != AutomataType::eNFA) {
        problems << tr("ε só é permitido em AFN-ε");
      } else if (symbol != Epsilon &&
                 (alphabet == nullptr || !alphabet->contains(symbol))) {
        problems << tr("Símbolo fora do alfabeto");
      }
      if (spec.type == AutomataType::DFA && t->to.size() > 1) {
        problems << tr("AFD aceita apenas um destino");
      }
      for (const auto &target : t->to) {
        if (!contains(spec.states, target)) {
          problems << tr("Estado \"%1\" não existe").arg(to_qt(target));
        }
      }

      if (!problems.isEmpty()) {
        cell->setBackground(ErrorBackground);
        cell->setToolTip(problems.join('\n'));
      }
    }
  }
}

void MainWindow::spec_changed() {
  if (!has_automata) {
    return;
  }

  Result<std::shared_ptr<AutomataBase>> built = std::unexpected(std::string{});

  if (spec.name.empty()) {
    built = std::unexpected(
        tr("O nome do autômato não pode ser vazio").toStdString());
  } else if (spec.name != loaded_name &&
             system_manager->find_automata(spec.name) != nullptr) {
    built = std::unexpected(tr("Já existe um autômato chamado \"%1\"")
                                .arg(to_qt(spec.name))
                                .toStdString());
  } else {
    built = build_automata(spec, system_manager->find_alphabet(spec.alphabet));
  }

  if (built) {
    if (auto saved = system_manager->replace_automata(loaded_name, *built);
        !saved) {
      built = std::unexpected(saved.error());
    }
  }

  ui->validationList->clear();

  if (!built) {
    dirty = true;
    add_message(ui->validationList, QStringLiteral("✗ ") + to_qt(built.error()),
                ErrorText);
    add_message(ui->validationList,
                tr("Alterações não salvas até o autômato ficar válido."),
                palette().color(QPalette::PlaceholderText));
    update_window_title();
    return;
  }

  dirty = false;
  add_message(ui->validationList, tr("✓ Autômato válido e salvo"), OkText);

  if (spec.type == AutomataType::DFA) {
    const auto alphabet = system_manager->find_alphabet(spec.alphabet);
    const auto defined = static_cast<int>(spec.transitions.size());
    const auto expected =
        static_cast<int>(spec.states.size() * alphabet->symbols().size());
    if (defined < expected) {
      add_message(ui->validationList,
                  tr("⚠ AFD parcial: %1 transição(ões) não definida(s); a "
                     "palavra é rejeitada ao cair nelas")
                      .arg(expected - defined),
                  WarningText);
    }
  }

  if (spec.finals.empty()) {
    add_message(
        ui->validationList,
        tr("⚠ Nenhum estado final: o autômato não aceita nenhuma palavra"),
        WarningText);
  }

  if (loaded_name != spec.name) {
    loaded_name = spec.name;
    refresh_library(loaded_name);
  }

  update_window_title();
}

void MainWindow::update_window_title() {
  QString title = QStringLiteral("Automata++");
  if (has_automata) {
    title += QStringLiteral(" — ") +
             to_qt(spec.name.empty() ? loaded_name : spec.name);
    if (dirty) {
      title += QStringLiteral(" *");
    }
  }

  setWindowTitle(title);
}
