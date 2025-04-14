#include "qtutils.h"

void fitInView(QGraphicsView& view, const QRectF& rect, int xMargin, int yMargin) {
    view.fitInView(rect, Qt::KeepAspectRatio);
    view.centerOn(rect.center().x(), rect.center().y());
    // view.ensureVisible(rect, xMargin, yMargin);
    // Qt::KeepAspectRatio
}