#pragma once
#include <QJsonDocument>

#include "clockdata.h"

class QJsonObject;


class Config
{
public:
    Config();
    const std::vector<std::unique_ptr<ClockData>>& clocks() const;

private:

    QJsonDocument mDoc;
    std::vector<std::unique_ptr<ClockData>> mClocks;
};
