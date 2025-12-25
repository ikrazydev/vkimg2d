#version 460 core

layout (binding = 0) uniform sampler2D inImage;
layout (binding = 1, rgba8) uniform writeonly image2D outImage;

void main() {
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv = (vec2(coord) + 0.5) / vec2(imageSize(outImage));
    vec4 color = texture(inImage, uv);

    imageStore(outImage, coord, color);
}
