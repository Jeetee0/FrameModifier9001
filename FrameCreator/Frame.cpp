#include "Frame.h"
#include "stdlib.h"
#include <iostream>
#include <array>


Frame::Frame(QString Name, bool pinArray[16][16]) {
    this->name = Name;
    std::copy(&pinArray[0][0], &pinArray[0][0]+16*16, &this->pinFrame[0][0]);
}

QString Frame::getName() {
    return name;
}
