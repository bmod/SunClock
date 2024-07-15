#include "utils.h"

#include <QApplication>
#include <QImage>
#include <config.h>
#include <iostream>
#include <qmath.h>
#include <random>
#include <vec3.h>


void utils::ERR_AND_EXIT(const QString& message) {
    std::cerr << message.toStdString() << std::endl;
    QApplication::exit(-1);
}

void utils::ERR_AND_EXIT(const QString& message, const QString& a1) { ERR_AND_EXIT(QString(message).arg(a1)); }

void utils::ERR_AND_EXIT(const QString& message, const QString& a1, const QString& a2) {
    ERR_AND_EXIT(QString(message).arg(a1, a2));
}

void utils::ASSERT_OR_EXIT(bool condition, const QString& message) {
    if (!condition) ERR_AND_EXIT(message);
}

void utils::ASSERT_OR_EXIT(bool condition, const QString& message, const QString& a1) {
    if (!condition) ERR_AND_EXIT(message, a1);
}


float utils::averageBrightness(const QImage& im) {
    float total = 0;
    for (int y = 0, h = im.height(); y < h; y++) {
        for (int x = 0, w = im.width(); x < w; x++) { total += im.pixelColor(x, y).valueF(); }
    }
    return total / (im.width() * im.height());
}

float utils::clamp(const float v, const float min, const float max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

void utils::clamp(Vec3f& p) {
    p.x = clamp(p.x);
    p.y = clamp(p.y);
    p.z = clamp(p.z);
}
void utils::addNoise(QImage& im, const float amount) {
    std::default_random_engine generator;
    std::uniform_real_distribution distribution(-amount, amount);
    const int w = im.width();
    const int len = w * im.height();
    for (int i = 0; i < len; i++) {
        const int x = i % w;
        const int y = qFloor(i / static_cast<float>(w));
        const float noise = distribution(generator);
        auto pixCol = im.pixelColor(x, y);
        const float value = clamp(pixCol.valueF() + noise);
        pixCol = pixCol.fromHsvF(pixCol.hueF(), pixCol.saturationF(), value);
        im.setPixelColor(x, y, pixCol);
    }
}

void utils::gammaCorrect(Vec3f& p, float e) {
    p.x = 1.f - pow(1 - p.x, e);
    p.y = 1.f - pow(1 - p.y, e);
    p.z = 1.f - pow(1 - p.z, e);
}
