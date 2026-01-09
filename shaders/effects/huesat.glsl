#version 460 core

#include "color.glsl"

layout(binding = 0, rgba8) uniform readonly image2D inImage;
layout(binding = 1, rgba8) uniform writeonly image2D outImage;

layout(push_constant) uniform pc {
    float hue;
    float saturation;
    float brightness;
};

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    vec4 color = imageLoad(inImage, ivec2(gl_GlobalInvocationID.xy));

    vec3 hsl = rgbToHsl(color.rgb);
    hsl.x = fract(hsl.x + hue);
    hsl.y = clamp(hsl.y + saturation, 0.0, 1.0);
    hsl.z = clamp(hsl.z + brightness, 0.0, 1.0);

    color.rgb = hslToRgb(hsl);

    imageStore(outImage, ivec2(gl_GlobalInvocationID.xy), color);
}
