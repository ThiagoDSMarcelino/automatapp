#include "testing/test_result.h"
#include <algorithm>
#include <utility>

TestResult::TestResult(std::vector<TestResultItem> results)
    : m_items(std::move(results)),
      m_has_pass(
          std::all_of(m_items.begin(), m_items.end(),
                      [](const TestResultItem &i) { return i.is_valid(); })) {}
