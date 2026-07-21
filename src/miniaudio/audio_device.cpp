#include "miniaudio/audio_device.hpp"
#include "ffmpeg/player/player.hpp" // IWYU pragma: keep

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/file.h>
}

void data_callback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
    // TODO:
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
    deviceConfig.pUserData = 0;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open audio playback device.\n");
        return;
    }
}

AudioDevice::~AudioDevice() { ma_device_uninit(&device); }
