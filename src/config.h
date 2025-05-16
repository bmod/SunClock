#pragma once
#include <QJsonDocument>

#include "clockdata.h"

#include <airportdb.h>


class Config
{
public:
    Config();
    const std::vector<std::unique_ptr<ClockData>>& clocks() const;

private:
    AirportDB mAirportDB;
    QJsonDocument mDoc;
    std::vector<std::unique_ptr<ClockData>> mClocks;
};
