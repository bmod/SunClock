#include "scalinglabel.h"

void ScalingLabel::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateFontSize();
}

void ScalingLabel::paintEvent(QPaintEvent* event) {
    QLabel::paintEvent(event);
    updateFontSize();
}

void ScalingLabel::updateFontSize() {
    auto fnt = font();
    auto fontSize = maximumFontSize();
    fnt.setPointSizeF(fontSize);
    setFont(fnt);
}

qreal ScalingLabel::maximumFontSize() {
    auto fnt = font();
    auto widgetRect = contentsRect();
    auto widgetWidth = widgetRect.width();
    auto widgetHeight = widgetRect.height();
    auto currentSize = fnt.pointSizeF();

    auto step = currentSize / 2;

    if (step <= mFontPrecision) {
        step = mFontPrecision * 4;
    }

    if (text().isEmpty())
        return currentSize;

    double currentWidth = 0;
    double currentHeight = 0;
    double lastTestedSize = 0;
    while (step > mFontPrecision || currentWidth > widgetWidth || currentHeight > widgetHeight) {
        lastTestedSize = currentSize;
        fnt.setPointSizeF(currentSize);
        QFontMetricsF fm(fnt);

        auto newFontSizeRect = fm.boundingRect(widgetRect, 0, text());

        currentWidth = newFontSizeRect.width();
        currentHeight = newFontSizeRect.height();

        if (currentWidth > widgetWidth or currentHeight > widgetHeight) {
            currentSize -= step;
            if (step > mFontPrecision)
                step /= 2;

            if (currentSize <= 0)
                break;
        } else {
            currentSize += step;
        }
    }
    return lastTestedSize;
}

QSize ScalingLabel::minimumSizeHint() const {
    return {};
}

QSize ScalingLabel::sizeHint() const {
    return {};
}