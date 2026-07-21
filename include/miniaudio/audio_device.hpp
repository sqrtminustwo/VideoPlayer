#ifndef AUDIO_DEVICE_H
#define AUDIO_DEVICE_H

#include "types/types.hpp"
#include "miniaudio.h"

class AudioDevice {
    player_ptr player;

    ma_device_config deviceConfig;
    ma_device device;

  public:
    AudioDevice(player_ptr);
    ~AudioDevice();
};

#endif
