#include "core/alphabet.h"
#include "core/system_manager.h"
#include "main_window.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    auto manager = std::make_unique<SystemManager>();

    auto binary = Alphabet::Create("Binary", {"0", "1"});
    auto boolean = Alphabet::Create("Boolean", {"true", "false"});

    manager->add_alphabet(binary.value());
    manager->add_alphabet(boolean.value());

    QApplication app(argc, argv);

    MainWindow w(std::move(manager));

    w.show();

    return app.exec();
}
