#ifndef FRAME_H
#define FRAME_H

#include "QString"

class Frame
{
public:
    Frame(QString name, bool pinArray[16][16]);
    bool pinFrame[16][16];
    QString getName();
private:
    QString name;

};

#endif // FRAME_H
