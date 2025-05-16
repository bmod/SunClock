#pragma once

#include <QImage>

QImage sRenderImage(const QSize& size, const std::chrono::system_clock::time_point& time, float lat, float lng);