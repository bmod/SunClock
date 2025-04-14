#pragma once

#include <QGridLayout>
#include <QLabel>

#include "clockfaces/abstractclockface.h"

class GridWidget : public QWidget {
    Q_OBJECT

public:
    GridWidget(QWidget* parent = nullptr);
    void setFace(int index, AbstractClockFace* cell);
    AbstractClockFace* face(int index);

Q_SIGNALS:
    void tileClicked(int index);

protected:
    AbstractClockFace* clockFaceAt(const QPoint& pos) const;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    int columnCount = 3;
    QGridLayout mLayout;
    QList<AbstractClockFace*> mCells;
};
