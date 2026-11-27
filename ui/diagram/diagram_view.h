#ifndef DIAGRAM_VIEW_H
#define DIAGRAM_VIEW_H

#include <QGraphicsView>

class DiagramView : public QGraphicsView {
  Q_OBJECT

public:
  explicit DiagramView(QWidget *parent = nullptr);

  void fit_content();
  void ensure_content_visible();

protected:
  void wheelEvent(QWheelEvent *event) override;
};

#endif // DIAGRAM_VIEW_H
