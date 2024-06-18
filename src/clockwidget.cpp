#include "clockwidget.h"


ClockWidget::ClockWidget(const QList<ClockDef>& timeZones) : mClockDefs(timeZones) {
    const int margin = 4;
    mLayout.setContentsMargins(margin, margin, margin, margin);
    mLayout.setSpacing(8);
    setLayout(&mLayout);

    for (auto tz: timeZones) {
        auto block = new BlockWidget();
        mZoneWidgets << block;
    }

    mLayout.addWidget(mZoneWidgets[0], 0, 0);
    mLayout.addWidget(mZoneWidgets[1], 0, 1);
    mLayout.addWidget(mZoneWidgets[2], 1, 0);
    mLayout.addWidget(mZoneWidgets[3], 1, 1);
    mLayout.addWidget(&mMinuteWidget, 0, 2);

    connect(&mTimer, &QTimer::timeout, this, &ClockWidget::updateTime);
    mTimer.setInterval(1000);
    mTimer.start();

    updateTime();
}

void ClockWidget::updateTime() {
    auto minuteStr = QDateTime::currentDateTimeUtc().toString(":mm");
    mMinuteWidget.setTime(minuteStr);

    for (int i = 0, len = mZoneWidgets.count(); i < len; i++) {
        auto def = mClockDefs[i];
        auto widget = mZoneWidgets[i];
        auto dt = QDateTime::currentDateTimeUtc().toTimeZone(def.timeZone());
        auto hourStr = dt.time().toString("HH");
        widget->setTime(hourStr);
        widget->setText(def.displayName());
    }
}

