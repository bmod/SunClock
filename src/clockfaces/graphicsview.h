#pragma once

#include <QGraphicsView>
#include <QMouseEvent>

class GraphicsView : public QGraphicsView {
    Q_OBJECT
protected:
    void mouseReleaseEvent(QMouseEvent* event) override {
        event->accept();
        QGraphicsView::mouseReleaseEvent(event);

    }
};