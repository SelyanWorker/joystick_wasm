add_library(imgui_widgets INTERFACE)

target_include_directories(imgui_widgets INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(imgui_widgets INTERFACE implot)