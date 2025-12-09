#pragma once
#include "GlobalDefs.h"

class UdpPacket {
private:
unsigned int    udpPacketId;        // 4 byte identifier that represents a CAN ID (29 bit)
unsigned int    udpPacketData[3];   // 8 byte data similar to a CAN frame data

public:
    UdpPacket(const unsigned int id, const unsigned int data[3]);
    Generic::Status CheckFrameStr();
    unsigned char UdpIdToCanIDParsing(void);
    void pr();

};