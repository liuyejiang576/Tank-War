// common.cpp

#include "common.h"

Direction turnLeft(Direction dir) {
    switch (dir) {
        case D_Left:  return D_Down;
        case D_Up:    return D_Left;
        case D_Right: return D_Up;
        case D_Down:  return D_Right;
        default:      return dir;
    }
}

Direction turnRight(Direction dir) {
    switch (dir) {
        case D_Left:  return D_Up;
        case D_Up:    return D_Right;
        case D_Right: return D_Down;
        case D_Down:  return D_Left;
        default:      return dir;
    }
}

Direction getOppositeDirection(Direction dir) {
    switch (dir) {
        case D_Left:  return D_Right;
        case D_Up:    return D_Down;
        case D_Right: return D_Left;
        case D_Down:  return D_Up;
        default:      return dir;
    }
}