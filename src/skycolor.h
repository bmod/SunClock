#pragma once

#include <QImage>
#include "vec3.h"

namespace skycolor {

    QImage renderCamera(const Vec3f& sunDir, const QSize& dim = QSize(320, 240),
                        bool toneMap = false, float fov = 65, int numSamples = 2, float subjectHeight = 1);

    QImage renderSkydome(const Vec3f& sunDir, const QSize& dim = QSize(256, 256));
}