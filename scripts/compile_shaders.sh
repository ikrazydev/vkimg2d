#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SHADER_DIR="$SCRIPT_DIR/../shaders"

rm -f "$SHADER_DIR"/*.spv "$SHADER_DIR"/effects/*.spv

glslc -fshader-stage=vertex "$SHADER_DIR/vertex.glsl" -o "$SHADER_DIR/vertex.spv"
glslc -fshader-stage=fragment "$SHADER_DIR/fragment.glsl" -o "$SHADER_DIR/fragment.spv"
glslc -fshader-stage=compute "$SHADER_DIR/sampler.glsl" -o "$SHADER_DIR/sampler.spv"

glslc -fshader-stage=compute "$SHADER_DIR/effects/grayscale.glsl" -o "$SHADER_DIR/effects/grayscale.spv"
glslc -fshader-stage=compute "$SHADER_DIR/effects/sepia.glsl" -o "$SHADER_DIR/effects/sepia.spv"
glslc -fshader-stage=compute "$SHADER_DIR/effects/invert.glsl" -o "$SHADER_DIR/effects/invert.spv"
glslc -fshader-stage=compute "$SHADER_DIR/effects/posterize.glsl" -o "$SHADER_DIR/effects/posterize.spv"
glslc -fshader-stage=compute "$SHADER_DIR/effects/solarize.glsl" -o "$SHADER_DIR/effects/solarize.spv"
glslc -fshader-stage=compute "$SHADER_DIR/effects/threshold.glsl" -o "$SHADER_DIR/effects/threshold.spv"

glslc -fshader-stage=compute "$SHADER_DIR/effects/bricon.glsl" -o "$SHADER_DIR/effects/bricon.spv"
glslc -fshader-stage=compute -I"$SHADER_DIR/include" "$SHADER_DIR/effects/huesat.glsl" -o "$SHADER_DIR/effects/huesat.spv"
glslc -fshader-stage=compute "$SHADER_DIR/effects/coloffset.glsl" -o "$SHADER_DIR/effects/coloffset.spv"

echo "Shaders compiled."
