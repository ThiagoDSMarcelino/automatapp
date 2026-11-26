#include "core/alphabet.h"
#include "core/system_manager.h"
#include "main_window/main_window.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  auto manager = std::make_unique<SystemManager>();

  manager->add_alphabet(Alphabet::Create("Binary", {"0", "1"}).value());
  manager->add_alphabet(Alphabet::Create("Boolean", {"true", "false"}).value());

  QApplication app(argc, argv);

  MainWindow w(std::move(manager));

  w.show();

  return app.exec();
}
