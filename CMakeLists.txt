cmake_minimum_required(VERSION 3.17)
project(joystick_wasm)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXECUTABLE_SUFFIX .html)

set(USE_FLAGS "-s USE_SDL=2 -s WASM=1 -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1 -s NO_FILESYSTEM=1 -DIMGUI_DISABLE_FILE_FUNCTIONS")

set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

add_library(imgui
        STATIC
            ${THIRD_PARTY_PATH}/imgui/imgui.cpp
            ${THIRD_PARTY_PATH}/imgui/imgui_draw.cpp
            ${THIRD_PARTY_PATH}/imgui/imgui_tables.cpp
            ${THIRD_PARTY_PATH}/imgui/imgui_widgets.cpp
            ${THIRD_PARTY_PATH}/imgui/imgui_demo.cpp
            ${THIRD_PARTY_PATH}/imgui/backends/imgui_impl_glfw.cpp
            ${THIRD_PARTY_PATH}/imgui/backends/imgui_impl_sdl.cpp
            ${THIRD_PARTY_PATH}/imgui/backends/imgui_impl_opengl3.cpp
        )
target_include_directories(imgui
        PUBLIC
            ${THIRD_PARTY_PATH}/imgui
            ${THIRD_PARTY_PATH}/imgui/backends
            ${THIRD_PARTY_PATH}/implot
        )

add_library(implot
        STATIC
            ${THIRD_PARTY_PATH}/implot/implot.cpp
            ${THIRD_PARTY_PATH}/implot/implot_items.cpp
            ${THIRD_PARTY_PATH}/implot/implot_demo.cpp
        )
target_include_directories(implot
        PUBLIC
            ${THIRD_PARTY_PATH}/implot
        )
target_link_libraries(implot
        PUBLIC
            imgui
        )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")

add_subdirectory(imgui_widgets)

add_executable(joystick_glfw main_glfw.cpp)
target_link_libraries(joystick_glfw PRIVATE imgui imgui_widgets)

add_executable(joystick_sdl main_sdl.cpp)
target_link_libraries(joystick_sdl PRIVATE imgui)
