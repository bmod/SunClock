#pragma once


#include <QVBoxLayout>
#include "scalinglabel.h"

class BlockWidget : public QWidget {
Q_OBJECT
public:
    BlockWidget();
    void setTime(const QString& time);
    void setText(const QString& text);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QVBoxLayout mLayout;
    ScalingLabel mTimeLabel;
    ScalingLabel mZoneLabel;
    qreal mTimeVsZoneWeight = 0.8;
};