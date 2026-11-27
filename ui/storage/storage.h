#ifndef STORAGE_H
#define STORAGE_H

#include "core/language_types.h"
#include "core/system_manager.h"
#include "diagram/diagram_scene.h"
#include <QString>
#include <memory>
#include <optional>
#include <string>

class Storage {
public:
  struct LoadResult {
    std::unique_ptr<SystemManager> system;
    std::optional<QString> warning;
  };

  explicit Storage(const QString &directory);

  static QString default_directory();

  const QString &data_path() const { return m_data_path; }

  LoadResult load() const;
  Result<void> save(const SystemManager &system) const;

  DiagramScene::Positions layout(const std::string &automata) const;
  void set_layout(const std::string &automata,
                  const DiagramScene::Positions &positions) const;
  void remove_layout(const std::string &automata) const;
  void rename_layout(const std::string &from, const std::string &to) const;

private:
  QString m_data_path;
  QString m_layout_path;

  QVariantMap layouts() const;
  void set_layouts(const QVariantMap &layouts) const;
};

#endif // STORAGE_H
