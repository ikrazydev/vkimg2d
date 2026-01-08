@echo off
setlocal

set SCRIPT_DIR=%~dp0
set SHADER_DIR=%SCRIPT_DIR%..\shaders

del /q "%SHADER_DIR%\*.spv" 2>nul
del /q "%SHADER_DIR%\effects\*.spv" 2>nul

glslc -fshader-stage=vertex "%SHADER_DIR%\vertex.glsl" -o "%SHADER_DIR%\vertex.spv"
glslc -fshader-stage=fragment "%SHADER_DIR%\fragment.glsl" -o "%SHADER_DIR%\fragment.spv"
glslc -fshader-stage=compute "%SHADER_DIR%\sampler.glsl" -o "%SHADER_DIR%\sampler.spv"

glslc -fshader-stage=compute "%SHADER_DIR%\effects\grayscale.glsl" -o "%SHADER_DIR%\effects\grayscale.spv"
glslc -fshader-stage=compute "%SHADER_DIR%\effects\sepia.glsl" -o "%SHADER_DIR%\effects\sepia.spv"
glslc -fshader-stage=compute "%SHADER_DIR%\effects\invert.glsl" -o "%SHADER_DIR%\effects\invert.spv"
glslc -fshader-stage=compute "%SHADER_DIR%\effects\posterize.glsl" -o "%SHADER_DIR%\effects\posterize.spv"
glslc -fshader-stage=compute "%SHADER_DIR%\effects\solarize.glsl" -o "%SHADER_DIR%\effects\solarize.spv"
glslc -fshader-stage=compute "%SHADER_DIR%\effects\threshold.glsl" -o "%SHADER_DIR%\effects\threshole.spv"

echo Shaders compiled.
