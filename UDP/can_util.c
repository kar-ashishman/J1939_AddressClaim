#include "can_defs.h"
int name_compare(unsigned char *name1, unsigned char *name2) {
    for (int i = 7; i >= 0; i--) {
        if (name1[i] > name2[i]) {
            return CONT2_WINS;
        } else if (name1[i] < name2[i]) {
            return CONT1_WINS;
        }
    }
    return NAMES_IDENTICAL; // Names are identical
}