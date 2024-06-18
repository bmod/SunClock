#pragma once

#include <QPaintEvent>
#include <QLabel>

class ScalingLabel : public QLabel {
Q_OBJECT
public:
    ScalingLabel() {
        updateFontSize();
    }

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

public:
    [[nodiscard]] QSize minimumSizeHint() const override;
    [[nodiscard]] QSize sizeHint() const override;

private:
    void updateFontSize();
    qreal maximumFontSize();
    qreal mFontPrecision = 0.5;
};