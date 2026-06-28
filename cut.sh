file=$1
IFS='.' read -r -a array <<<"$file"
name=${array[0]}
extension=${array[1]}

# ffmpeg -i "$file" -ss 00:00:00 -to 00:01:00 -c copy "${name}_cut.${extension}"
ffmpeg -i "$file" -ss 00:00:35 -to 00:01:00 -c copy "${name}_cut.${extension}"
