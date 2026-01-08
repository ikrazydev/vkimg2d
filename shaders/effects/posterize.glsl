#version 460 core

layout(binding = 0, rgba8) uniform readonly image2D inImage;
layout(binding = 1, rgba8) uniform writeonly image2D outImage;

layout(push_constant) uniform pc {
    float level;
};

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    vec4 color = imageLoad(inImage, ivec2(gl_GlobalInvocationID.xy));

    float powLevel = pow(2.0, level);
    color.rgb = floor(color.rgb * powLevel) / powLevel;

    imageStore(outImage, ivec2(gl_GlobalInvocationID.xy), color);
}
