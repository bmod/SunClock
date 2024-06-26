#pragma once

#include <QImage>
#include <vec3.h>

namespace utils {
    void ERR_AND_EXIT(const QString& message);
    void ERR_AND_EXIT(const QString& message, const QString& a1);
    void ERR_AND_EXIT(const QString& message, const QString& a1, const QString& a2);
    void ASSERT_OR_EXIT(bool condition, const QString& message);
    void ASSERT_OR_EXIT(bool condition, const QString& message, const QString& a1);

    float averageBrightness(const QImage& im);
    float clamp(float v, float min = 0, float max = 1);
    void clamp(Vec3f& p);
    void addNoise(QImage& im, float amount);
    void gammaCorrect(Vec3f& p, float e);
}// namespace utils