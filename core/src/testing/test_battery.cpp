#include "testing/test_battery.h"
#include "testing/test_result.h"
#include "testing/test_result_item.h"
#include <memory>
#include <vector>

TestBattery::TestBattery(const std::string &name,
                         std::vector<std::vector<std::string>> &language,
                         std::shared_ptr<AutomataBase> automata)
    : m_name(name), m_language(language), m_automata(automata) {}

std::shared_ptr<TestResult> TestBattery::run() {
  std::vector<TestResultItem> items;
  items.reserve(m_language.size());

  for (const auto &word : m_language) {
    auto pass = m_automata->accept(word);

    TestResultItem item(word, pass);

    items.emplace_back(item);
  }

  return std::make_shared<TestResult>(items);
}
