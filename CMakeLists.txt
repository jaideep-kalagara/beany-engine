cmake_minimum_required(VERSION 3.20)
project(BeanyRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ---------------------------------------------
# Fetch GLFW
# ---------------------------------------------
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.4
)
FetchContent_MakeAvailable(glfw)

# ---------------------------------------------
# Add source files and executable
# ---------------------------------------------
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(main ${SOURCES})

add_custom_command(TARGET main POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
)
# ---------------------------------------------
# Include directories
# ---------------------------------------------
target_include_directories(main PRIVATE
  include/webgpu   # Elie Michel's wrapper
  third_party/wgpu/include
  include          # Your own headers
)

# ---------------------------------------------
# Link GLFW (fetched above)
# ---------------------------------------------
target_link_libraries(main PRIVATE glfw)

target_compile_definitions(main PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_BINARY_DIR}/assets")

# ---------------------------------------------
# Link WebGPU native backend
# ---------------------------------------------
if(WIN32)
  target_compile_definitions(main PRIVATE _WINDOWS)
  target_link_directories(main PRIVATE third_party/wgpu/lib/windows)
  target_link_libraries(main PRIVATE wgpu_native)

  add_custom_command(TARGET main PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_SOURCE_DIR}/third_party/wgpu/lib/windows/wgpu_native.dll
      $<TARGET_FILE_DIR:main>
  )

elseif(APPLE)
  target_compile_definitions(main PRIVATE _MACOS)
  target_link_directories(main PRIVATE third_party/wgpu/lib/macos)
  target_link_libraries(main PRIVATE wgpu_native)

  set_target_properties(main PROPERTIES INSTALL_RPATH "@loader_path")

elseif(UNIX)
  target_compile_definitions(main PRIVATE _LINUX)
  target_link_directories(main PRIVATE third_party/wgpu/lib/linux)
  target_link_libraries(main PRIVATE wgpu_native)

  set_target_properties(main PROPERTIES
    BUILD_RPATH "$ORIGIN"
    INSTALL_RPATH "$ORIGIN"
  )
endif()
