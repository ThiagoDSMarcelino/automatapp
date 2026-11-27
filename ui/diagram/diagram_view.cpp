#include "diagram/diagram_view.h"

#include <QWheelEvent>
#include <QtMath>
#include <algorithm>

namespace {

constexpr qreal MinScale = 0.2;
constexpr qreal MaxScale = 4.0;
constexpr qreal ZoomStep = 1.15;

} // namespace

DiagramView::DiagramView(QWidget *parent) : QGraphicsView(parent) {
  setRenderHint(QPainter::Antialiasing);
  setDragMode(ScrollHandDrag);
  setTransformationAnchor(AnchorUnderMouse);
  setViewportUpdateMode(BoundingRectViewportUpdate);
}

void DiagramView::fit_content() {
  if (scene() == nullptr || scene()->items().isEmpty()) {
    resetTransform();
    return;
  }

  const QRectF rect = scene()->itemsBoundingRect().adjusted(-40, -40, 40, 40);
  fitInView(rect, Qt::KeepAspectRatio);
  if (transform().m11() > 1.0) {
    resetTransform();
  }
  centerOn(rect.center());
}

void DiagramView::ensure_content_visible() {
  if (scene() == nullptr || scene()->items().isEmpty()) {
    return;
  }

  const QRectF visible = mapToScene(viewport()->rect()).boundingRect();
  if (!visible.contains(scene()->itemsBoundingRect())) {
    fit_content();
  }
}

void DiagramView::wheelEvent(QWheelEvent *event) {
  const qreal steps = event->angleDelta().y() / 120.0;
  if (steps == 0) {
    return;
  }

  const qreal current = transform().m11();
  const qreal target =
      std::clamp(current * std::pow(ZoomStep, steps), MinScale, MaxScale);
  scale(target / current, target / current);
  event->accept();
}
