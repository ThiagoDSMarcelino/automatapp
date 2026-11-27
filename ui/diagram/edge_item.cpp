#include "diagram/edge_item.h"
#include "diagram/state_item.h"

#include <QApplication>
#include <QFontMetricsF>
#include <QPainter>
#include <QPainterPathStroker>
#include <QPalette>
#include <QtMath>
#include <algorithm>

namespace {

constexpr qreal ArrowHeadSize = 9;
constexpr qreal LabelOffset = 10;

QPointF unit(const QPointF &v) {
  const qreal length = std::hypot(v.x(), v.y());
  return length < 1e-6 ? QPointF(0, 0) : v / length;
}

QPointF on_circle(const QPointF &center, qreal degrees) {
  const qreal radians = qDegreesToRadians(degrees);
  return center +
         StateItem::Radius * QPointF(std::cos(radians), -std::sin(radians));
}

QPolygonF arrow_head(const QPointF &tip, const QPointF &direction) {
  const QPointF u = unit(direction);
  const QPointF normal(-u.y(), u.x());
  const QPointF base = tip - u * ArrowHeadSize;
  return QPolygonF{tip, base + normal * (ArrowHeadSize / 2),
                   base - normal * (ArrowHeadSize / 2)};
}

} // namespace

EdgeItem::EdgeItem(StateItem *from, StateItem *to, const QString &label,
                   bool curved)
    : m_from(from), m_to(to), m_label(label), m_curved(curved) {
  from->add_edge(this);
  if (to != from) {
    to->add_edge(this);
  }
  adjust();
}

void EdgeItem::adjust() {
  prepareGeometryChange();
  m_path = QPainterPath();

  const QPointF p1 = m_from->pos();
  const QPointF p2 = m_to->pos();
  constexpr qreal r = StateItem::Radius;

  if (m_from == m_to) {
    const QPointF start = on_circle(p1, 115);
    const QPointF end = on_circle(p1, 65);
    const QPointF c1 = p1 + QPointF(-r * 0.9, -r * 2.6);
    const QPointF c2 = p1 + QPointF(r * 0.9, -r * 2.6);

    m_path.moveTo(start);
    m_path.cubicTo(c1, c2, end);
    m_head = arrow_head(end, end - c2);
    m_label_pos = p1 + QPointF(0, -r * 2.2 - LabelOffset);
    return;
  }

  const QPointF delta = p2 - p1;
  const qreal length = std::hypot(delta.x(), delta.y());
  if (length < 2 * r) {
    m_head.clear();
    m_label_pos = (p1 + p2) / 2;
    return;
  }

  const QPointF normal = QPointF(-delta.y(), delta.x()) / length;
  const qreal bend = m_curved ? std::clamp(length * 0.2, 20.0, 60.0) : 0.0;
  const QPointF control = (p1 + p2) / 2 + normal * bend;

  const QPointF start = p1 + unit(control - p1) * r;
  const QPointF end = p2 + unit(control - p2) * r;

  m_path.moveTo(start);
  m_path.quadTo(control, end);
  m_head = arrow_head(end, end - control);
  m_label_pos =
      0.25 * start + 0.5 * control + 0.25 * end + normal * LabelOffset;
}

QRectF EdgeItem::label_rect() const {
  const QFontMetricsF metrics(QApplication::font());
  QRectF rect = metrics.boundingRect(m_label);
  rect.moveCenter(m_label_pos);
  return rect.adjusted(-2, -1, 2, 1);
}

QRectF EdgeItem::boundingRect() const {
  return m_path.boundingRect()
      .united(m_head.boundingRect())
      .united(label_rect())
      .adjusted(-3, -3, 3, 3);
}

QPainterPath EdgeItem::shape() const {
  QPainterPathStroker stroker;
  stroker.setWidth(8);
  QPainterPath path = stroker.createStroke(m_path);
  path.addRect(label_rect());
  return path;
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *,
                     QWidget *) {
  if (m_path.isEmpty()) {
    return;
  }

  const QColor color = QApplication::palette().color(QPalette::Text);

  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(QPen(color, 1.5));
  painter->setBrush(Qt::NoBrush);
  painter->drawPath(m_path);

  painter->setBrush(color);
  painter->drawPolygon(m_head);

  painter->drawText(label_rect(), Qt::AlignCenter, m_label);
}
