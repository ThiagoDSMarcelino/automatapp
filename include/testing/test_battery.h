#pragma once

#include "automatas/automata_base.h"
#include "core/language_types.h"
#include "testing/test_result.h"
#include <memory>
#include <string>
#include <vector>

class TestBattery {
public:
  TestBattery(const std::string &name, std::vector<Word> &language,
              std::shared_ptr<AutomataBase> automata);
  ~TestBattery() = default;

  const std::string &name() const { return m_name; };
  const std::vector<Word> &language() const { return m_language; };
  const AutomataBase &automata() const { return *m_automata; };

  std::shared_ptr<TestResult> run();

private:
  std::string m_name;
  std::vector<Word> m_language;
  std::shared_ptr<AutomataBase> m_automata;
};
