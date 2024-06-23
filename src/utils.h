#pragma once

#include <QImage>

namespace utils {
    void ERR_AND_EXIT(const QString& message);
    void ERR_AND_EXIT(const QString& message, const QString& a1);
    void ERR_AND_EXIT(const QString& message, const QString& a1, const QString& a2);
    void ASSERT_OR_EXIT(bool condition, const QString& message);
    void ASSERT_OR_EXIT(bool condition, const QString& message, const QString& a1);

    float averageBrightness(const QImage& im);
}// namespace utils