#include "screens/main_menu.h"
#include "screens/screen_manager.h"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

Component Placeholder(ScreenManager &manager, std::string titulo) {
  auto voltar = Button("Go back", [&manager] { manager.navigate(0); });
  return Renderer(voltar, [=] {
    return vbox({text(titulo) | bold, separator(), voltar->Render()}) | border |
           center;
  });
}

int main() {
  ScreenManager manager;

  manager.add(Make<MainMenu>(manager));
  manager.add(Placeholder(manager, "Automatas"));
  manager.add(Placeholder(manager, "Languages"));
  manager.add(Placeholder(manager, "Tests"));

  manager.run();
}
