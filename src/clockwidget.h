#pragma once

#include <QTimer>

#include "scalinglabel.h"
#include "clockdef.h"
#include "blockwidget.h"


class ClockWidget : public QWidget {
Q_OBJECT
public:
    ClockWidget(const QList<ClockDef>& timeZones);

protected:

private:
    void updateTime();

    QList<ClockDef> mClockDefs;
    QList<BlockWidget*> mZoneWidgets;
    QGridLayout mLayout;
    BlockWidget mMinuteWidget;
    QTimer mTimer;
};

