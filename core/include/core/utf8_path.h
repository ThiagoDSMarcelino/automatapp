#pragma once

#include <filesystem>
#include <string>

inline std::filesystem::path utf8_path(const std::string &path) {
  return std::filesystem::path(std::u8string(
      reinterpret_cast<const char8_t *>(path.data()), path.size()));
}
