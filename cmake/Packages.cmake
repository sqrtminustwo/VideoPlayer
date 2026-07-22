# Packages

set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")

# FFmpeg
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
if(EMSCRIPTEN)
    set(FFMPEG_PATH "${CMAKE_CURRENT_SOURCE_DIRCMAKE_CURRENT_SOURCE_DIR}/external/ffmpeg-wasm-built")
    list(APPEND CMAKE_FIND_ROOT_PATH "${FFMPEG_PATH}")
endif()
find_package(FFmpeg REQUIRED COMPONENTS avdevice avfilter avformat avcodec swscale swresample avutil)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    FFmpeg::avdevice FFmpeg::avfilter FFmpeg::avformat FFmpeg::avcodec FFmpeg::swscale FFmpeg::swresample FFmpeg::avutil
)

# ffmpeg-circular-buffer

set(
    FFMPEG_CIRCULAR_BUFFER
    "${EXTERNAL_DIR}/ffmpeg-circular-buffer"
)

set(
    FFMPEG_CIRCULAR_BUFFER_SRC
    ${FFMPEG_CIRCULAR_BUFFER}/src/buffer/cfb.cpp
    ${FFMPEG_CIRCULAR_BUFFER}/src/buffer/default_buffer.cpp
    ${FFMPEG_CIRCULAR_BUFFER}/src/buffer/buffer.cpp
)

add_library(ffmpeg_circular_buffer STATIC ${FFMPEG_CIRCULAR_BUFFER_SRC})

target_include_directories(ffmpeg_circular_buffer
    PUBLIC
        ${FFMPEG_CIRCULAR_BUFFER}/include
)

target_link_libraries(${PROJECT_NAME} PUBLIC ffmpeg_circular_buffer)

# ImGui
# https://github.com/ocornut/imgui/pull/1713
# https://medium.com/@sigmoid90/work-with-vcpkg-on-ubuntu-b484363b1fed

set(IMGUI_PATH "${EXTERNAL_DIR}/imgui")

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

    set(GLAD_PATH "${EXTERNAL_DIR}/glad")
    add_library(glad STATIC ${GLAD_PATH}/glad.c)
    set_target_properties(glad PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${GLAD_PATH}/include"
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE glad)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        SUFFIX ".js"
        RUNTIME_OUTPUT_DIRECTORY "${WEB_OUTPUT_DIR}"
    )
else()
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    set(GLAD_PATH "${EXTERNAL_DIR}/glad")
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

# # miniaudio
#
# set(MINIAUDIO "${EXTERNAL_DIR}/miniaudio")
#
# add_library(miniaudio STATIC
#     "${MINIAUDIO}/miniaudio.c"
# )
# target_include_directories(miniaudio PUBLIC ${MINIAUDIO})
#
# target_link_libraries(miniaudio PRIVATE m pthread dl)
#
# target_link_libraries(${PROJECT_NAME} PUBLIC miniaudio)
