#pragma once

#include "vec3.h"
#include <QImage>

namespace skycolor {

    void renderCamera(const Vec3f& sunDir, QImage& im, bool toneMap = false, float fov = 65, int numSamples = 2,
                      float subjectHeight = 1, float stretchDown = 0.6, float gamma = 1.2);

    QImage renderSkydome(const Vec3f& sunDir, const QSize& dim = QSize(256, 256));

}// namespace skycolor