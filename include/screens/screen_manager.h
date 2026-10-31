#pragma once

#include <ftxui/component/component_base.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <functional>
#include <vector>

class SystemManager;

class ScreenManager {
public:
  explicit ScreenManager(SystemManager &system);

  void run();

private:
  std::function<void()> add(ftxui::Component screen);

  int m_current_screen = 0;
  int m_home_screen = 0;
  std::vector<ftxui::Component> m_screens;
  ftxui::ScreenInteractive m_screen = ftxui::ScreenInteractive::Fullscreen();
};
