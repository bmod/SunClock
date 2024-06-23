// modified from: https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/simulating-sky/simulating-colors-of-the-sky.html

//[header]
// Simulating the color of the sky (Nishita model).
//
// See "Display of The Earth Taking into Account Atmospheric Scattering" for more information.
//[/header]
//[compile]
// Download the acceleration.cpp and teapotdata.h file to a folder.
// Open a shell/terminal, and run the following command where the files is saved:
//
// clang++ -std=c++11 -o skycolor skycolor.cpp -O3
//
// You can use c++ if you don't use clang++
//
// Run with: ./skycolor. Open the resulting image (ppm) in Photoshop or any program
// reading PPM files.
//[/compile]
//[ignore]
// Copyright (C) 2016  www.scratchapixel.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//[/ignore]
#include "skycolor.h"

//#if defined(WIN32) || defined(_WIN32)
//
//#include "stdafx.h"
//
//#endif

#include <QApplication>
#include <QColorSpace>
#include <QPixmap>
#include <QtDebug>
#include <algorithm>
#include <cmath>
#include <random>

#ifndef M_PI
#define M_PI (3.14159265358979323846f)
#endif

float clamp(const float v, const float min = 0, const float max = 1) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

// [comment]
// The atmosphere class. Stores data about the planetory body (its radius), the atmosphere itself
// (thickness) and things such as the Mie and Rayleigh coefficients, the sun direction, etc.
// [/comment]
class Atmosphere {
public:
    explicit Atmosphere(const Vec3f sd = Vec3f(0, 1, 0), const float er = 6360e3, const float ar = 6420e3,
                        const float hr = 7994, const float hm = 1200)
        : sunDirection(sd), earthRadius(er), atmosphereRadius(ar), Hr(hr), Hm(hm) {}

    Vec3f computeIncidentLight(const Vec3f& orig, const Vec3f& dir, float tmin, float tmax) const;

    Vec3f sunDirection;    // The sun direction (normalized)
    float earthRadius;     // In the paper this is usually Rg or Re (radius ground, eart)
    float atmosphereRadius;// In the paper this is usually R or Ra (radius atmosphere)
    float Hr;              // Thickness of the atmosphere if density was uniform (Hr)
    float Hm;              // Same as above but for Mie scattering (Hm)

    static const Vec3f betaR;
    static const Vec3f betaM;
};

const Vec3f Atmosphere::betaR(3.8e-6f, 13.5e-6f, 33.1e-6f);
const Vec3f Atmosphere::betaM(21e-6f);

bool solveQuadratic(const float a, const float b, const float c, float& x1, float& x2) {
    if (b == 0) {
        // Handle special case where the two vector ray.dir and V are perpendicular
        // with V = ray.orig - sphere.centre
        if (a == 0) return false;
        x1 = 0;
        x2 = sqrtf(-c / a);
        return true;
    }
    const float discr = b * b - 4 * a * c;

    if (discr < 0) return false;

    const float q = (b < 0.f) ? -0.5f * (b - sqrtf(discr)) : -0.5f * (b + sqrtf(discr));
    x1 = q / a;
    x2 = c / q;

    return true;
}

// [comment]
// A simple routine to compute the intersection of a ray with a sphere
// [/comment]
bool raySphereIntersect(const Vec3f& orig, const Vec3f& dir, const float& radius, float& t0, float& t1) {
    // They ray dir is normalized so A = 1
    const float A = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
    const float B = 2 * (dir.x * orig.x + dir.y * orig.y + dir.z * orig.z);
    const float C = orig.x * orig.x + orig.y * orig.y + orig.z * orig.z - radius * radius;

    if (!solveQuadratic(A, B, C, t0, t1)) return false;

    if (t0 > t1) std::swap(t0, t1);

    return true;
}

