uniform sampler2D texture;
uniform vec3 sunDir;
uniform vec2 resolution;

#define M_PI 3.14159265358979323846

int numSamples = 4;
int numSamplesLight = 8;
float atmosphereRadius = 6420.0e3;
float earthRadius = 6360.0e3;
float Hr = 7994.0;
float Hm = 1200.0;
vec3 betaR = vec3(3.8e-6, 13.5e-6, 33.1e-6);
vec3 betaM = vec3(21e-6);
float kInfinity = 3.40282e+38;

float clamp01(float v) {
    if (v < 0.0) return 0.0;
    if (v > 1.0) return 1.0;
    return v;
}

vec3 clamp01(vec3 v) {
    return vec3(clamp01(v.x), clamp01(v.y), clamp01(v.z));
}

float random(vec2 st)
{
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float applyToneMap(float p) {
    return p < 1.413f ? pow(p * 0.38317, 1.0 / 2.2) : 1.0 - exp(-p);
}
vec3 gammaCorrect(inout vec3 p, float e) {
    return vec3(
        1.0 - pow(1.0 - p.x, e),
        1.0 - pow(1.0 - p.y, e),
        1.0 - pow(1.0 - p.z, e)
    );
}

bool solveQuadratic(float a, float b, float c, inout float x1, inout float x2) {
    if (b == 0.0) {
        // Handle special case where the two vector ray.dir and V are perpendicular
        // with V = ray.orig - sphere.centre
        if (a == 0.0) {
            return false;
        }
        x1 = 0.0;
        x2 = sqrt(-c / a);
        return true;
    }
    float discr = b * b - 4.0 * a * c;

    if (discr < 0.0) {
        return false;
    }

    float q = (b < 0.0) ? -0.5 * (b - sqrt(discr)) : -0.5 * (b + sqrt(discr));
    x1 = q / a;
    x2 = c / q;

    return true;
}

bool raySphereIntersect(vec3 orig, vec3 dir, float radius, inout float t0, inout float t1) {
    // They ray dir is normalized so A = 1
    float A = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
    float B = 2.0 * (dir.x * orig.x + dir.y * orig.y + dir.z * orig.z);
    float C = orig.x * orig.x + orig.y * orig.y + orig.z * orig.z - radius * radius;

    if (!solveQuadratic(A, B, C, t0, t1)) {
        return false;
    }

    if (t0 > t1) {
        float tmp = t1;
        t0 = t1;
        t1 = tmp;
    }

    return true;
}

vec3 computeIncidentLight(vec3 orig, vec3 dir, float tmin, float tmax) {
//    vec3 sunDir = vec3(1, 1, 0);
    sunDir = normalize(sunDir);
    float t0;
    float t1;
    if (!raySphereIntersect(orig, dir, atmosphereRadius, t0, t1) || t1 < 0.0) {
        return vec3(0.0);
    }
    if (t0 > tmin && t0 > 0.0) {
        tmin = t0;
    }
    if (t1 < tmax) {
        tmax = t1;
    }
    float segmentLength = (tmax - tmin) / float(numSamples);
    float tCurrent = tmin;
    vec3 sumR = vec3(0.0);
    vec3 sumM = vec3(0.0);// mie and rayleigh contribution
    float opticalDepthR = 0.0;
    float opticalDepthM = 0.0;
    // mu in the paper which is the cosine of the angle between the sun direction and the ray direction
    float mu = dot(dir, sunDir);
    float phaseR = 3.0 / (16.0 * M_PI) * (1.0 + mu * mu);
    float g = 0.76;
    float phaseM = 3.0 / (8.0 * M_PI) * ((1.0 - g * g) * (1.0 + mu * mu)) /
    ((2.0 + g * g) * pow(1.0 + g * g - 2.0 * g * mu, 1.5));
    for (int i = 0; i < numSamples; ++i) {
        vec3 samplePosition = orig + (tCurrent + segmentLength * 0.5) * dir;
        float height = float(samplePosition.length()) - earthRadius;
        // compute optical depth for light
        float hr = exp(-height / Hr) * segmentLength;
        float hm = exp(-height / Hm) * segmentLength;
        opticalDepthR += hr;
        opticalDepthM += hm;
        // light optical depth
        float t0Light, t1Light;
        raySphereIntersect(samplePosition, sunDir, atmosphereRadius, t0Light, t1Light);
        float segmentLengthLight = t1Light / float(numSamplesLight);
        float tCurrentLight = 0.0;
        float opticalDepthLightR = 0.0;
        float opticalDepthLightM = 0.0;
        int j=0;
        for (; j < numSamplesLight; ++j) {
            vec3 samplePositionLight = samplePosition + (tCurrentLight + segmentLengthLight * 0.5) * sunDir;
            float heightLight = float(samplePositionLight.length()) - earthRadius;
            if (heightLight < 0.0) {
                break;
            }
            opticalDepthLightR += exp(-heightLight / Hr) * segmentLengthLight;
            opticalDepthLightM += exp(-heightLight / Hm) * segmentLengthLight;
            tCurrentLight += segmentLengthLight;
        }
        if (j == numSamplesLight) {
            vec3 tau = betaR * (opticalDepthR + opticalDepthLightR) + betaM * 1.1 * (opticalDepthM + opticalDepthLightM);
            vec3 attenuation = vec3(exp(-tau.x), exp(-tau.y), exp(-tau.z));
            sumR += attenuation * hr;
            sumM += attenuation * hm;
        }
        tCurrent += segmentLength;
    }

    // [comment]
    // We use a magic number here for the intensity of the sun (20). We will make it more
    // scientific in a future revision of this lesson/code
    // [/comment]
    return (sumR * betaR * phaseR + sumM * betaM * phaseM) * 20.0;
}

void main()
{
    gl_FragColor = vec4(0.1, 0.2, 0.3, 1.0);
    return;

    bool toneMap = true;
    float fov = 90.0;
    float stretchDown = 0.4;
    float subjectHeight = 1.0;
    float gamma = 1.2;
    float width = resolution.x;
    float height = resolution.y;

    vec2 uv = gl_TexCoord[0].xy;

    float aspectRatio = width / height;
    float angle = tan(fov * M_PI / 180.0 * 0.5);

    float x = uv.x * float(width);
    float y = uv.y * float(height) * stretchDown;

    vec3 orig = vec3(0, earthRadius + subjectHeight, 0);

    vec3 p;
    int i = 0;
    for (int m=0; m < numSamples; m++) {
        for (int n=0; n < numSamples; n++) {
            float rayx = (2.0 * (float(x) + (float(m) + random(vec2(float(i + m), 0.0))) / float(numSamples)) / float(width) - 1.0) * aspectRatio * angle;
            float rayy = (1.0 - (float(y) + (float(n) + random(vec2(float(i + n), 0.0))) / float(numSamples)) / float(height) * 2.0) * angle;
            vec3 dir = vec3(rayx, rayy, -1);
            normalize(dir);
            float t0, t1, tMax = kInfinity;
            if (raySphereIntersect(orig, dir, earthRadius, t0, t1) && t1 > 0.0) {
                tMax = max(0.0, t0);
            }
            p += computeIncidentLight(orig, dir, 0.0, tMax);
            i++;
        }
    }

    p *= 1.0 / float(numSamples * numSamples);

    p = clamp01(p);// never exceed range, will look broken
    p = gammaCorrect(p, gamma);
    if (toneMap) {
       p = applyToneMap(p);
    }
    p *= 1.4;// make things a little brighter for sunsets
    p = clamp01(p);// never exceed range, will look broken

    gl_FragColor = vec4(p, 1.0);
}