#version 460 core

layout(binding = 0) uniform sampler2D processedTexture;
layout(binding = 1) uniform sampler2D originalTexture;

layout(push_constant) uniform pc {
    float resultMix;
};

//layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(originalTexture, inTexCoord);
    vec4 imageColor = texture(processedTexture, inTexCoord);

    outColor = mix(texColor, imageColor, resultMix);
}
