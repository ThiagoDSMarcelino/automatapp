#pragma once

#include <ftxui/component/screen_interactive.hpp>

class ScreenManager {
public:
  void add(ftxui::Component screen);
  void navigate(int screen_index);
  void exit();
  void run();

private:
  int m_current_screen = 0;
  std::vector<ftxui::Component> m_screens;
  ftxui::ScreenInteractive m_screen = ftxui::ScreenInteractive::Fullscreen();
};
