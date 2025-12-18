#pragma once
#include <stdint.h>

/* Typedefs */
using ac_name_t = uint64_t; // 64-bit NAME
using ac_id_t   = uint8_t;  // 8-bit Source Address



/* Defines */
#define SIG_CLAIM_ADDR                  1
#define SIG_CHANGE_NAME                 2

#define ADDRESS_CLAIMED                 0
#define ADDRESS_CLAIM_TIMEOUT           1   

namespace Generic {
    enum class Status : char {
        BAD = -1,
        OK = 1,
        UNKNOWN = 2
    };
}