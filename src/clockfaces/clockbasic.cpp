#include "clockbasic.h"

#include <QFile>
#include <QGraphicsLinearLayout>
#include <qmath.h>
#include <qtutils.h>


ClockBasic::ClockBasic(const ClockData& clock) : AbstractClockFace(clock) {
    QFile file(":styleClockBasic.css");
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qFatal("Could not open style file");
    }
    setStyleSheet(file.readAll());

    setLayout(&mLayout);
    mLayout.setContentsMargins(0, 0, 0, 0);
    mLayout.setSpacing(0);
    mLayout.addWidget(&mGraphicsView);
    mGraphicsView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mGraphicsView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mGraphicsView.setScene(&mScene);

    QFont font("DM Mono");
    font.setBold(true);

    const auto colDark = QColor("#567");
    const auto colMedium = QColor("#889");
    const auto colStrong = QColor("#ECA");
    const auto colLight = QColor("#FED");

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


void ClockBasic::setTime(QDateTime timeUtc) {
    const auto dtLocal = timeUtc.toTimeZone(clock().timeZone());
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
    mDateTextItem.setHtml(dtLocal.toString("yyyy-<font color=\"#F00\">MM</font>-dd"));
    mDateTextItem.setPlainText(dtLocal.toString("yyyy-MM-dd"));

    constexpr qreal scaleXLarge = 1.618 * 1.618;
    constexpr qreal scaleLarge = 1.618 ;
    constexpr qreal scaleMedium = 1;
    constexpr qreal scaleSmall = 1/1.618;

    mDateTextItem.setScale(scaleSmall);
    mHoursTextItem.setScale(scaleXLarge);
    mColonTextItem.setScale(scaleLarge);
    mMinutesTextItem.setScale(scaleLarge);
    mTimeZoneTextItem.setScale(scaleSmall);

    qreal y = 0;

    mDateTextItem.setPos(8, y);

    y += 5;

    mHoursTextItem.setPos(0, 5);
    mColonTextItem.setPos(52, y + 7);
    mMinutesTextItem.setPos(65, y + 10);

    y += 55;

    mTimeZoneTextItem.setPos(8, y);

    fitInView(mGraphicsView, mScene.sceneRect(), 1, 1);
}

