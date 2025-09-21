# Initialization
cmake_minimum_required(VERSION 3.5)
project(TinyTracer)

# Set Graphics API as VULKAN or OPENGL
set(GRAPHICS_API "VULKAN")

# Set acceleration structure as BVH or NONE
set(ACC_STRUCT "BVH")

# Packages
find_package(Vulkan REQUIRED)

# External libraries
include(FetchContent)

FetchContent_Declare(
	glm
	GIT_REPOSITORY "https://github.com/g-truc/glm.git"
	GIT_TAG "1.0.1"
	GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glm)

FetchContent_Declare(
	spdlog
	GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
	GIT_TAG "v1.15.3"
	GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
	glfw
	GIT_REPOSITORY "https://github.com/glfw/glfw.git"
	GIT_TAG "3.3.8"
	GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
	microsoft-gsl
	GIT_REPOSITORY "https://github.com/microsoft/GSL.git"
	GIT_TAG "v4.0.0"
	GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(microsoft-gsl)

# Source files
file(GLOB_RECURSE TinyTracerSources CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

add_executable(TinyTracer ${TinyTracerSources})
target_include_directories(TinyTracer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Glad setup
add_library(glad STATIC "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/glad.c")
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include")

# Dependencies
target_link_libraries(TinyTracer PRIVATE Vulkan::Vulkan)
target_link_libraries(TinyTracer PRIVATE glm)
target_link_libraries(TinyTracer PRIVATE glfw)
target_link_libraries(TinyTracer PRIVATE Microsoft.GSL::GSL)
target_link_libraries(TinyTracer PRIVATE spdlog)
target_link_libraries(TinyTracer PRIVATE glad)

# Path to shader folder
set(SHADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders/")

# Compile
target_compile_features(TinyTracer PRIVATE cxx_std_20)
target_precompile_headers(TinyTracer PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/utils/precomp.hpp")
target_compile_definitions(TinyTracer PRIVATE SHADER_DIR="${SHADER_DIR}")

if(GRAPHICS_API STREQUAL "VULKAN")
	target_compile_definitions(TinyTracer PRIVATE GRAPHICS_API_STRING=\"Vulkan\")
	target_compile_definitions(TinyTracer PRIVATE VULKAN=1)
	target_compile_definitions(TinyTracer PRIVATE OPENGL=0)
elseif(GRAPHICS_API STREQUAL "OPENGL")
	target_compile_definitions(TinyTracer PRIVATE GRAPHICS_API_STRING=\"OpenGL\")
	target_compile_definitions(TinyTracer PRIVATE VULKAN=0)
	target_compile_definitions(TinyTracer PRIVATE OPENGL=1)
else()
    message(FATAL_ERROR "Unknown GRAPHICS_API value: ${GRAPHICS_API}")
endif()

if(ACC_STRUCT STREQUAL "BVH")
	target_compile_definitions(TinyTracer PRIVATE ACC_STRUCT_STRING=\"BHV\")
	target_compile_definitions(TinyTracer PRIVATE BVH=1)
elseif(ACC_STRUCT STREQUAL "NONE")
	target_compile_definitions(TinyTracer PRIVATE ACC_STRUCT_STRING=\"None\")
	target_compile_definitions(TinyTracer PRIVATE BVH=0)
else()
    message(FATAL_ERROR "Unknown ACC_STRUCT value: ${ACC_STRUCT}")
endif()