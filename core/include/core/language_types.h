#pragma once

#include <expected>
#include <string>
#include <vector>

using Symbol = std::string;
using Word = std::vector<Symbol>;
using State = std::string;

template <typename T> using Result = std::expected<T, std::string>;

const Symbol Epsilon = "ε";
