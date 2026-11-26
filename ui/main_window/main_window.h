#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "automatas/automata_spec.h"
#include "core/system_manager.h"
#include <QMainWindow>
#include <string>
#include <vector>

class QListWidgetItem;
class QTableWidgetItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(std::unique_ptr<SystemManager> manager,
                      QWidget *parent = nullptr);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  Ui::MainWindow *ui;
  std::unique_ptr<SystemManager> system_manager;

  AutomataSpec spec;
  std::string loaded_name;
  bool has_automata = false;
  bool dirty = false;
  bool refresh_pending = false;
  std::vector<Symbol> column_symbols;

  void refresh_alphabet_combo();
  void open_alphabet_dialog();

  void refresh_library(const std::string &select = {});
  void apply_library_filter();
  void on_library_current_changed(QListWidgetItem *current,
                                  QListWidgetItem *previous);
  void on_add_automata();
  void on_rename_automata();
  void on_remove_automata();
  bool confirm_discard();

  void load_automata(const std::string &name);
  void clear_editor();

  void on_name_edited(const QString &text);
  void on_type_changed(int index);
  void on_alphabet_changed(int index);
  void on_add_state();
  void on_remove_state();
  void on_state_item_changed(QTableWidgetItem *item);
  void on_transition_item_changed(QTableWidgetItem *item);

  void schedule_refresh();
  void render_header();
  void render_states();
  void render_transitions();
  std::vector<Symbol> visible_symbols() const;

  void spec_changed();
  void update_window_title();
};

#endif // MAIN_WINDOW_H
