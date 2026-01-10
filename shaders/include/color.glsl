#include "constants.glsl"

vec3 rgbToHsl(vec3 color) {
    float maxColor = max(color.r, max(color.g, color.b));
    float minColor = min(color.r, min(color.g, color.b));
    float delta = maxColor - minColor;

    float l = (maxColor + minColor) * 0.5;

    if (delta < EPSILON) {
        return vec3(0.0, 0.0, l);
    }

    float s = delta / (1.0 - abs(2.0 * l - 1.0));

    float h;
    if (maxColor == color.r) {
        h = mod((color.g - color.b) / delta, 6.0);
    } else if (maxColor == color.g) {
        h = (color.b - color.r) / delta + 2.0;
    } else {
        h = (color.r - color.g) / delta + 4.0;
    }

    h /= 6.0;

    return vec3(h, s, l);
}

vec3 hslToRgb(vec3 hsl) {
    if (hsl.y < EPSILON) {
        return vec3(hsl.z);
    }

    float h = hsl.x * 6.0;
    float c = (1.0 - abs(2.0 * hsl.z - 1.0)) * hsl.y;
    float x = c * (1.0 - abs(mod(h, 2.0) - 1.0));
    float m = hsl.z - c * 0.5;

    vec3 rgb;
    if (h < 1.0) rgb = vec3(c, x, 0.0);
    else if (h < 2.0) rgb = vec3(x, c, 0.0);
    else if (h < 3.0) rgb = vec3(0.0, c, x);
    else if (h < 4.0) rgb = vec3(0.0, x, c);
    else if (h < 5.0) rgb = vec3(x, 0.0, c);
    else rgb = vec3(c, 0.0, x);

    return rgb + m;
}

float luminance(vec3 color) {
    return dot(color.rgb, vec3(0.299, 0.587, 0.114));
}
