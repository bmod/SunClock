#include "utils.h"

#include <QApplication>
#include <QImage>
#include <iostream>



void utils::ERR_AND_EXIT(const QString& message) {
    std::cerr << message.toStdString() << std::endl;
    QApplication::exit(-1);
}

void utils::ERR_AND_EXIT(const QString& message, const QString& a1) {
    ERR_AND_EXIT(QString(message).arg(a1));
}

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
        for (int x = 0, w = im.width(); x < w; x++) {
            total += im.pixelColor(x, y).valueF();
        }
    }
    return total / (im.width() * im.height());
}