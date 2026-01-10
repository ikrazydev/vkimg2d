#version 460 core

layout(binding = 0, rgba8) uniform readonly image2D inImage;
layout(binding = 1, rgba8) uniform writeonly image2D outImage;

layout(push_constant) uniform pc {
    float sharpness;
};

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    vec4 center = imageLoad(inImage, coord);
    vec4 left = imageLoad(inImage, coord + ivec2(-1, 0));
    vec4 right = imageLoad(inImage, coord + ivec2(1, 0));
    vec4 up = imageLoad(inImage, coord + ivec2(0, -1));
    vec4 down = imageLoad(inImage, coord + ivec2(0, 1));

    vec4 edges = (left + right + up + down) * 0.25;
    vec4 color = center + (center - edges) * sharpness;

    imageStore(outImage, ivec2(gl_GlobalInvocationID.xy), color);
}
