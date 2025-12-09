#include <iostream>
#include "UdpPacket.h"
#include "GlobalDefs.h"


UdpPacket::UdpPacket(const unsigned int id, const unsigned int data[3]) {
    this->udpPacketId = id;
    this->udpPacketData[0] = data[0];
    this->udpPacketData[1] = data[1];
    this->udpPacketData[2] = data[2];
}

Generic::Status UdpPacket::CheckFrameStr(void) {
    return Generic::Status::OK;
}

unsigned char UdpPacket::UdpIdToCanIDParsing(void) {
    return (this->udpPacketId & 0x00FF0000)>>16;
}

void UdpPacket::pr() {
    std::cout << this->udpPacketId << std::endl;
    std::cout << this->udpPacketData[0] << " " << udpPacketData[1] << " " << udpPacketData[2] << std::endl;
}

int main() {
    unsigned int x[] = {10,20,30};
    UdpPacket packet(0x12345678, x);
    packet.pr();
    std::cout << std::endl << 1 << "\n" << (int)packet.CheckFrameStr();
}