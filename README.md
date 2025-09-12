# VkImg2D

A cross-platform Vulkan application for image manipulation. This application is designed to work on Windows, macOS (via MoltenVK), and Linux.

## Dependencies

### Required Libraries
- **Vulkan SDK**: Download from [LunarG](https://vulkan.lunarg.com/sdk/home)
- **GLFW**: For window management
- **GLM**: For math
- **CMake**: Version 3.16 or higher

### Platform-specific Notes

#### macOS
- **MoltenVK** is required (Vulkan SDK for macOS includes it)
- Download from [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home#mac)
- Required frameworks: Cocoa, IOKit, CoreVideo, QuartzCore, Metal

#### Linux
- Install Vulkan drivers for your GPU
- X11 development libraries required

#### Windows
- Vulkan SDK includes necessary libraries
- GLFW and GLM can be installed via vcpkg or built from source

## Building

### Quick Setup (macOS)
```bash
# This will install dependencies and configure environment variables
# Then build the project
mkdir build && cd build
cmake ..
make
```

### Using Package Managers (Recommended)

#### macOS with Homebrew + Manual SDK
```bash
# Install available dependencies via Homebrew
brew install glfw glm cmake

# Download and install Vulkan SDK for macOS separately on the Vulkan website
# It includes MoltenVK

# Set environment variables (add to ~/.zshrc):
export VULKAN_BIN_SDK=~/VulkanSDK/1.4.321.0/macOS
export PATH=$VULKAN_BIN_SDK/bin:$PATH
export VULKAN_SDK=/usr/local/share/vulkan
export PATH=$VULKAN_SDK/bin:$PATH
export VK_ICD_FILENAMES=$VULKAN_SDK/icd.d/MoltenVK_icd.json
export VK_LAYER_PATH=$VULKAN_SDK/etc/vulkan/explicit_layer.d

# Build
mkdir build && cd build
cmake ..
make
```

### Manual Build
```bash
# Ensure Vulkan SDK, GLFW, and GLM are installed and in your PATH/CMAKE_PREFIX_PATH
mkdir build && cd build
cmake ..
cmake --build .
```

### Environment Variables
```bash
# Helpful for debugging
export VK_LAYER_PATH=/path/to/vulkan/sdk/etc/vulkan/explicit_layer.d
export VK_ICD_FILENAMES=/path/to/vulkan/icd.json
```
