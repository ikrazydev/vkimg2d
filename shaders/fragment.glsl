#version 460 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    // outColor = vec4(inColor, 1.0);
    vec3 texColor = texture(texSampler, inTexCoord).rgb;
    
    outColor = vec4(inColor * texColor, 1.0);
}
