#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "automatas/automata_spec.h"
#include "core/system_manager.h"
#include "storage/storage.h"
#include <QMainWindow>
#include <string>
#include <vector>

class DiagramScene;
class QListWidgetItem;
class QTimer;
class QTableWidgetItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(std::unique_ptr<SystemManager> manager, Storage data_storage,
             QWidget *parent = nullptr);
  ~MainWindow();

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  Ui::MainWindow *ui;
  std::unique_ptr<SystemManager> system_manager;
  Storage storage;
  DiagramScene *diagram_scene;
  QTimer *save_timer;

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
  void render_diagram(bool reset_layout);

  void spec_changed();
  void update_window_title();

  void schedule_save();
  void save_now();
  void store_layout();
};

#endif // MAIN_WINDOW_H
