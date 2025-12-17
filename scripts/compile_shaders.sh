#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SHADER_DIR="$SCRIPT_DIR/../shaders"

rm -f "$SHADER_DIR"/*.spv "$SHADER_DIR"/effects/*.spv

glslc -fshader-stage=vertex "$SHADER_DIR/vertex.glsl" -o "$SHADER_DIR/vertex.spv"
glslc -fshader-stage=fragment "$SHADER_DIR/fragment.glsl" -o "$SHADER_DIR/fragment.spv"
glslc -fshader-stage=compute "$SHADER_DIR/effects/grayscale.glsl" -o "$SHADER_DIR/effects/grayscale.spv"

echo "Shaders compiled."
