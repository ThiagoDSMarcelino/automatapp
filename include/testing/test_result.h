#pragma once

#include "testing/test_result_item.h"
#include <vector>

class TestResult {
public:
  explicit TestResult(std::vector<TestResultItem> results);
  ~TestResult() = default;

  const std::vector<TestResultItem> &itens() const { return m_items; };
  bool has_pass() const { return m_has_pass; };

private:
  std::vector<TestResultItem> m_items;
  bool m_has_pass = true;
};
