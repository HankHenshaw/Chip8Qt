#ifndef CHIP8_H
#define CHIP8_H

#include <QDebug>
class Chip8
{
public:
    Chip8();
    unsigned char *getScreen();
    unsigned char *getKeyboard();
    unsigned char *getMemory();
    unsigned char getSoundTimer() const;
    unsigned char getDelayTimer() const;
    unsigned short getOpcode() const;
    bool isExtendedMod() const;
    void decrementTimer(int timer);
    void chipInit();
    void opcodeDecode();
private:
    unsigned char keyboard[16];
//    unsigned char screen[2048]; //64*32
    unsigned char screen[8192]; //128*64
    unsigned char memory[4096];
    unsigned char registers[16];
    unsigned char soundTimer;
    unsigned char delayTimer;
    unsigned char fontset[80];
    unsigned char sfontset[160]; // SHCIP
    unsigned short index;
    unsigned short stack[16];
    unsigned short stackPointer;
    unsigned short rplFlag; // SCHIP
    unsigned short programCounter;
    unsigned short opcode;
    bool isExtendedModEnable;

    void keyboardInit();
};

#endif // CHIP8_H
