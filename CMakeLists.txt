cmake_minimum_required (VERSION 2.6)

project(mcpelauncher-core LANGUAGES CXX)

if (NOT APPLE)
    find_package(PNG)
endif()

add_library(mcpelauncher-core include/mcpelauncher/app_platform.h include/mcpelauncher/hook.h include/mcpelauncher/patch_utils.h src/app_platform.cpp src/hook.cpp src/patch_utils.cpp include/mcpelauncher/minecraft_version.h src/minecraft_version.cpp)
target_include_directories(mcpelauncher-core PUBLIC include/)
target_link_libraries(mcpelauncher-core minecraft-symbols ${CMAKE_DL_LIBS})

if (APPLE)
    target_link_libraries(mcpelauncher-core osx-elf-header)
endif()

if (IS_ARMHF_BUILD)
    target_compile_definitions(mcpelauncher-core PRIVATE USE_BIONIC_LIBC)
endif()

if (PNG_FOUND)
    target_compile_definitions(mcpelauncher-core PRIVATE HAS_LIBPNG)
    target_include_directories(mcpelauncher-core PRIVATE ${PNG_INCLUDE_DIRS})
    target_link_libraries(mcpelauncher-core ${PNG_LIBRARIES})
endif()