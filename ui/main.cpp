#include "main_window/main_window.h"
#include "storage/storage.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setApplicationName(QStringLiteral("automatapp"));

  Storage storage(Storage::default_directory());
  auto [manager, warning] = storage.load();

  MainWindow w(std::move(manager), std::move(storage));

  w.show();

  if (warning) {
    QMessageBox::warning(&w, QObject::tr("Erro ao carregar dados"), *warning);
  }

  return app.exec();
}
