#version 460 core

layout(binding = 0, rgba8) uniform readonly image2D inImage;
layout(binding = 1, rgba8) uniform writeonly image2D outImage;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    vec4 color = imageLoad(inImage, ivec2(gl_GlobalInvocationID.xy));

    float red = dot(color.rgb, vec3(0.393, 0.769, 0.189));
    float green = dot(color.rgb, vec3(0.349, 0.686, 0.168));
    float blue = dot(color.rgb, vec3(0.272, 0.534, 0.131));
    color = vec4(red, green, blue, color.a);

    imageStore(outImage, ivec2(gl_GlobalInvocationID.xy), color);
}
