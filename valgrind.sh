video="assets/long_mountains_cut_yuv420.mp4"

if [ "$#" -ge 1 ]; then
    video=$1
fi

# valgrind --tool=massif ./build/clang-debug/WebScreenShare "$video"
valgrind --tool=massif ./run.sh
