# FFmpeg & OpenGL & ImGui & miniaudio mediaplayer

https://github.com/user-attachments/assets/9b781c4e-ae13-40a0-bf97-81fe78e5303f

- Video and audio
- yuv420 only (yuv444 shader present), both es and non es shader version present
- Pause, and clickable time line for controls
- Resizable (with aspect ratio)

Main files:

- [src/ffmpeg/player/ffmpeg.cpp](https://github.com/sqrtminustwo/VideoPlayer/blob/main/src/ffmpeg/player/ffmpeg.cpp)
- [src/ffmpeg/player/player.cpp](https://github.com/sqrtminustwo/VideoPlayer/blob/main/src/ffmpeg/player/player.cpp)
- [src/opengl/drawers/overlay/controller.cpp](https://github.com/sqrtminustwo/VideoPlayer/blob/main/src/opengl/drawers/overlay/controller.cpp)
- [src/opengl/drawers/frame/drawer.cpp](https://github.com/sqrtminustwo/VideoPlayer/blob/main/src/opengl/drawers/frame/drawer.cpp)

Dependencies (all in external):

- OpenGL (glad headers + src)
- FFmpeg (cloned and built)
- ImGUI (github submodule)
- Miniaudio (github submodule)

OS:

Developed on Ubuntu 24.04.4 OS version works on it.

Emscripten version works on linux and windows, not yet on android and ios.
