#pragma once

#include <string>

class State {
private:
  std::string m_name;

public:
  explicit State(std::string name);

  const std::string &name() const { return m_name; };
};
