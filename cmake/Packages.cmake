# Packages

# FFmpeg
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
if(EMSCRIPTEN)
    set(FFMPEG_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/ffmpeg-wasm-built")
    list(APPEND CMAKE_FIND_ROOT_PATH "${FFMPEG_PATH}")
endif()
find_package(FFmpeg REQUIRED COMPONENTS avdevice avfilter avformat avcodec swscale swresample avutil)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    FFmpeg::avdevice FFmpeg::avfilter FFmpeg::avformat FFmpeg::avcodec FFmpeg::swscale FFmpeg::swresample FFmpeg::avutil
)

# ImGui
# https://github.com/ocornut/imgui/pull/1713
# https://medium.com/@sigmoid90/work-with-vcpkg-on-ubuntu-b484363b1fed

set(IMGUI_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")

set(IMGUI_SRC
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.h
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_PATH}/backends/imgui_impl_opengl3_loader.h
        ${IMGUI_PATH}/backends/imgui_impl_glfw.h
        ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
        ${IMGUI_PATH}/imgui.h
        ${IMGUI_PATH}/imgui.cpp
        ${IMGUI_PATH}/imgui_draw.cpp
        ${IMGUI_PATH}/imgui_tables.cpp
        ${IMGUI_PATH}/imgui_widgets.cpp
        ${IMGUI_PATH}/imgui_demo.cpp
        )

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui
    PUBLIC
        ${IMGUI_PATH}
        ${IMGUI_PATH}/backends
)

target_link_libraries(${PROJECT_NAME} PUBLIC imgui)

# OpenGL
if(EMSCRIPTEN)
    set(WEB_OUTPUT_DIR "${CMAKE_BINARY_DIR}/web")

    set(GLAD_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")
    add_library(glad STATIC ${GLAD_PATH}/glad.c)
    set_target_properties(glad PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GLAD_PATH}/include"
    )

    target_link_options(${PROJECT_NAME} PRIVATE
        "-pthread"
        "-sPTHREAD_POOL_SIZE=4"
        "-sUSE_GLFW=3"
        "-sUSE_WEBGL2=1"
        "-sINITIAL_MEMORY=512MB"
        "SHELL:--js-library ${CMAKE_CURRENT_SOURCE_DIR}/../web/capi.js"
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/shaders@shaders"
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts@assets/fonts"
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets/long_mountains_cut_yuv420.mp4@assets/long_mountains_cut_yuv420.mp4"
        "SHELL:--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets/jungle_cut_yuv420.mp4@assets/jungle_cut_yuv420.mp4"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE glad)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        SUFFIX ".js"
        RUNTIME_OUTPUT_DIRECTORY "${WEB_OUTPUT_DIR}"
    )
    # The web folder is not related to videoplayer itself
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/../web"
                "${WEB_OUTPUT_DIR}"
    )
else()
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    set(GLAD_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/glad")
    add_library(glad STATIC ${GLAD_PATH}/glad.c)

    set_target_properties(glad PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GLAD_PATH}/include"
    )

    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            glad
            ${OPENGL_LIBRARIES}
            glfw
    )
endif()

# Fonts

include_directories("external/fonts")
