# VkImg2D

A cross-platform Vulkan application for image manipulation designed to work on Windows, macOS (via MoltenVK), and Linux.

## Dependencies

### Required Libraries
- **Vulkan SDK**: Download from [LunarG](https://vulkan.lunarg.com/sdk/home)
- **GLFW**: For window management
- **GLM**: For math
- **CMake**: Version 3.16 or higher

### Platform-specific Notes

#### Windows
- Vulkan SDK includes necessary libraries
- GLFW and GLM can be installed via vcpkg or built from source

#### macOS
- **MoltenVK** is required (Vulkan SDK for macOS includes it)
- Download from [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home#mac)
- Required frameworks: Cocoa, IOKit, CoreVideo, QuartzCore, Metal

#### Linux
- Install Vulkan drivers for your GPU
- X11 development libraries required

## Building

### CLI Setup
```bash
# This will install dependencies and configure environment variables
# Then build the project
mkdir build && cd build
cmake ..
cmake --build .
make
```

### Visual Studio

Using CMake extension, you can open the project as a folder.
You can compile and run the application through it.

If assets are not being copied, try building manually before running the app.

### Using Package Managers

#### Homebrew
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
cmake --build .
make
```

### Environment Variables
```bash
# Helpful for debugging
export VK_LAYER_PATH=/path/to/vulkan/sdk/etc/vulkan/explicit_layer.d
export VK_ICD_FILENAMES=/path/to/vulkan/icd.json
```
