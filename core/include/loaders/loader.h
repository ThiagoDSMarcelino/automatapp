#pragma once

#include "core/language_types.h"
#include "core/system_manager.h"
#include <memory>
#include <string>

class Loader {
public:
  virtual ~Loader() = default;

  virtual Result<std::unique_ptr<SystemManager>>
  load(const std::string &path) const = 0;
};
