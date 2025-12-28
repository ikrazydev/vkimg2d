#version 460 core

layout(binding = 0, rgba8) uniform readonly image2D inImage;
layout(binding = 1, rgba8) uniform writeonly image2D outImage;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    bool linear = false;

    vec4 color = imageLoad(inImage, ivec2(gl_GlobalInvocationID.xy));

    if (linear) {
        color.rgb = 1.0 - color.rgb;
    }
    else {
        vec3 srgb = pow(color.rgb, vec3(1.0 / 2.2));
        srgb = 1.0 - srgb;

        vec3 linear = pow(srgb, vec3(2.2));
        color.rgb = linear;
    }

    imageStore(outImage, ivec2(gl_GlobalInvocationID.xy), color);
}
