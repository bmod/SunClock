#include "blockwidget.h"

#include <QPainter>


BlockWidget::BlockWidget() {
    mLayout.setContentsMargins({});
    mLayout.setSpacing(0);
    setLayout(&mLayout);

    mTimeLabel.setAlignment(Qt::AlignRight);
    mLayout.addWidget(&mTimeLabel, (int) (mTimeVsZoneWeight * 1000));
    mLayout.addWidget(&mZoneLabel, (int) ((1 -mTimeVsZoneWeight) * 1000));
}

void BlockWidget::setTime(const QString& time) {
    mTimeLabel.setText(time);
}

void BlockWidget::setText(const QString& text) {
    mZoneLabel.setText(text);
}

void BlockWidget::paintEvent(QPaintEvent* event) {
    QWidget::paintEvent(event);
    QPainter ptr(this);
    ptr.setPen(Qt::red);
    ptr.setBrush(Qt::green);
    auto rec = rect().adjusted(0, 0, -1, -1);
    const int radius = 8;
    ptr.drawRoundedRect(rec, radius, radius);
}