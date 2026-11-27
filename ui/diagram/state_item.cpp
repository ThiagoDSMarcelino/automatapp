#include "diagram/state_item.h"
#include "diagram/edge_item.h"

#include <QApplication>
#include <QPainter>
#include <QPalette>
#include <QStyleOptionGraphicsItem>

namespace {

constexpr qreal FinalGap = 4;
constexpr qreal InitialArrowLength = 28;
constexpr qreal ArrowHeadSize = 8;

} // namespace

StateItem::StateItem(const QString &name, bool initial, bool final_state)
    : m_name(name), m_initial(initial), m_final(final_state) {
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
  setCursor(Qt::OpenHandCursor);
  setZValue(1);
}

void StateItem::add_edge(EdgeItem *edge) { m_edges.push_back(edge); }

QRectF StateItem::boundingRect() const {
  const qreal left = m_initial ? Radius + InitialArrowLength : Radius;
  return QRectF(-left, -Radius, left + Radius, 2 * Radius)
      .adjusted(-2, -2, 2, 2);
}

QPainterPath StateItem::shape() const {
  QPainterPath path;
  path.addEllipse(QPointF(0, 0), Radius, Radius);
  return path;
}

void StateItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                      QWidget *) {
  const QPalette palette = QApplication::palette();
  const bool selected = option->state & QStyle::State_Selected;

  painter->setRenderHint(QPainter::Antialiasing);
  painter->setPen(QPen(selected ? palette.color(QPalette::Highlight)
                                : palette.color(QPalette::Text),
                       selected ? 2.5 : 1.5));
  painter->setBrush(palette.color(QPalette::Base));
  painter->drawEllipse(QPointF(0, 0), Radius, Radius);

  if (m_final) {
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(QPointF(0, 0), Radius - FinalGap, Radius - FinalGap);
  }

  if (m_initial) {
    const QPointF tip(-Radius, 0);
    painter->drawLine(tip - QPointF(InitialArrowLength, 0), tip);
    painter->setBrush(palette.color(QPalette::Text));
    painter->drawPolygon(
        QPolygonF{tip, tip + QPointF(-ArrowHeadSize, -ArrowHeadSize / 2),
                  tip + QPointF(-ArrowHeadSize, ArrowHeadSize / 2)});
  }

  painter->setPen(palette.color(QPalette::Text));
  painter->drawText(QRectF(-Radius, -Radius, 2 * Radius, 2 * Radius),
                    Qt::AlignCenter, m_name);
}

QVariant StateItem::itemChange(GraphicsItemChange change,
                               const QVariant &value) {
  if (change == ItemPositionHasChanged) {
    for (auto *edge : m_edges) {
      edge->adjust();
    }
  }

  return QGraphicsItem::itemChange(change, value);
}
