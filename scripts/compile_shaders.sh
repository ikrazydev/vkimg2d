#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SHADER_DIR="$SCRIPT_DIR/../shaders"
BIN_DIR="$SHADER_DIR/bin"
INCLUDE_DIR="$SHADER_DIR/include"

rm -f "$SHADER_DIR"/*.spv "$SHADER_DIR"/effects/*.spv

mkdir -p "$BIN_DIR"
rm -f "$BIN_DIR"/*.spv

glslc -fshader-stage=vertex "$SHADER_DIR/vertex.glsl" -o "$BIN_DIR/vertex.spv"
glslc -fshader-stage=fragment "$SHADER_DIR/fragment.glsl" -o "$BIN_DIR/fragment.spv"
glslc -fshader-stage=compute "$SHADER_DIR/sampler.glsl" -o "$BIN_DIR/sampler.spv"

for shader in "$SHADER_DIR"/effects/*.glsl; do
    [ -f "$shader" ] || continue
    filename=$(basename "$shader" .glsl)
    glslc -fshader-stage=compute -I"$INCLUDE_DIR" "$shader" -o "$BIN_DIR/${filename}.spv"
    echo "Compiled ${filename}.glsl"
done

echo "Shaders compiled."
