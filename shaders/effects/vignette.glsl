#version 460 core

layout(binding = 0, rgba8) uniform readonly image2D inImage;
layout(binding = 1, rgba8) uniform writeonly image2D outImage;

layout(push_constant) uniform pc {
    float radius;
    float softness;
    float darkness;
};

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

void main() {
    vec4 color = imageLoad(inImage, ivec2(gl_GlobalInvocationID.xy));

    vec2 size = vec2(imageSize(inImage));
    vec2 uv = gl_GlobalInvocationID.xy / size;
    vec2 centered = (uv - 0.5) * 2.0;
    centered.x *= size.x / size.y;

    float invRadius = 1.75 - radius * 1.75;
    float dist = length(centered);
    float vig = smoothstep(invRadius, invRadius + softness, dist);

    vec3 vigColor = darkness > 0.0 ? vec3(0.0) : vec3(1.0);
    color.rgb = mix(color.rgb, vigColor, vig * abs(darkness));

    imageStore(outImage, ivec2(gl_GlobalInvocationID.xy), color);
}