// [comment]
// This is where all the magic happens. We first raymarch along the primary ray (from the camera origin
// to the point where the ray exits the atmosphere or intersect with the planetory body). For each
// sample along the primary ray, we then "cast" a light ray and raymarch along that ray as well.
// We basically shoot a ray in the direction of the sun.
// [/comment]
Vec3f Atmosphere::computeIncidentLight(const Vec3f& orig, const Vec3f& dir, float tmin, float tmax) const {
    float t0, t1;
    if (!raySphereIntersect(orig, dir, atmosphereRadius, t0, t1) || t1 < 0) return 0;
    if (t0 > tmin && t0 > 0) tmin = t0;
    if (t1 < tmax) tmax = t1;
    const uint32_t numSamples = 16;
    const uint32_t numSamplesLight = 8;
    const float segmentLength = (tmax - tmin) / numSamples;
    float tCurrent = tmin;
    Vec3f sumR(0), sumM(0);// mie and rayleigh contribution
    float opticalDepthR = 0, opticalDepthM = 0;
    const float mu = dot(
            dir,
            sunDirection);// mu in the paper which is the cosine of the angle between the sun direction and the ray direction
    const float phaseR = 3.f / (16.f * M_PI) * (1 + mu * mu);
    const float g = 0.76f;
    const float phaseM = 3.f / (8.f * M_PI) * ((1.f - g * g) * (1.f + mu * mu)) /
                   ((2.f + g * g) * pow(1.f + g * g - 2.f * g * mu, 1.5f));
    for (uint32_t i = 0; i < numSamples; ++i) {
        Vec3f samplePosition = orig + (tCurrent + segmentLength * 0.5f) * dir;
        const float height = samplePosition.length() - earthRadius;
        // compute optical depth for light
        float hr = exp(-height / Hr) * segmentLength;
        float hm = exp(-height / Hm) * segmentLength;
        opticalDepthR += hr;
        opticalDepthM += hm;
        // light optical depth
        float t0Light, t1Light;
        raySphereIntersect(samplePosition, sunDirection, atmosphereRadius, t0Light, t1Light);
        float segmentLengthLight = t1Light / numSamplesLight, tCurrentLight = 0;
        float opticalDepthLightR = 0, opticalDepthLightM = 0;
        uint32_t j;
        for (j = 0; j < numSamplesLight; ++j) {
            Vec3f samplePositionLight = samplePosition + (tCurrentLight + segmentLengthLight * 0.5f) * sunDirection;
            const float heightLight = samplePositionLight.length() - earthRadius;
            if (heightLight < 0) break;
            opticalDepthLightR += exp(-heightLight / Hr) * segmentLengthLight;
            opticalDepthLightM += exp(-heightLight / Hm) * segmentLengthLight;
            tCurrentLight += segmentLengthLight;
        }
        if (j == numSamplesLight) {
            const Vec3f tau =
                    betaR * (opticalDepthR + opticalDepthLightR) + betaM * 1.1f * (opticalDepthM + opticalDepthLightM);
            Vec3f attenuation(exp(-tau.x), exp(-tau.y), exp(-tau.z));
            sumR += attenuation * hr;
            sumM += attenuation * hm;
        }
        tCurrent += segmentLength;
    }

    // [comment]
    // We use a magic number here for the intensity of the sun (20). We will make it more
    // scientific in a future revision of this lesson/code
    // [/comment]
    return (sumR * betaR * phaseR + sumM * betaM * phaseM) * 20;
}

QImage skycolor::renderSkydome(const Vec3f& sunDir, const QSize& dim) {
    const Atmosphere atmosphere(sunDir);

    QImage im(dim.width(), dim.height(), QImage::Format_RGB32);
    const int width = im.width();
    const int height = im.height();

    Vec3f *image = new Vec3f[width * height], *p = image;
    memset(image, 0x0, sizeof(Vec3f) * width * height);
    for (int j = 0; j < height; ++j) {
        const float y = 2.f * (j + 0.5f) / float(height - 1) - 1.f;
        for (int i = 0; i < width; ++i, ++p) {
            const float x = 2.f * (i + 0.5f) / float(width - 1) - 1.f;
            const float z2 = x * x + y * y;
            if (z2 <= 1) {
                const float phi = std::atan2(y, x);
                const float theta = std::acos(1 - z2);
                Vec3f dir(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi));
                // 1 meter above sea level
                const auto light =
                        atmosphere.computeIncidentLight(Vec3f(0, atmosphere.earthRadius + 1, 0), dir, 0, kInfinity);
                im.setPixelColor(i, j, QColor::fromRgbF(light.x, light.y, light.z));
            }
        }
    }
    return im;
}


