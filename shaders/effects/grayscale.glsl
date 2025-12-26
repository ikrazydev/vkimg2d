#version 460 core

layout(binding = 0, rgba8) uniform readonly image2D inImage;
layout(binding = 1, rgba8) uniform writeonly image2D outImage;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    vec4 color = imageLoad(inImage, ivec2(gl_GlobalInvocationID.xy));

    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    color = vec4(vec3(gray), color.a);

    imageStore(outImage, ivec2(gl_GlobalInvocationID.xy), color);
}
