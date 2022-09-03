#include "helpers.h"

void remove_first_x (int x) {
    pos = (pos + x) % 1024;
    size -= x;
}