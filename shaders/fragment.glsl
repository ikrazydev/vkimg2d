#version 460 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 texColor = texture(texSampler, inTexCoord).rgb;

    outColor = vec4(inColor * texColor, 1.0);

    float gray = dot(outColor.rgb, vec3(0.299, 0.587, 0.114));
    outColor = vec4(vec3(gray), 1.0);
}
