// #include "ffmpeg/video_player.hpp"
// #include "utils/utils.hpp"
// #include <chrono>
// #include <string>
//
// frame_ptr VideoPlayer::make_frame_ptr() {
//     return frame_ptr(av_frame_alloc(), [](AVFrame *f) { av_frame_free(&f); });
// }
//
// format_ptr VideoPlayer::make_format_ptr() {
//     return format_ptr(avformat_alloc_context(), [](AVFormatContext *f) {
//         avformat_close_input(&f);
//     });
// }
//
// decoder_ptr VideoPlayer::make_decoder_ptr(const AVCodec *dec) {
//     // Initial initialization
//     if (dec == NULL) return decoder_ptr(nullptr, [](AVCodecContext *) {});
//
//     return decoder_ptr(avcodec_alloc_context3(dec), [](AVCodecContext *f) {
//         avcodec_free_context(&f);
//     });
// }
//
// packet_ptr VideoPlayer::make_packet_ptr() {
//     return packet_ptr(av_packet_alloc(), [](AVPacket *f) { av_packet_free(&f); });
// }
//
// int VideoPlayer::set_video(const std::string &filename) {
//     const AVCodec *dec;
//     int ret;
//
//     // already allocated in member variable
//     auto format_ptr = fmt_ctx.get();
//     if ((ret = avformat_open_input(&format_ptr, filename.c_str(), NULL, NULL)) < 0) {
//         av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
//         return ret;
//     }
//
//     if ((ret = avformat_find_stream_info(format_ptr, NULL)) < 0) {
//         av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
//         return ret;
//     }
//
//     /* select the video stream */
//     ret = av_find_best_stream(format_ptr, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
//     if (ret < 0) {
//         av_log(NULL, AV_LOG_ERROR, "Cannot find a video stream in the input file\n");
//         return ret;
//     }
//     video_stream_index = ret;
//
//     /* create decoding context */
//     dec_ctx = make_decoder_ptr(dec);
//     auto dec_ptr = dec_ctx.get();
//     if (!dec_ctx) return AVERROR(ENOMEM);
//
//     avcodec_parameters_to_context(dec_ptr, fmt_ctx->streams[video_stream_index]->codecpar);
//     time_base = av_q2d(fmt_ctx->streams[video_stream_index]->time_base);
//
//     /* init the video decoder */
//     if ((ret = avcodec_open2(dec_ptr, dec, NULL)) < 0) {
//         av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
//         return ret;
//     }
//
//     aspect_ratio = AspectRatio(dec_ptr->width, dec_ptr->height);
//
//     started_playing_loaded_video = false;
//
//     total_duration = std::chrono::duration<float>(fmt_ctx->duration / AV_TIME_BASE);
//     total_duration_str = duration_to_string(total_duration);
//
//     return 0;
// }
//
// bool VideoPlayer::load_more_frames() {
//     if (av_read_frame(fmt_ctx.get(), packet.get()) < 0) return false;
//
//     int frames_queue_size = frames_queue.size();
//     int ret;
//     while (frames_queue_size == frames_queue.size() && packet->stream_index ==
//     video_stream_index) {
//         ret = avcodec_send_packet(dec_ctx.get(), packet.get());
//
//         /*
//          * Normally beacuse of this
//          * no inifinite loop is possible
//          */
//         if (ret < 0) {
//             av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
//             return false;
//         }
//
//         while (ret >= 0) {
//             /*
//              * Allocates new frame each time
//              */
//             /* If we are skipping frames we don't need
//              * to allocate new space for frames that will
//              * be just skipped
//              */
//             auto frame_ptr = make_frame_ptr();
//             ret = avcodec_receive_frame(dec_ctx.get(), frame_ptr.get());
//
//             if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//                 break;
//             } else if (ret < 0) {
//                 av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder\n");
//                 return false;
//             }
//             if (frame_ptr->width > 0 && frame_ptr->height > 0) frames_queue.push_back(frame_ptr);
//         }
//     }
//
//     av_packet_unref(packet.get());
//
//     return true;
// }
//
// double VideoPlayer::front_frame_timestamp_in_seconds() {
//     return ((double)frames_queue.front()->pts) * time_base;
// }
//
// frame_ptr VideoPlayer::operator()() {
//     if (fmt_ctx.get() == nullptr || dec_ctx.get() == nullptr) {
//         printDebug("No video set, can't play!");
//         return nullptr;
//     }
//
//     bool video_paused = pause.paused_now && last_frame;
//     if (video_paused) return last_frame;
//     if (frames_queue.empty() && !load_more_frames()) {
//         // File ended
//         played_duration = total_duration;
//         return last_frame;
//     }
//
//     auto now = std::chrono::system_clock::now();
//
//     if (!started_playing_loaded_video) {
//         started_playing_loaded_video = true;
//         start_time = now;
//     }
//
//     if (pause.adjust_player) {
//         start_time += now - pause.pause_time;
//         pause.adjust_player = false;
//     }
//
//     played_duration = std::chrono::duration_cast<std::chrono::duration<float>>(now - start_time);
//
//     if (!frames_queue.empty()) {
//         auto frame = frames_queue.front();
//         auto current = front_frame_timestamp_in_seconds();
//         auto expected = played_duration.count();
//         // printf("current = %f, expected = %f\n", current, expected);
//         if (current <= expected) {
//             frames_queue.pop_front();
//             last_frame = frame;
//         }
//     }
//
//     return last_frame;
// }
//
// void VideoPlayer::set_played_duration(const duration &duration) {
//     // Duration below 0, exceeds video length -> don't do anything
//     if ((duration < std::chrono::duration<float>(0)) || (duration > total_duration)) return;
//
//     static auto make_diff = [](::duration a, ::duration b) {
//         return std::chrono::duration_cast<typename decltype(this->start_time)::duration>(a - b);
//     };
//
//     // old frames are now invalid
//     frames_queue.clear();
//     if (duration > played_duration) this->start_time -= make_diff(duration, played_duration);
//     else if (duration < played_duration) this->start_time += make_diff(played_duration,
//     duration);
//
//     double duration_in_seconds =
//     std::chrono::duration_cast<std::chrono::seconds>(duration).count(); int64_t ts =
//         av_rescale_q(duration_in_seconds, {1, 1},
//         fmt_ctx->streams[video_stream_index]->time_base);
//
//     /*
//      * There seem to be no problem with seeking forward in time
//      * seeking backwards requires multiple same avformat_seek_file
//      * calls, after research seeking file / frame is a long
//      * time problematic part of ffmpeg so its not my
//      * skill issue
//      */
//
//     auto seek = [&]() {
//         return avformat_seek_file(
//             fmt_ctx.get(),
//             video_stream_index,
//             0,
//             ts,
//             ts,
//             AVSEEK_FLAG_BACKWARD
//         );
//     };
//     auto skip = [&]() {
//         frames_queue.clear();
//         load_more_frames();
//     };
//
//     // initial
//     if (seek() < 0) return;
//     load_more_frames();
//
//     if (duration < played_duration) {
//         while (front_frame_timestamp_in_seconds() > duration_in_seconds) {
//             frames_queue.clear();
//             skip();
//         }
//     }
//
//     // skip to real seeked time
//     while (front_frame_timestamp_in_seconds() < duration_in_seconds) skip();
// }
