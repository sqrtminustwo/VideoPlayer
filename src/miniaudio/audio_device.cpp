#include "miniaudio/audio_device.hpp"
#include "ffmpeg/player/player.hpp" // IWYU pragma: keep
#include "utils/guarded_que.hpp"
#include <cstring>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/file.h>
}

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
    auto queue = reinterpret_cast<GuardedQue<frame_ptr> *>(pDevice->pUserData);
    auto frame = queue->get_front();

    if (frame == nullptr) {
        printf("need audio frame but none present\n");
        return;
    }

    int channels = frame->ch_layout.nb_channels;
    int framesToCopy = std::min(frame->nb_samples, (int)frameCount);
    size_t bytesToCopy = framesToCopy * channels * sizeof(float);
    memcpy(pOutput, frame->data[0], bytesToCopy);

    // if (framesToCopy < frame->nb_samples) {
    //     int remaining_frames = frame->nb_samples - framesToCopy;
    //     size_t remaining_bytes = remaining_frames * channels * sizeof(float);
    //     memmove(frame->data[0], frame->data[0] + bytesToCopy, remaining_bytes);
    //     frame->nb_samples = remaining_frames;
    // } else {
    //     queue->pop_front();
    // }
    queue->pop_front();
}

AudioDevice::AudioDevice(player_ptr player) : player{player} {
    if (!player->get_audio_stream()->is_valid()) {
        printf("Can't initialize audio device if there is no audio stream!\n");
        return;
    }

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_f32;

    auto codecpar = player->get_audio_stream()->get_stream()->codecpar;
    deviceConfig.playback.channels = codecpar->ch_layout.nb_channels;
    deviceConfig.sampleRate = codecpar->sample_rate;
    deviceConfig.dataCallback = data_callback;
    deviceConfig.pUserData = &player->get_audio_stream()->frames_queue;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open audio playback device.\n");
        return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        return;
    }
}

AudioDevice::~AudioDevice() { ma_device_uninit(&device); }
