#include "storage/storage.h"
#include "exporters/json_exporter.h"
#include "loaders/json_loader.h"

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>

namespace {

const QString LayoutsKey = QStringLiteral("layouts");

std::unique_ptr<SystemManager> seeded_system() {
  auto system = std::make_unique<SystemManager>();
  system->add_alphabet(Alphabet::Create("Binary", {"0", "1"}).value());
  system->add_alphabet(Alphabet::Create("Boolean", {"true", "false"}).value());
  return system;
}

} // namespace

Storage::Storage(const QString &directory)
    : m_data_path(QDir(directory).filePath(QStringLiteral("automatapp.json"))),
      m_layout_path(QDir(directory).filePath(QStringLiteral("layout.ini"))) {
  QDir().mkpath(directory);
}

QString Storage::default_directory() {
  return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

Storage::LoadResult Storage::load() const {
  if (!QFile::exists(m_data_path)) {
    return {seeded_system(), std::nullopt};
  }

  auto loaded = JsonLoader().load(m_data_path.toStdString());
  if (loaded) {
    return {std::move(*loaded), std::nullopt};
  }

  const QString backup = m_data_path + QStringLiteral(".bak");
  QFile::remove(backup);
  const bool copied = QFile::copy(m_data_path, backup);

  const QString warning =
      QObject::tr("Não foi possível carregar os dados salvos:\n%1\n\n%2")
          .arg(QString::fromStdString(loaded.error()),
               copied ? QObject::tr("Uma cópia do arquivo foi salva em:\n%1")
                            .arg(QDir::toNativeSeparators(backup))
                      : QObject::tr("Não foi possível criar uma cópia de "
                                    "segurança do arquivo."));

  return {seeded_system(), warning};
}

Result<void> Storage::save(const SystemManager &system) const {
  return JsonExporter().export_system(system, m_data_path.toStdString());
}

DiagramScene::Positions Storage::layout(const std::string &automata) const {
  DiagramScene::Positions positions;

  const QVariantMap states =
      layouts().value(QString::fromStdString(automata)).toMap();
  for (auto it = states.begin(); it != states.end(); ++it) {
    positions.insert(it.key(), it.value().toPointF());
  }

  return positions;
}

void Storage::set_layout(const std::string &automata,
                         const DiagramScene::Positions &positions) const {
  QVariantMap states;
  for (auto it = positions.begin(); it != positions.end(); ++it) {
    states.insert(it.key(), it.value());
  }

  QVariantMap all = layouts();
  all.insert(QString::fromStdString(automata), states);
  set_layouts(all);
}

void Storage::remove_layout(const std::string &automata) const {
  QVariantMap all = layouts();
  if (all.remove(QString::fromStdString(automata)) > 0) {
    set_layouts(all);
  }
}

void Storage::rename_layout(const std::string &from,
                            const std::string &to) const {
  QVariantMap all = layouts();
  const QString key = QString::fromStdString(from);
  if (!all.contains(key)) {
    return;
  }

  all.insert(QString::fromStdString(to), all.take(key));
  set_layouts(all);
}

QVariantMap Storage::layouts() const {
  return QSettings(m_layout_path, QSettings::IniFormat)
      .value(LayoutsKey)
      .toMap();
}

void Storage::set_layouts(const QVariantMap &layouts) const {
  QSettings settings(m_layout_path, QSettings::IniFormat);
  settings.setValue(LayoutsKey, layouts);
}
