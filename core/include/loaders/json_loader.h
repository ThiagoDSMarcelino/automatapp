#pragma once

#include "loaders/loader.h"

class JsonLoader : public Loader {
public:
  Result<std::unique_ptr<SystemManager>>
  load(const std::string &path) const override;
};
