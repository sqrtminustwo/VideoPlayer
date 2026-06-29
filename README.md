# FFmpeg & OpenGL & ImGui videoplayer (1000 lines)

[demo.webm](https://github.com/user-attachments/assets/17fb360f-af61-412b-934f-60b5bc47ca24)

- This is part of a larger project for web video streaming
- Cmake parts for emscritpen won't work as they require ../web folder, which you naturally won't have, to build without emscripten just ignore it, to build with emscritpen either create the web folder in upper directory or change the `cmake/Packages.cmake`
- Video only
- yuv420 only (yuv444 shader present), both es and non es shader version present
- Pause, and clickable time line for controls
- Resizable (with aspect ratio)

Sturcture:

- High probability you came to see [`src/ffmpeg/video_player.cpp`](https://github.com/sqrtminustwo/FFmpegPlayer/blob/2f2586c4004a11d01756874f0a9f3b3bcb4f0f84/src/ffmpeg/video_player.cpp),
  GUI controls for it are in [`src/opengl/drawers/overlay/controller.cpp`](https://github.com/sqrtminustwo/FFmpegPlayer/blob/main/src/opengl/drawers/overlay/controller.cpp), frame drawing in [`src/opengl/drawers/frame/drawer.cpp`](https://github.com/sqrtminustwo/FFmpegPlayer/blob/main/src/opengl/drawers/frame/drawer.cpp)
- Implementations in `src`, headers in `include`

Dependencies (all in external):

- OpenGL (glad headers + src)
- FFmpeg (cloned and built)
- ImGUI (literally cloned in external)

OS:

Developed on Ubuntu 24.04.4, not tested on other OS's (only emscripten built will naturally work on any OS).
