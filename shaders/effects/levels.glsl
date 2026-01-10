#version 460 core

layout(binding = 0, rgba8) uniform readonly image2D inImage;
layout(binding = 1, rgba8) uniform writeonly image2D outImage;

layout(push_constant) uniform pc {
    float blacks;
    float whites;
    float mids;
};

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    vec4 color = imageLoad(inImage, ivec2(gl_GlobalInvocationID.xy));

    float range = max(whites - blacks, 0.00091);
    color.rgb = (color.rgb - blacks) / range;
    color.rgb = clamp(color.rgb, 0.0, 1.0);
    color.rgb = pow(color.rgb, vec3(1.0 / mids));

    imageStore(outImage, ivec2(gl_GlobalInvocationID.xy), color);
}
