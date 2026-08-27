#include "miniaudio/audio_device.hpp"
#include "player/player.hpp" // IWYU pragma: keep
#include "utils/guarded_que.hpp"
#include <cstdint>
#include <cstring>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/file.h>
}

#define frame_s (*frame)
constexpr auto f = sizeof(float);

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
    auto player = static_cast<Player *>(pDevice->pUserData);
    if (player->is_stalled()) return;
    auto queue = &player->get_audio_stream()->frames_queue;

    uint8_t *pOutputReal = static_cast<uint8_t *>(pOutput);
    size_t filled_size = 0;

    while (filled_size < frameCount) {
        auto frame = queue->front_ptr();
        if (!frame) return;

        int remaining_size = frame_s->nb_samples - frame->offset;
        int framesToCopy = std::min(remaining_size, (int)(frameCount - filled_size));

        auto channels_size = frame_s->ch_layout.nb_channels * f;
        auto start = frame_s.offset * channels_size;
        size_t bytesToCopy = framesToCopy * channels_size;

        memcpy(pOutputReal, frame_s->data[0] + start, bytesToCopy);
        pOutputReal += bytesToCopy;
        filled_size += framesToCopy;

        if (framesToCopy < remaining_size) frame->offset += framesToCopy;
        else queue->pop_front();
    }
}

AudioDevice::AudioDevice(player_ptr player) {
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
    deviceConfig.pUserData = player.get();

    if (ma_device_init(nullptr, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open audio playback device.\n");
        return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        return;
    }
}

AudioDevice::~AudioDevice() { ma_device_uninit(&device); }
