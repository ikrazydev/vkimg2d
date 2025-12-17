@echo off
setlocal

set SCRIPT_DIR=%~dp0
set SHADER_DIR=%SCRIPT_DIR%..\shaders

del /q "%SHADER_DIR%\*.spv" 2>nul
del /q "%SHADER_DIR%\effects\*.spv" 2>nul

glslc -fshader-stage=vertex "%SHADER_DIR%\vertex.glsl" -o "%SHADER_DIR%\vertex.spv"
glslc -fshader-stage=fragment "%SHADER_DIR%\fragment.glsl" -o "%SHADER_DIR%\fragment.spv"
glslc -fshader-stage=compute "%SHADER_DIR%\effects\grayscale.glsl" -o "%SHADER_DIR%\effects\grayscale.spv"

echo Shaders compiled.
