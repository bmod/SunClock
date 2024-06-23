#pragma once

#include <QTimer>
#include <QVBoxLayout>

#include "scalinglabel.h"
#include "tilewidget.h"
#include "config.h"

class TileWidget final : public QWidget {
    Q_OBJECT
public:
    explicit TileWidget(const apdata::Location* clock = nullptr);
    [[nodiscard]] const apdata::Location* location() const;
    void setHours(const QString& h);
    void setHourColor(const QColor& col);
    void setText(const QString& text);
    void setLocationColor(const QColor& col);
    [[nodiscard]] QImage image();
    void setImage(const QImage& im);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void clearImage();
    static void setForegroundColor(QWidget& widget, const QColor& color);
    const apdata::Location* mLocation = nullptr;

    QVBoxLayout mLayout;
    QImage mImage;
    ScalingLabel mHoursLabel;
    ScalingLabel mLocationLabel;
    qreal mTimeVsZoneWeight = 0.8;
};