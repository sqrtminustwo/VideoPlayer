video="assets/long_mountains_cut_yuv420.mp4"

if [ "$#" -ge 1 ]; then
    video=$1
fi

cmake --preset clang-debug
cmake --build build/clang-debug
./build/clang-debug/WebScreenShare $video
