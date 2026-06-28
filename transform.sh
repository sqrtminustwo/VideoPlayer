# https://superuser.com/questions/1756187/extract-only-video-from-file-using-ffmpeg

file=$1
fmt="yuv420p"

if [ "$#" -ge 2 ]; then
    fmt=$2
fi

IFS='.' read -r -a array <<<"$file"
name=${array[0]}
extension=${array[1]}

# ffmpeg -i "$file" -an -vcodec copy -pix_fmt "$fmt" "${name}_${fmt}.${extension}"
ffmpeg -i "$file" -pix_fmt "$fmt" "${name}_${fmt}.${extension}"
