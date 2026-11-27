#ifndef STATE_ITEM_H
#define STATE_ITEM_H

#include <QGraphicsItem>
#include <vector>

class EdgeItem;

class StateItem : public QGraphicsItem {
public:
  static constexpr qreal Radius = 24;

  StateItem(const QString &name, bool initial, bool final_state);

  const QString &name() const { return m_name; }
  void add_edge(EdgeItem *edge);

  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

protected:
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant &value) override;

private:
  QString m_name;
  bool m_initial;
  bool m_final;
  std::vector<EdgeItem *> m_edges;
};

#endif // STATE_ITEM_H
