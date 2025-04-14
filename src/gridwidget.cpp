#include "gridwidget.h"

#include <QMouseEvent>
#include <QtMath>

GridWidget::GridWidget(QWidget* parent) {
    setLayout(&mLayout);
}

void GridWidget::setFace(const int index, AbstractClockFace* cell) {
    const size_t oldSize = mCells.size();
    const size_t expectedLength = qMax(0, index + 1);
    if (oldSize < expectedLength) {
        const size_t dif = expectedLength - oldSize;
        for (int i = 0; i < dif; i++) {
            mCells.append(nullptr);
        }
    }

    if (const auto oldCell = mCells[index]) {
        oldCell->deleteLater();
    }

    mCells[index] = cell;
    const int row = qFloor(index / columnCount);
    const int column = qFloor(index % columnCount);
    qDebug() << "Add cell at row:" << row << ", column:" << column;
    mLayout.addWidget(cell, row, column);
    mLayout.setColumnStretch(column, 1);
    mLayout.setRowStretch(row, 1);
}

AbstractClockFace* GridWidget::face(const int index) {
    return mCells[index];
}

AbstractClockFace* GridWidget::clockFaceAt(const QPoint& pos) const {
    auto child = this->childAt(pos);
    while (child && !dynamic_cast<AbstractClockFace*>(child)) {
        child = dynamic_cast<QWidget*>(child->parent());
    }
    return dynamic_cast<AbstractClockFace*>(child);
}
void GridWidget::mouseReleaseEvent(QMouseEvent* event) {
    QWidget::mouseReleaseEvent(event);
    const auto face = clockFaceAt(event->pos());
    const int cellIndex = mCells.indexOf(face);
    qDebug() << cellIndex;
    if (cellIndex >= 0)
        tileClicked(cellIndex);
}