#ifndef EDGE_ITEM_H
#define EDGE_ITEM_H

#include <QGraphicsItem>
#include <QPainterPath>

class StateItem;

class EdgeItem : public QGraphicsItem {
public:
  EdgeItem(StateItem *from, StateItem *to, const QString &label, bool curved);

  void adjust();

  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget) override;

private:
  StateItem *m_from;
  StateItem *m_to;
  QString m_label;
  bool m_curved;

  QPainterPath m_path;
  QPolygonF m_head;
  QPointF m_label_pos;

  QRectF label_rect() const;
};

#endif // EDGE_ITEM_H
