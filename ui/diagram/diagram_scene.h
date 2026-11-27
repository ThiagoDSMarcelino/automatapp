#ifndef DIAGRAM_SCENE_H
#define DIAGRAM_SCENE_H

#include "automatas/automata_spec.h"
#include <QGraphicsScene>
#include <QHash>
#include <QPointF>

class DiagramScene : public QGraphicsScene {
  Q_OBJECT

public:
  using Positions = QHash<QString, QPointF>;

  explicit DiagramScene(QObject *parent = nullptr);

  void set_spec(const AutomataSpec &spec, bool reset_layout);
  Positions positions() const;
  void set_positions(const Positions &positions);
  void rename_state(const QString &from, const QString &to);
  bool has_manual_layout() const { return !m_auto_layout; }

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
  Positions m_positions;
  AutomataSpec m_spec;
  bool m_auto_layout = true;

  void store_positions();
  void rebuild();
};

#endif // DIAGRAM_SCENE_H