void clamp(Vec3f& p) {
    p.x = clamp(p.x);
    p.y = clamp(p.y);
    p.z = clamp(p.z);
}

void skycolor::renderCamera(const Vec3f& sunDir, QImage& im, bool toneMap, float fov, int numSamples,
                            float subjectHeight, float stretchDown) {
    const Atmosphere atmosphere(sunDir);

    const float width = im.width();
    const float height = im.height();
    const float aspectRatio = width / height;
    const float angle = std::tan(fov * M_PI / 180 * 0.5f);
    const int numPixelSamples = numSamples;
    const Vec3f orig(0, atmosphere.earthRadius + subjectHeight, 0);// camera position
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0, 1);// to generate random floats in the range [0:1]


    for (int yy = 0; yy < height; ++yy) {
        const auto ty = yy * stretchDown;
        const auto y = qRound(ty);
        for (unsigned x = 0; x < width; ++x) {
            Vec3f p;
            for (unsigned m = 0; m < numPixelSamples; ++m) {
                for (unsigned n = 0; n < numPixelSamples; ++n) {
                    const float rayx = (2 * (x + (m + distribution(generator)) / numPixelSamples) / width - 1) *
                                       aspectRatio * angle;
                    const float rayy = (1 - (y + (n + distribution(generator)) / numPixelSamples) / height * 2) * angle;
                    Vec3f dir(rayx, rayy, -1);
                    normalize(dir);
                    // [comment]
                    // Does the ray intersect the planetory body? (the intersection test is against the Earth here
                    // not against the atmosphere). If the ray intersects the Earth body and that the intersection
                    // is ahead of us, then the ray intersects the planet in 2 points, t0 and t1. But we
                    // only want to comupute the atmosphere between t=0 and t=t0 (where the ray hits
                    // the Earth first). If the viewing ray doesn't hit the Earth, or course the ray
                    // is then bounded to the range [0:INF]. In the method computeIncidentLight() we then
                    // compute where this primary ray intersects the atmosphere and we limit the max t range
                    // of the ray to the point where it leaves the atmosphere.
                    // [/comment]
                    float t0, t1, tMax = kInfinity;
                    if (raySphereIntersect(orig, dir, atmosphere.earthRadius, t0, t1) && t1 > 0)
                        tMax = std::max(0.f, t0);
                    // [comment]
                    // The *viewing or camera ray* is bounded to the range [0:tMax]
                    // [/comment]
                    p += atmosphere.computeIncidentLight(orig, dir, 0, tMax);
                }
            }
            p *= 1.f / (numPixelSamples * numPixelSamples);

            const float e = 2.4;

            clamp(p);

            // Gamma correct
            p.x = 1.f - pow(1 - p.x, e);
            p.y = 1.f - pow(1 - p.y, e);
            p.z = 1.f - pow(1 - p.z, e);

            im.setPixelColor(x, qRound(ty / stretchDown), QColor::fromRgbF(p.x, p.y, p.z));
        }
    }

    if (!toneMap) return;

    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            auto col = im.pixelColor(i, j);
            Vec3f p(col.redF(), col.greenF(), col.blueF());
            if (toneMap) {
                // Apply tone mapping function
                for (int c = 0; c < 3; c++)
                    p[c] = p[c] < 1.413f ? pow(p[c] * 0.38317f, 1.0f / 2.2f) : 1.0f - exp(-p[c]);
            }
            p *= 1.4;

            im.setPixelColor(i, j, QColor::fromRgbF(p.x, p.y, p.z));
        }
    }
}
