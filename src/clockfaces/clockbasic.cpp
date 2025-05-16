#include "clockbasic.h"

#include <QFile>
#include <QGraphicsLinearLayout>
#include <qmath.h>
#include <qtutils.h>
#include <renderimage.h>


ClockBasic::ClockBasic(ClockData& clock) : AbstractClockFace(clock), mClockData(clock) {
    QFile file(":styleClockBasic.css");
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qFatal("Could not open style file");
    }
    setStyleSheet(file.readAll());

    mItems = {
            &mHoursTextItem, &mColonTextItem, &mMinutesTextItem, &mDateTextItem, &mTimeZoneTextItem,
    };

    setLayout(&mLayout);
    mLayout.setContentsMargins(0, 0, 0, 0);
    mLayout.setSpacing(0);
    mLayout.addWidget(&mGraphicsView);
    mGraphicsView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mGraphicsView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mGraphicsView.setScene(&mScene);
    mGraphicsView.setBackgroundBrush(QBrush(Qt::black));
    // mGraphicsView.setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));
    mGraphicsView.setViewport(new QGLWidget(QGLFormat(QGL::SingleBuffer)));

    QFont font("DM Mono");
    font.setBold(true);

    const auto colDark = QColor("#79B");
    const auto colMedium = QColor("#889");
    const auto colStrong = QColor("#ECA");
    const auto colLight = QColor("#FED");

    mScene.addItem(&mBackgroundItem);

    mHoursTextItem.setDefaultTextColor(colLight);
    mHoursTextItem.setFont(font);
    mScene.addItem(&mHoursTextItem);

    mColonTextItem.setDefaultTextColor(colDark);
    mColonTextItem.setFont(font);
    mScene.addItem(&mColonTextItem);

    mMinutesTextItem.setDefaultTextColor(colMedium);
    mMinutesTextItem.setFont(font);
    mScene.addItem(&mMinutesTextItem);

    mTimeZoneTextItem.setDefaultTextColor(colStrong);
    mTimeZoneTextItem.setFont(font);
    mScene.addItem(&mTimeZoneTextItem);

    mDateTextItem.setFont(font);
    mDateTextItem.setDefaultTextColor(colDark);
    mScene.addItem(&mDateTextItem);
}


void ClockBasic::setTime(const QDateTime timeUtc) {
    const QDateTime dtLocal = timeUtc.toTimeZone(clockData().timeZone());
    const long secs = qFloor(dtLocal.toSecsSinceEpoch());
    const bool isEven = secs % 2;
    const QString colon = isEven ? ":" : " ";
    const QString tpl("HH%1mm");

    const auto tzId = dtLocal.timeZone().id();
    auto locationName = QString(tzId).splitRef("/").at(1).toString();
    locationName = locationName.replace("_", " ");

    mTimeZoneTextItem.setPlainText(locationName);
    mColonTextItem.setPlainText(":");
    mColonTextItem.setVisible(isEven);
    mHoursTextItem.setPlainText(dtLocal.toString("HH"));
    mMinutesTextItem.setPlainText(dtLocal.toString("mm"));
    mDateTextItem.setHtml(dtLocal.toString("yyyy <font color=\"#F00\">MM</font> dd"));
    // mDateTextItem.setPlainText(dtLocal.toString("yyyy-MM-dd"));

    constexpr qreal scaleXLarge = 1.618 * 1.618;
    constexpr qreal scaleLarge = 1.618;
    // constexpr qreal scaleMedium = 1;
    constexpr qreal scaleSmall = 1 / 1.618;

    mDateTextItem.setScale(scaleSmall);
    mHoursTextItem.setScale(scaleXLarge);
    mColonTextItem.setScale(scaleLarge);
    mMinutesTextItem.setScale(scaleLarge);
    mTimeZoneTextItem.setScale(scaleSmall);

    qreal y = 0;

    mDateTextItem.setPos(8, y);

    y += 5;

    mHoursTextItem.setPos(0, 5);
    mColonTextItem.setPos(52, y + 9);
    mMinutesTextItem.setPos(65, y + 10);

    y += 55;

    mTimeZoneTextItem.setPos(8, y);

    const auto itemsRect = itemsBounds();


    // const auto scaleX = mBackgroundItem.boundingRect().width() / static_cast<float>(viewRect.width());
    fitInView(mGraphicsView, itemsRect, 1, 1);

    // float tx = mGraphicsView.transform().m31();
    // float ty = mGraphicsView.transform().m32();
    // float sx = mGraphicsView.transform().m11();
    const float sy = mGraphicsView.transform().m22();

    const auto size = mGraphicsView.viewport()->geometry().size();

    const GeoLocation loc = clockData().location();

    const std::chrono::duration<int64_t> duration(secs);
    const std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<int64_t>> timePoint(duration);

    const auto im = sRenderImage(size, timePoint, loc.latitude, loc.longitude);
    const auto pm = QPixmap::fromImage(im);

    mBackgroundItem.setPixmap(pm);
    mBackgroundItem.setScale(1 / sy);
}

QRectF ClockBasic::itemsBounds() const {
    if (mItems.isEmpty())
        return {};

    QRectF rect = mItems[0]->boundingRect();
    for (int i = 0; i < mItems.size(); i++) {
        rect = mItems[i]->boundingRect().united(rect);
    }
    return rect;
}
