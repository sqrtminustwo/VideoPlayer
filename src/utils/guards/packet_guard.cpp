#include "utils/guards/packet_guard.hpp"

extern "C" {
#include <libavcodec/packet.h>
}

PacketGuard::PacketGuard(packet_ptr &packet) : packet{packet} {}
PacketGuard::~PacketGuard() { av_packet_unref(packet.get()); }
