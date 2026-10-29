cmake_minimum_required(VERSION 3.24)
set(CMAKE_VS_GLOBALS "VcpkgEnabled=false")
project(Billiards LANGUAGES C CXX)

if(NOT WIN32)
    message(FATAL_ERROR "This project uses Windows PlaySound and requires Windows.")
endif()

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
add_subdirectory(third_party/freeglut EXCLUDE_FROM_ALL)

add_library(billiards_glew STATIC "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glew/src/glew.c")
target_include_directories(billiards_glew PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glew/include")
target_compile_definitions(billiards_glew PUBLIC GLEW_STATIC)
target_link_libraries(billiards_glew PUBLIC opengl32)

add_executable(Billiards main.cpp ball.cpp box.cpp floor.cpp)
target_compile_features(Billiards PRIVATE cxx_std_17)
target_compile_definitions(Billiards PRIVATE FREEGLUT_STATIC FREEGLUT_LIB_PRAGMAS=0)
target_include_directories(Billiards PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/freeglut/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm")
target_link_libraries(Billiards PRIVATE billiards_glew freeglut_static opengl32 winmm)
set_target_properties(Billiards PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:Billiards>")
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Billiards)

set(BILLIARDS_ASSETS vertex2.glsl fragment2.glsl sphere.obj billiard.wav stomp.wav)
foreach(asset IN LISTS BILLIARDS_ASSETS)
    add_custom_command(TARGET Billiards POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/${asset}" "$<TARGET_FILE_DIR:Billiards>/${asset}")
endforeach()
