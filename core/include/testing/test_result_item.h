#pragma once

#include "core/language_types.h"

class TestResultItem {
public:
  TestResultItem(const Word &word, bool is_valid);
  ~TestResultItem() = default;

  const Word &word() const { return m_word; };
  bool is_valid() const { return m_is_valid; };

private:
  Word m_word;
  bool m_is_valid;
};
