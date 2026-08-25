video="assets/long_mountains_cut_yuv420.mp4"

if [ "$#" -ge 1 ]; then
    video=$1
fi

# preset="clang-debug"
preset="release"

cmake --preset "$preset"
cmake --build "build/$preset"

# ./build/clang-debug/WebScreenShare $video
./build/release/WebScreenShare $video
