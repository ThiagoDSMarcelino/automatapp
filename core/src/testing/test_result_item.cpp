#include "testing/test_result_item.h"

TestResultItem::TestResultItem(const std::vector<std::string> &word,
                               bool is_valid)
    : m_word(word), m_is_valid(is_valid) {}
