@echo off
setlocal

set SCRIPT_DIR=%~dp0
set SHADER_DIR=%SCRIPT_DIR%..\shaders
set BIN_DIR=%SHADER_DIR%\bin
set INCLUDE_DIR=%SHADER_DIR%\include

del /q "%SHADER_DIR%\*.spv" 2>nul
del /q "%SHADER_DIR%\effects\*.spv" 2>nul

if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"
del /q "%BIN_DIR%\*.spv" 2>nul

glslc -fshader-stage=vertex "%SHADER_DIR%\vertex.glsl" -o "%BIN_DIR%\vertex.spv"
glslc -fshader-stage=fragment "%SHADER_DIR%\fragment.glsl" -o "%BIN_DIR%\fragment.spv"
glslc -fshader-stage=compute "%SHADER_DIR%\sampler.glsl" -o "%BIN_DIR%\sampler.spv"

for %%f in ("%SHADER_DIR%\effects\*.glsl") do (
    glslc -fshader-stage=compute -I"%INCLUDE_DIR%" "%%f" -o "%BIN_DIR%\%%~nxf.spv"
    echo Compiled %%~nxf.glsl
)

echo Shaders compiled.
