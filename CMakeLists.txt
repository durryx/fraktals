# Project definition
project(fraktals)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# Source files
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scr")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
set(SOURCES "${SRC_DIR}/main.cpp"
            "${SRC_DIR}/koch.cpp"
            "${SRC_DIR}/koch.hpp"
            "${SRC_DIR}/globals.hpp"
            "${SRC_DIR}/callback_functions.hpp"
            "${SRC_DIR}/callback_functions.cpp"
            "${SRC_DIR}/shader.hpp"
            "${SRC_DIR}/shader.cpp"

            "${LIB_DIR}/imgui/imgui_impl_glfw.cpp"
            "${LIB_DIR}/imgui/imgui.cpp"
            "${LIB_DIR}/imgui/imgui_impl_opengl3.cpp"
            "${LIB_DIR}/imgui/imgui_widgets.cpp"
            "${LIB_DIR}/imgui/imgui_tables.cpp"
            "${LIB_DIR}/imgui/imgui_demo.cpp"
            "${LIB_DIR}/imgui/imgui_draw.cpp"

            "${LIB_DIR}/imgui/imgui_impl_opengl3.h"
            "${LIB_DIR}/imgui/imgui.h"
            "${LIB_DIR}/imgui/imgui_impl_glfw.h"
            "${LIB_DIR}/imgui/imgui_internal.h"
            "${LIB_DIR}/imgui/imstb_rectpack.h"
            "${LIB_DIR}/imgui/imstb_textedit.h"
            "${LIB_DIR}/imgui/imstb_truetype.h"

            )

# Executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")

# ImGui
#find_p
# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}")
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")