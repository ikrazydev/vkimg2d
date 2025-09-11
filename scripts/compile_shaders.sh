rm ../shaders/vertex.spv ../shaders/fragment.spv

# Vulkan SDK binaries must be added to path
glslc -fshader-stage=vertex ../shaders/vertex.glsl -o ../shaders/vertex.spv
glslc -fshader-stage=fragment ../shaders/fragment.glsl -o ../shaders/fragment.spv

# Copy from ../shaders to ../build/shaders
rm -rf ../build/shaders/
cp -r ../shaders ../build/shaders
