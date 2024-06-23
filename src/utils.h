#pragma once

#include <QImage>

namespace utils {
    void ERR_AND_EXIT(const QString& message);
    void ERR_AND_EXIT(const QString& message, const QString& a1);
    void ERR_AND_EXIT(const QString& message, const QString& a1, const QString& a2);

    float averageBrightness(const QImage& im);
}// namespace utils