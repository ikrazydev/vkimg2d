#version 460 core

layout(binding = 0, rgba8) uniform readonly image2D inImage;
layout(binding = 1, rgba8) uniform writeonly image2D outImage;

layout(push_constant) uniform pc {
    float threshold;
};

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    vec4 color = imageLoad(inImage, ivec2(gl_GlobalInvocationID.xy));

    color.rgb = mix(color.rgb, 1.0 - color.rgb, step(threshold, color.rgb));

    imageStore(outImage, ivec2(gl_GlobalInvocationID.xy), color);
}
