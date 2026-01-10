#version 460 core

#include "color.glsl"

layout(binding = 0, rgba8) uniform readonly image2D inImage;
layout(binding = 1, rgba8) uniform writeonly image2D outImage;

layout(push_constant) uniform pc {
    float vibrance;
};

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    vec4 color = imageLoad(inImage, ivec2(gl_GlobalInvocationID.xy));

    float lum = luminance(color.rgb);
    float maxComp = max(color.r, max(color.g, color.b));
    float minComp = min(color.r, min(color.g, color.b));
    float sat = maxComp - minComp;

    color.rgb = mix(vec3(lum), color.rgb, 1.0 + vibrance * (1.0 - sat));

    imageStore(outImage, ivec2(gl_GlobalInvocationID.xy), color);
}
