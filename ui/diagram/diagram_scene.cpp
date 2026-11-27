#include "diagram/diagram_scene.h"
#include "diagram/edge_item.h"
#include "diagram/state_item.h"

#include <QGraphicsSceneMouseEvent>
#include <QLineF>
#include <QtMath>
#include <algorithm>
#include <map>
#include <utility>

namespace {

constexpr qreal StateSpacing = 110;

QPointF circle_position(int index, int count) {
  if (count <= 1) {
    return {0, 0};
  }

  const qreal radius = std::max(90.0, count * StateSpacing / (2 * M_PI));
  const qreal angle = M_PI + index * 2 * M_PI / count;
  return {radius * std::cos(angle), radius * std::sin(angle)};
}

QPointF free_position(const std::vector<QPointF> &taken) {
  if (taken.empty()) {
    return {0, 0};
  }

  QPointF center;
  for (const auto &point : taken) {
    center += point;
  }
  center /= static_cast<qreal>(taken.size());

  for (int ring = 1;; ++ring) {
    const qreal radius = ring * StateSpacing;
    const int steps = 6 * ring;
    for (int step = 0; step < steps; ++step) {
      const qreal angle = step * 2 * M_PI / steps;
      const QPointF candidate =
          center + radius * QPointF(std::cos(angle), std::sin(angle));

      const bool free = std::ranges::none_of(taken, [&](const QPointF &point) {
        return QLineF(point, candidate).length() < StateSpacing * 0.9;
      });
      if (free) {
        return candidate;
      }
    }
  }
}

bool contains(const std::vector<std::string> &items, const std::string &item) {
  return std::ranges::find(items, item) != items.end();
}

} // namespace

DiagramScene::DiagramScene(QObject *parent) : QGraphicsScene(parent) {}

void DiagramScene::set_spec(const AutomataSpec &spec, bool reset_layout) {
  if (reset_layout) {
    m_positions.clear();
    m_auto_layout = true;
  } else {
    store_positions();
  }

  m_spec = spec;
  rebuild();
}

DiagramScene::Positions DiagramScene::positions() const {
  Positions result;
  for (auto *item : items()) {
    if (auto *state = dynamic_cast<StateItem *>(item)) {
      result.insert(state->name(), state->pos());
    }
  }

  return result;
}

void DiagramScene::set_positions(const Positions &positions) {
  m_positions = positions;
  m_auto_layout = positions.isEmpty();
  rebuild();
}

void DiagramScene::rename_state(const QString &from, const QString &to) {
  store_positions();
  if (m_positions.contains(from)) {
    m_positions.insert(to, m_positions.take(from));
  }
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton &&
      dynamic_cast<StateItem *>(mouseGrabberItem()) != nullptr) {
    m_auto_layout = false;
  }

  QGraphicsScene::mouseMoveEvent(event);
}

void DiagramScene::store_positions() {
  const Positions current = positions();
  for (auto it = current.begin(); it != current.end(); ++it) {
    m_positions.insert(it.key(), it.value());
  }
}

void DiagramScene::rebuild() {
  clear();

  const int count = static_cast<int>(m_spec.states.size());

  std::vector<QPointF> taken;
  if (!m_auto_layout) {
    for (const auto &name : m_spec.states) {
      const QString label = QString::fromStdString(name);
      if (m_positions.contains(label)) {
        taken.push_back(m_positions.value(label));
      }
    }
  }

  std::map<std::string, StateItem *> states;
  for (int i = 0; i < count; ++i) {
    const std::string &name = m_spec.states[i];
    const QString label = QString::fromStdString(name);

    auto *item = new StateItem(label, name == m_spec.initial,
                               contains(m_spec.finals, name));

    if (m_auto_layout) {
      item->setPos(circle_position(i, count));
    } else if (m_positions.contains(label)) {
      item->setPos(m_positions.value(label));
    } else {
      item->setPos(free_position(taken));
      taken.push_back(item->pos());
    }

    addItem(item);
    states.emplace(name, item);
  }

  std::map<std::pair<std::string, std::string>, QStringList> groups;
  for (const auto &t : m_spec.transitions) {
    if (!states.contains(t.from)) {
      continue;
    }

    for (const auto &target : t.to) {
      if (states.contains(target)) {
        groups[{t.from, target}] << QString::fromStdString(t.symbol);
      }
    }
  }

  for (const auto &[key, symbols] : groups) {
    const auto &[from, to] = key;
    const bool curved = from != to && groups.contains({to, from});
    addItem(new EdgeItem(states.at(from), states.at(to),
                         symbols.join(QStringLiteral(", ")), curved));
  }
}
