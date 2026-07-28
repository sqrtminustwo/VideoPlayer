#ifndef PACKET_GUARD_H
#define PACKET_GUARD_H

#include "types/types.hpp"

struct PacketGuard {
    packet_ptr &packet;

    PacketGuard() = delete;
    PacketGuard(packet_ptr &packet);
    ~PacketGuard();
};

#endif
