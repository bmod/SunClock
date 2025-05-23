/*
Copy-pasted from https://github.com/wwwtyro/glsl-atmosphere/tree/master
*/

uniform vec3 sunDir;
varying vec3 vPosition;
uniform vec2 skyRangeX;
uniform vec2 skyRangeY;

#define M_PI 3.14159265358979323846

//#define M_PI 3.141592
#define iSteps 32
#define jSteps 16

vec2 rsi(vec3 r0, vec3 rd, float sr) {
    // ray-sphere intersection that assumes
    // the sphere is centered at the origin.
    // No intersection when result.x > result.y
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, r0);
    float c = dot(r0, r0) - (sr * sr);
    float d = (b * b) - 4.0 * a * c;
    if (d < 0.0) return vec2(1e5, -1e5);
    return vec2(
    (-b - sqrt(d)) / (2.0 * a),
    (-b + sqrt(d)) / (2.0 * a)
    );
}

// vibrance in range [-1, 1].. usually, do whaterver of course
vec3 vibrance(vec3 color, float amount) {
    float luminance = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;
    float mn = min(min(color.r, color.g), color.b);
    float mx = max(max(color.r, color.g), color.b);
    float sat = (1.0 - (mx - mn)) * (1.0 - mx) * luminance * 5.0;
    vec3 lightness = vec3((mn + mx) / 2.0);

    if (amount >= 0.0) {
        color = mix(color, mix(color, lightness, -amount), sat);
    } else {
        color = mix(color, lightness, (1.0 - lightness) * (1.0 - amount) / 2.0 * abs(amount));
    }
    return color;
}

vec3 atmosphere(vec3 r, vec3 r0, vec3 pSun, float iSun, float rPlanet, float rAtmos, vec3 kRlh, float kMie, float shRlh, float shMie, float g) {
    // Normalize the sun and view directions.
    pSun = normalize(pSun);
    r = normalize(r);

    // Calculate the step size of the primary ray.
    vec2 p = rsi(r0, r, rAtmos);
    if (p.x > p.y) return vec3(0, 0, 0);
    p.y = min(p.y, rsi(r0, r, rPlanet).x);
    float iStepSize = (p.y - p.x) / float(iSteps);

    // Initialize the primary ray time.
    float iTime = 0.0;

    // Initialize accumulators for Rayleigh and Mie scattering.
    vec3 totalRlh = vec3(0, 0, 0);
    vec3 totalMie = vec3(0, 0, 0);

    // Initialize optical depth accumulators for the primary ray.
    float iOdRlh = 0.0;
    float iOdMie = 0.0;

    // Calculate the Rayleigh and Mie phases.
    float mu = dot(r, pSun);
    float mumu = mu * mu;
    float gg = g * g;
    float pRlh = 3.0 / (16.0 * M_PI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * M_PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

    // Sample the primary ray.
    for (int i = 0; i < iSteps; i++) {

        // Calculate the primary ray sample position.
        vec3 iPos = r0 + r * (iTime + iStepSize * 0.5);

        // Calculate the height of the sample.
        float iHeight = length(iPos) - rPlanet;

        // Calculate the optical depth of the Rayleigh and Mie scattering for this step.
        float odStepRlh = exp(-iHeight / shRlh) * iStepSize;
        float odStepMie = exp(-iHeight / shMie) * iStepSize;

        // Accumulate optical depth.
        iOdRlh += odStepRlh;
        iOdMie += odStepMie;

        // Calculate the step size of the secondary ray.
        float jStepSize = rsi(iPos, pSun, rAtmos).y / float(jSteps);

        // Initialize the secondary ray time.
        float jTime = 0.0;

        // Initialize optical depth accumulators for the secondary ray.
        float jOdRlh = 0.0;
        float jOdMie = 0.0;

        // Sample the secondary ray.
        for (int j = 0; j < jSteps; j++) {

            // Calculate the secondary ray sample position.
            vec3 jPos = iPos + pSun * (jTime + jStepSize * 0.5);

            // Calculate the height of the sample.
            float jHeight = length(jPos) - rPlanet;

            // Accumulate the optical depth.
            jOdRlh += exp(-jHeight / shRlh) * jStepSize;
            jOdMie += exp(-jHeight / shMie) * jStepSize;

            // Increment the secondary ray time.
            jTime += jStepSize;
        }

        // Calculate attenuation.
        vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));

        // Accumulate scattering.
        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;

        // Increment the primary ray time.
        iTime += iStepSize;

    }

    // Calculate and return the final color.
    return iSun * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie);
}


vec3 rectToSpherical(vec2 uv) {
    float phi = uv.x * M_PI * 2.0;
    float theta = uv.y * M_PI;

    float locZ = sin(phi) * sin(-theta);
    float locX = sin(theta) * cos(phi);
    float locY = cos(theta);

    return vec3(locX, locY, locZ);
}

float remap01(float v, float lo, float hi) {
    return lo + v * (hi - lo);
}

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    uv.x = 1.0 - uv.x; // flip horizontally
    uv.y = 1.0 - uv.y; // flip vert, because I couldn't find how to flip the rendertexture
    vec2 uvWindow = vec2(remap01(uv.x, skyRangeX.x, skyRangeX.y),
    remap01(uv.y, skyRangeY.x, skyRangeY.y));

    vec3 ray = rectToSpherical(uvWindow);

    vec3 color = atmosphere(
        normalize(ray), // normalized ray direction
        vec3(0.0, 6372e3, 0.0), // ray origin
        sunDir, // position of the sun
        22.0, // intensity of the sun
        6371e3, // radius of the planet in meters
        6471e3, // radius of the atmosphere in meters
        vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
        21e-6, // Mie scattering coefficient
        8e3, // Rayleigh scale height
        1.2e3, // Mie scale height
        0.758 // Mie preferred scattering direction
    );

    // Apply exposure.
//    color = 1.0 - exp(-2.0 * color);

//    color = vibrance(color, 0.8);

    //    gl_FragColor = vec4(gl_TexCoord[0].xy, 0, 1);
    //    gl_FragColor = vec4(color, 1);
    gl_FragColor = vec4(color, 1.0);
}