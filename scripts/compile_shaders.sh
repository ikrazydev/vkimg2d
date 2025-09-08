# Vulkan SDK binaries must be added to path
glslc -fshader-stage=vertex ../shaders/vertex.glsl -o ../shaders/vertex.spv
glslc -fshader-stage=fragment ../shaders/fragment.glsl -o ../shaders/fragment.spv
