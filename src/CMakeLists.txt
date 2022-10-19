# conan packeges
find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(imgui REQUIRED)
find_package(glm REQUIRED)

# add C++23 features
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

# sources
set(SRC_DIR ".")
set(LIB_DIR "../include/")
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

# executable definition and properties
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "${SRC_DIR}")

# GLFW
set(GLFW_DIR "${LIB_DIR}/glfw/")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
add_subdirectory("${GLFW_DIR}" "${GLFW_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# glad
set(GLAD_DIR "${LIB_DIR}/glad")
#add_library("glad" "${GLAD_DIR}/src/glad.c")

#add_library(glad ${GLAD_DIR}/include/glad/glad.h ${GLAD_DIR}/src/glad.c)
#add_library(
#        glad
#        ${GLAD_DIR}/include/glad/glad.h
#        ${GLAD_DIR}/src/glad.c
#        ${GLAD_DIR}/include/KHR/khrplatform.h
#)
#target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
add_subdirectory(${GLAD_DIR} ${GLAD_DIR})


# copy shaders
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ../../../../src/shaders/ ${CMAKE_CURRENT_BINARY_DIR}/shaders
)
add_dependencies(${PROJECT_NAME} copy_assets)

# linkng static and global dependences
target_link_libraries(${PROJECT_NAME}
    PUBLIC project_warnings
    PRIVATE 
    CLI11::CLI11 
    fmt::fmt 
    spdlog::spdlog
    glm::glm
    "glad"
    "glfw" "${GLFW_LIBRARIES}")