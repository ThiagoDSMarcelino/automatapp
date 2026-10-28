#pragma once

#include "automatas/automata.h"
#include "core.h"
#include "testing/test_result.h"
#include <memory>
#include <string>
#include <vector>

class TestBattery {
public:
  TestBattery(const std::string &name, std::vector<Word> &language,
              std::shared_ptr<Automata> automata);
  ~TestBattery() = default;

  const std::string &name() const { return m_name; };
  const std::vector<Word> &language() const { return m_language; };
  const Automata &automata() const { return *m_automata; };

  std::shared_ptr<TestResult> run();

private:
  std::string m_name;
  std::vector<Word> m_language;
  std::shared_ptr<Automata> m_automata;
};
