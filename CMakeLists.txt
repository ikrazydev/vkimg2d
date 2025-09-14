cmake_minimum_required(VERSION 3.16)
project(VkImg2D VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

if(APPLE)
    if(EXISTS "/usr/local/lib/libMoltenVK.dylib")
        set(MOLTENVK_LIBRARY "/usr/local/lib/libMoltenVK.dylib")
        message(STATUS "Found MoltenVK: ${MOLTENVK_LIBRARY}")
        set(VULKAN_FOUND TRUE)
        set(Vulkan_LIBRARIES "/usr/local/lib/libvulkan.dylib")
        set(Vulkan_INCLUDE_DIRS /usr/local/include)
    else()
        message(FATAL_ERROR "MoltenVK not found on macOS! This is required for Vulkan support. Please install Vulkan SDK for macOS from https://vulkan.lunarg.com/sdk/home#mac")
    endif()
else()
    find_package(Vulkan REQUIRED)
    set(VULKAN_FOUND TRUE)
endif()

if(NOT VULKAN_FOUND)
    message(FATAL_ERROR "Vulkan/MoltenVK not found!")
endif()

find_package(glfw3 REQUIRED)

set(IMGUI_DIR external/imgui)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/app.cpp

    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp

    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    ${Vulkan_LIBRARIES}
    glfw
)

if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VK_USE_PLATFORM_MACOS_MVK)

    target_link_libraries(${PROJECT_NAME}
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework QuartzCore"
        "-framework Metal"
        "-framework IOSurface"
    )

elseif(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VK_USE_PLATFORM_WIN32_KHR)

elseif(UNIX AND NOT APPLE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE VK_USE_PLATFORM_XCB_KHR)
    
    find_package(X11 REQUIRED)
    target_link_libraries(${PROJECT_NAME}
        ${X11_LIBRARIES}
        ${CMAKE_DL_LIBS}
    )
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

message(STATUS "Vulkan found: ${Vulkan_FOUND}")
message(STATUS "Vulkan include dirs: ${Vulkan_INCLUDE_DIRS}")
message(STATUS "Vulkan libraries: ${Vulkan_LIBRARIES}")
message(STATUS "GLFW found: ${glfw3_FOUND}")
