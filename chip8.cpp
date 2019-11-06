#include "chip8.h"

Chip8::Chip8()
{
    chipInit();
}

unsigned char *Chip8::getScreen()
{
    return screen;
}
unsigned char *Chip8::getKeyboard()
{
    return keyboard;
}
unsigned char *Chip8::getMemory()
{
    return memory;
}
unsigned char Chip8::getDelayTimer() const
{
    return delayTimer;
}

unsigned short Chip8::getOpcode() const
{
    return opcode;
}

bool Chip8::isExtendedMod() const
{
    return isExtendedModEnable;
}
unsigned char Chip8::getSoundTimer() const
{
    return soundTimer;
}
void Chip8::decrementTimer(int timer)
{
    timer ? --delayTimer : --soundTimer;
}
void Chip8::chipInit()
{
    keyboardInit();
    programCounter = 0x200;
    opcode = 0;
    index = 0;
    stackPointer = 0;
    delayTimer = 0;
    soundTimer = 0;
    rplFlag = 0xF0; // ???
    isExtendedModEnable = false; //SCHIP

    for(int i = 0; i < 8192; ++i) // SCHIP
        screen[i] = 0;

    for(int i = 0; i < 4096; ++ i)
        memory[i] = 0;

    for(int i = 0; i < 80; ++i)
        memory[i] = fontset[i];

    for(int i = 0; i < 160; ++i) // SCHIP
        memory[0x50 + i] = sfontset[i];

    for(int i = 0; i < 16; ++i)
    {
        stack[i] = 0;
        registers[i] = 0;
        keyboard[i] = 0;
    }

}

void Chip8::opcodeDecode()
{
    opcode = (memory[programCounter] << 8) | memory[programCounter + 1];
    qDebug() << "Opcode:" << hex << opcode;
    switch (opcode & 0xF000) {
    case 0x0000:
        if((opcode & 0x00F0) == 0x00C0) // Прокрутить экран на N строк вниз
        {//SCHIP
            int value = opcode & 0x000F;
            char tempScreen[128*64] = {};
            int width = isExtendedModEnable ? 128 : 64;
            memcpy(tempScreen+(width*value), screen, (128*64) - width*value);
            memcpy(screen, tempScreen, sizeof(tempScreen));

            programCounter += 2;
            qDebug() << "0x00C0";
        } else {
            switch (opcode & 0x00FF) {
            case 0x00E0: // Очистить экран
                for(int i = 0; i < 2048; ++i)
                    screen[i] = 0;
                programCounter += 2;
                qDebug() << "0x00E0";
                break;
            case 0x00EE: // Возврат из подпрограммы
                --stackPointer;
                programCounter = stack[stackPointer];
                programCounter += 2;
                qDebug() << "0x00EE";
                break;
            case 0x00FB: //Прокрутить экран на 4 (2) пикселя вправо в режиме 128x64 (64x32)
            {
                uchar tempScreen[8192];
                memcpy(tempScreen, screen, 8192);

                if(isExtendedModEnable) //Прокрупка вправо на 4
                {
                    for(int i = 0; i < 64; ++i)
                    {
                        for(int j = 127; j >= 0; --j)
                        {
                            if(j < 4)
                                screen[i*128+j] = 0;
                            else
                                screen[i*128+j] = tempScreen[i*128+j-4];
                        }
                    }
                } else { //Прокрупка вправо на 2
                    for(int i = 0; i < 32; ++i)
                    {
                        for(int j = 63; j >= 0; --j)
                        {
                            if(j < 2)
                                screen[i*64+j] = 0;
                            else
                                screen[i*64+j] = tempScreen[i*64+j-2];
                        }
                    }
                }
                programCounter += 2;
                qDebug() << "0x00FB";
                break;
            }
            case 0x00FC://Прокрутить экран на 4 (2) пикселя влево в режиме 128x64 (64x32)
            {
                uchar tempScreen[8192]; //memcpy
                memcpy(tempScreen, screen, 8192);

                if(isExtendedModEnable) // прокрутка влево на 4
                {
                    for(int i = 0; i < 64; ++i)
                        for(int j = 0; j < 128; ++j)
                        {
                            if(j > 128 - 4)
                                screen[i*128+j] = 0;
                            else
                                screen[i*128+j] = tempScreen[i*128+j+4];
                        }
                } else { // прокрутка влево на 2
                    for(int i = 0; i < 32; ++i)
                        for(int j = 0; j < 64; ++j)
                        {
                            if(j > 64 - 2)
                                screen[i*64+j] = 0;
                            else
                                screen[i*64+j] = tempScreen[i*64+j+2];
                        }
                }
                programCounter += 2;
                qDebug() << "0x00FB";
                break;
            }
            case 0x00FD://Завершить программу
            {
                chipInit();

                qDebug() << "0x00FD";
                break;
            }
            case 0x00FE: //Установить разрешение экрана 64x32
            {
                isExtendedModEnable = false;

                programCounter += 2;
                qDebug() << "0x00FE";
                break;
            }
            case 0x00FF: //Установить разрешение экрана 128x64
            {
                isExtendedModEnable = true;

                programCounter += 2;
                qDebug() << "0x00FE";
                break;
            }
            default:
                qDebug() << "Unknwon 0x00NN chip.opcode";
            }
        }
        break;
    case 0x1000: // Перейти по адресу NNN
    {
        unsigned short address = opcode & 0x0FFF;
        programCounter = address;
        qDebug() << "0x1NNN";
        break;
    }
    case 0x2000: // Вызов подпрограммы по адресу NNN
    {
        unsigned short address = opcode & 0x0FFF;
        stack[stackPointer] = programCounter;
        ++stackPointer;
        programCounter = address;
        qDebug() << "0x2NNN";
        break;
    }
    case 0x3000: // Пропустить следующую инструкцию, если VX = NN
    {
        unsigned short regNumber = (opcode & 0x0F00) >> 8;
        unsigned short address = opcode & 0x00FF;
        programCounter += 2;
        if(registers[regNumber] == address)
            programCounter += 2;
        qDebug() << "0x3XNN";
        break;
    }
    case 0x4000: // Пропустить следующую инструкцию, если VX != NN
    {
        unsigned short regNumber = (opcode & 0x0F00) >> 8;
        unsigned short address = opcode & 0x00FF;
        programCounter += 2;
        if(registers[regNumber] != address)
            programCounter += 2;
        qDebug() << "0x4XNN";
        break;
    }
    case 0x5000: // Пропустить следующую инструкцию, если VX = VY
        if((opcode & 0x000F) == 0)
        {
            unsigned short regNumberX = (opcode & 0x0F00) >> 8;
            unsigned short regNumberY = (opcode & 0x00F0) >> 4;
            programCounter += 2;
            if(registers[regNumberX] == registers[regNumberY])
                programCounter += 2;
            qDebug() << "0x5XY0";
        }
        else
            qDebug() << "Unknown 0x5XY0 chip.opcode";
        break;
    case 0x6000: // Загрузить в регистр VX число NN
    {
        unsigned short regNumber = (opcode & 0x0F00) >> 8;
        unsigned char value = opcode & 0x00FF;

        registers[regNumber] = value;
        programCounter += 2;

        qDebug() << "0x6XNN";
        break;
    }
    case 0x7000: // Загрузить в регистр VX сумму VX и NN
    {
        unsigned short regNumber = (opcode & 0x0F00) >> 8;
        unsigned short value = opcode & 0x00FF;
        registers[regNumber] += value;
        programCounter += 2;
        qDebug() << "0x7XNN";
        break;
    }
    case 0x8000:
        switch (opcode & 0x000F) {
        case 0x0000: // Присвоить регистру VX значение VY
        {
            unsigned short regNumberX = (opcode & 0x0F00) >> 8;
            unsigned short regNumberY = (opcode & 0x00F0) >> 4;
            registers[regNumberX] = registers[regNumberY];
            programCounter += 2;
            qDebug() << "0x8XY0";
            break;
        }
        case 0x0001: // Выполнить операцию логическое “ИЛИ” над значениями регистров VX и VY, результат сохранить в VX
        {
            unsigned short regNumberX = (opcode & 0x0F00) >> 8;
            unsigned short regNumberY = (opcode & 0x00F0) >> 4;
            registers[regNumberX] |= registers[regNumberY];
            programCounter += 2;
            qDebug() << "0x8XY1";
            break;
        }
        case 0x0002: // Выполнить операцию логическое “И” над значениями регистров VX и VY, результат сохранить в VX
        {
            unsigned short regNumberX = (opcode & 0x0F00) >> 8;
            unsigned short regNumberY = (opcode & 0x00F0) >> 4;
            registers[regNumberX] &= registers[regNumberY];
            programCounter += 2;
            qDebug() << "0x8XY2";
            break;
        }
        case 0x0003: // Выполнить операцию исключающее “ИЛИ” над значениями регистров VX и VY, результат сохранить в VX
        {
            unsigned short regNumberX = (opcode & 0x0F00) >> 8;
            unsigned short regNumberY = (opcode & 0x00F0) >> 4;
            registers[regNumberX] ^= registers[regNumberY];
            programCounter += 2;
            qDebug() << "0x8XY3";
            break;
        }
        case 0x0004: // Значения VX и VY суммируются. Если результат больше, чем 255 , то VF установить в 1, иначе 0. Только младшие 8 бит результата сохраняются в VX
        {
            unsigned short regNumberX = (opcode & 0x0F00) >> 8;
            unsigned short regNumberY = (opcode & 0x00F0) >> 4;
            unsigned short value = registers[regNumberX] + registers[regNumberY];
            registers[regNumberX] = value & 0x00FF;
            if(value > 255)
                registers[0xF] = 1;
            else
                registers[0xF] = 0;
            programCounter += 2;
            qDebug() << "0x8XY4";
            break;
        }
        case 0x0005: // Если VX >= VY, то VF установить в 1, иначе 0.Потом VY вычитается из VX, результат сохранить в VX.
        {
            unsigned short regNumberX = (opcode & 0x0F00) >> 8;
            unsigned short regNumberY = (opcode & 0x00F0) >> 4;
            unsigned short value = registers[regNumberX] - registers[regNumberY];
            if(registers[regNumberX] >= registers[regNumberY])
                registers[0xF] = 0x1;
            else
                registers[0xF] = 0x0;
            registers[regNumberX] = value & 0x00FF;
            programCounter += 2;
            qDebug() << "0x8XY5";
            break;
        }
        case 0x0006: // Операция сдвига вправо на 1 бит. Сдвигается регистр VX. Если младший бит (самый правый) регистра VX равен 1, то VF = 1, иначе VF = 0
        {
            unsigned short regNumberX = (opcode & 0x0F00) >> 8;
            registers[0xF] = registers[regNumberX] & 0x1;
            registers[regNumberX] >>= 1;
            programCounter += 2;
            qDebug() << "0x8XY6";
            break;
        }
        case 0x0007: // Если VY >= VX, то VF = 1, иначе 0. Потом VX вычитается из VY, и результат сохраняется в VX
        {
            unsigned short regNumberX = (opcode & 0x0F00) >> 8;
            unsigned short regNumberY = (opcode & 0x00F0) >> 4;

            if(registers[regNumberY] >= registers[regNumberX])
                registers[0xF] = 1;
            else
                registers[0xF] = 0;

            registers[regNumberX] = registers[regNumberY] - registers[regNumberX];
            programCounter += 2;
            qDebug() << "0x8XY7";
            break;
        }
        case 0x000E: // Операция сдвига влево на 1 бит. Сдвигается регистр VX. Если старший бит (самый левый) регистра VX равен 1, то VF = 1, иначе VF = 0
        {
            unsigned short regNumberX = (opcode & 0x0F00) >> 8;

            registers[0xF] = (registers[regNumberX] >> 7) & 0x1;

            registers[regNumberX] <<= 1;
            programCounter += 2;

            qDebug() << "0x8XYE";
            break;
        }
        default:
            qDebug() << "Unknown 0x8XYN chip.opcode";
        }
        break;
    case 0x9000: // Пропустить следующую инструкцию, если VX != VY
        if((opcode & 0x000F) == 0)
        {
            unsigned short regNumberX = (opcode & 0x0F00) >> 8;
            unsigned short regNumberY = (opcode & 0x00F0) >> 4;
            programCounter += 2;
            if(registers[regNumberX] != registers[regNumberY])
                programCounter += 2;
            qDebug() << "0x9XY0";
        }
        else
            qDebug() << "Unknown 0x9XYN chip.opcode";
        break;
    case 0xA000: // Значение регистра I устанавливается в NNN
    {
        unsigned short value = opcode & 0x0FFF;
        index = value;
        programCounter += 2;
        qDebug() << "0xANNN";
        break;
    }
    case 0xB000: // Перейти по адресу NNN + V0
    {
        unsigned short address = opcode & 0x0FFF;
        programCounter = address + registers[0];
        qDebug() << "0xBNNN";
        break;
    }
    case 0xC000: // VX = (Случайное число от 0 до 255) & NN
    {
        unsigned short regNumber = (opcode & 0x0F00) >> 8;
        unsigned short value = opcode & 0x00FF;
        registers[regNumber] = (qrand()%255) & value;
        programCounter += 2;
        qDebug() << "0xCXNN";
        break;
    }
    case 0xD000: // Нарисовать на экране спрайт. Эта инструкция считывает N байт по адресу в регистре I и рисует их на экране в виде спрайта с координатами VX, VY
    {//При DXY0 рисует спрайты 8x16 (16x16) в режиме 64x32 (128x64)
        unsigned short regNumberX = (opcode & 0x0F00) >> 8;
        unsigned short regNumberY = (opcode & 0x00F0) >> 4;
        unsigned short value = opcode & 0x000F;

        uint8_t xPos, yPos;
        ushort w, h;
        if(value)
        {
            w = value;
            h = 8;
            if(!isExtendedModEnable)
            {
                xPos = registers[regNumberX] % 64;
                yPos = registers[regNumberY] % 32;
            } else {
                xPos = registers[regNumberX] % 128;
                yPos = registers[regNumberY] % 64;
            }
            registers[15] = 0;
            for(ushort row = 0; row < w; ++row)
            {
                unsigned short spriteByte;
                spriteByte = memory[index + row];

                for(ushort col = 0; col < h; ++col)
                {
                    unsigned short spritePixel;
                    spritePixel = spriteByte & (0x80 >> col);

                    unsigned char *screenPixel;
                    if(isExtendedModEnable)
                        screenPixel = &screen[(yPos + row)*128+(xPos+col)];
                    else
                        screenPixel = &screen[(yPos + row)*64+(xPos+col)];
                    if(spritePixel)
                    {
                        if(*screenPixel == 0xFF)
                            registers[15] = 1;
                        *screenPixel ^= 0xFF;
                    }
                }
            }
        } else {
            w = 16;
            h = 16;
            xPos = registers[regNumberX] % 128;
            yPos = registers[regNumberY] % 64;
            registers[0xF] = 0;
            ushort offset = index;
            for(ushort row = 0; row < w; ++row)
            {
                unsigned short spriteByte;
                spriteByte = static_cast<ushort>((memory[offset] << 8) + memory[offset+1]);
                offset += 2;

                for(ushort col = 0; col < h; ++col)
                {
                    unsigned short spritePixel = 0;
                    spritePixel = spriteByte & (0x8000 >> col);

                    unsigned char *screenPixel;
                    screenPixel = &screen[(yPos + row)*128+(xPos+col)];
                    if(spritePixel)
                    {
                        if(*screenPixel == 0xFF)
                            registers[15] = 1;
                        *screenPixel ^= 0xFF;
                    }
                }
            }
        }
        programCounter += 2;

        qDebug() << "0xDXYN";
        break;
    }
    case 0xE000:
        switch (opcode & 0x00FF) {
        case 0x00A1: // Пропустить следующую инструкцию если клавиша, номер которой хранится в регистре VX, не нажата
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            unsigned short key = registers[regNumber];
            programCounter += 2;
            if(!keyboard[key])
                programCounter += 2;
            qDebug() << "0xEXA1";
            break;
        }
        case 0x009E: // Пропустить следующую инструкцию если клавиша, номер которой хранится в регистре VX, нажата
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            unsigned short key = registers[regNumber];
            programCounter += 2;
            if(keyboard[key])
                programCounter += 2;
            qDebug() << "0xEX9E";
            break;
        }
        default:
            qDebug() << "Unknown 0xEXNN chip.opcode";
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF) {
        case 0x0007: // Регистру VX присваивается значение таймера задержки
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            registers[regNumber] = delayTimer;
            programCounter += 2;
            qDebug() << "0xFX07";
            break;
        }
        case 0x000A: // Ждать нажатия любой клавиши. Как только клавиша будет нажата, записать ее номер в регистр VX и перейти к выполнению следующей инструкции
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            for(unsigned char i = 0; i < 16; ++i)
            {
                if(keyboard[i])
                {
                    registers[regNumber] = i;
                    programCounter += 2;
                    break;
                }
            }

            qDebug() << "0xFX0A";
            break;
        }
        case 0x0015: // Установить значение таймера задержки равным значению регистра VX
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            delayTimer = registers[regNumber];
            programCounter += 2;
            qDebug() << "0xFX15";
            break;
        }
        case 0x0018: // Установить значение звукового таймера равным значению регистра VX
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            soundTimer = registers[regNumber];
            programCounter += 2;
            qDebug() << "0xFX18";
            break;
        }
        case 0x001E: // Присвоить регистру I сумму I и VX
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            index += registers[regNumber];
            programCounter += 2;
            qDebug() << "0xFX1E";
            break;
        }
        case 0x0029: //Используется для вывода на экран символов встроенного шрифта размером 4x5 пикселей.
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            unsigned short value = registers[regNumber];

            index = 0x0 + (5*value);

            programCounter += 2;
            qDebug() << "0xFX29";
            break;
        }
        case 0x0030: //Аналогично инструкции FX29 , но загружает спрайты 8x10
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            unsigned short value = registers[regNumber];

            index = 0x50 + (10*value);

            programCounter += 2;
            qDebug() << "0xFX30";
            break;
        }
        case 0x0033: // Сохранить значение регистра VX в двоично-десятичном представлении по адресам I, I+1 и I+2
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            unsigned short value = registers[regNumber];

            memory[index + 2] = value%10; // Единицы
            value /= 10;
            memory[index + 1] = value%10; //Десятки
            value /= 10;
            memory[index] = value%10;//Сотни

            programCounter += 2;
            qDebug() << "0xFX33";
            break;
        }
        case 0x0055: // Сохранить значения регистров от V0 до VX в памяти, начиная с адреса в I
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            for(int i = 0; i <= regNumber; ++i)
            {
                memory[index + i] = static_cast<uchar>(registers[i]);
            }
            programCounter += 2;
            qDebug() << "0xFX55";
            break;
        }
        case 0x0065: // Загрузить значения регистров от V0 до VX из памяти, начиная с адреса в I
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            for(int i = 0; i <= regNumber; ++i)
            {
                registers[i] = memory[index + i];
            }
            programCounter += 2;
            qDebug() << "0xFX65";
            break;

        }
        case 0x0075: //Сохранить регистры V0 … VX в пользовательских флагах RPL
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            for(int i = 0; i <= regNumber; ++i)
                memory[rplFlag + i] = registers[i];

            programCounter += 2;
            qDebug() << "0xFX75";
            break;
        }
        case 0x0085: //Загрузить регистры V0 … VX из пользовательских флагов RPL
        {
            unsigned short regNumber = (opcode & 0x0F00) >> 8;
            for(int i = 0; i <= regNumber; ++i)
                 registers[i] = memory[rplFlag + i];

            programCounter += 2;
            qDebug() << "0xFX85";
            break;
        }
        default:
            qDebug() << "Unknown FXNN chip.opcode";
        }
        break;
    default:
        qDebug() << "If u get there, something really went wrong";
    }
}

void Chip8::keyboardInit()
{
    /*CHIP8*/
    //0
    fontset[0] = 0xF0;
    fontset[1] = 0x90;
    fontset[2] = 0x90;
    fontset[3] = 0x90;
    fontset[4] = 0xF0;
    //1
    fontset[5] = 0x20;
    fontset[6] = 0x60;
    fontset[7] = 0x20;
    fontset[8] = 0x20;
    fontset[9] = 0x70;
    //2
    fontset[10] = 0xF0;
    fontset[11] = 0x10;
    fontset[12] = 0xF0;
    fontset[13] = 0x80;
    fontset[14] = 0x70;
    //3
    fontset[15] = 0xF0;
    fontset[16] = 0x10;
    fontset[17] = 0xF0;
    fontset[18] = 0x10;
    fontset[19] = 0xF0;
    //4
    fontset[20] = 0x90;
    fontset[21] = 0x90;
    fontset[22] = 0xF0;
    fontset[23] = 0x10;
    fontset[24] = 0x10;
    //5
    fontset[25] = 0xF0;
    fontset[26] = 0x80;
    fontset[27] = 0xF0;
    fontset[28] = 0x10;
    fontset[29] = 0xF0;
    //6
    fontset[30] = 0xF0;
    fontset[31] = 0x80;
    fontset[32] = 0xF0;
    fontset[33] = 0x90;
    fontset[34] = 0xF0;
    //7
    fontset[35] = 0xF0;
    fontset[36] = 0x10;
    fontset[37] = 0x20;
    fontset[38] = 0x40;
    fontset[39] = 0x40;
    //8
    fontset[40] = 0xF0;
    fontset[41] = 0x90;
    fontset[42] = 0xF0;
    fontset[43] = 0x90;
    fontset[44] = 0xF0;
    //9
    fontset[45] = 0xF0;
    fontset[46] = 0x90;
    fontset[47] = 0xF0;
    fontset[48] = 0x10;
    fontset[49] = 0xF0;
    //A
    fontset[50] = 0xF0;
    fontset[51] = 0x90;
    fontset[52] = 0xF0;
    fontset[53] = 0x90;
    fontset[54] = 0x90;
    //B
    fontset[55] = 0xE0;
    fontset[56] = 0x90;
    fontset[57] = 0xE0;
    fontset[58] = 0x90;
    fontset[59] = 0xE0;
    //C
    fontset[60] = 0xF0;
    fontset[61] = 0x80;
    fontset[62] = 0x80;
    fontset[63] = 0x80;
    fontset[64] = 0xF0;
    //D
    fontset[65] = 0xE0;
    fontset[66] = 0x90;
    fontset[67] = 0x90;
    fontset[68] = 0x90;
    fontset[69] = 0xE0;
    //E
    fontset[70] = 0xF0;
    fontset[71] = 0x80;
    fontset[72] = 0xF0;
    fontset[73] = 0x80;
    fontset[74] = 0xF0;
    //F
    fontset[75] = 0xF0;
    fontset[76] = 0x80;
    fontset[77] = 0xF0;
    fontset[78] = 0x80;
    fontset[79] = 0x80;
    /*CHIP8*/
    /*SCHIP*/
    //0
    sfontset[0] = 0xF0;
    sfontset[1] = 0xF0;
    sfontset[2] = 0x90;
    sfontset[3] = 0x90;
    sfontset[4] = 0x90;
    sfontset[5] = 0x90;
    sfontset[6] = 0x90;
    sfontset[7] = 0x90;
    sfontset[8] = 0xF0;
    sfontset[9] = 0xF0;
    //1
    sfontset[10] = 0x20;
    sfontset[11] = 0x20;
    sfontset[12] = 0x60;
    sfontset[13] = 0x60;
    sfontset[14] = 0x20;
    sfontset[15] = 0x20;
    sfontset[16] = 0x20;
    sfontset[17] = 0x20;
    sfontset[18] = 0x70;
    sfontset[19] = 0x70;
    //2
    sfontset[20] = 0xF0;
    sfontset[21] = 0xF0;
    sfontset[22] = 0x10;
    sfontset[23] = 0x10;
    sfontset[24] = 0xF0;
    sfontset[25] = 0xF0;
    sfontset[26] = 0x80;
    sfontset[27] = 0x80;
    sfontset[28] = 0xF0;
    sfontset[29] = 0xF0;
    //3
    sfontset[30] = 0xF0;
    sfontset[31] = 0xF0;
    sfontset[32] = 0x10;
    sfontset[33] = 0x10;
    sfontset[34] = 0xF0;
    sfontset[35] = 0xF0;
    sfontset[36] = 0x10;
    sfontset[37] = 0x10;
    sfontset[38] = 0xF0;
    sfontset[39] = 0xF0;
    //4
    sfontset[40] = 0x90;
    sfontset[41] = 0x90;
    sfontset[42] = 0x90;
    sfontset[43] = 0x90;
    sfontset[44] = 0xF0;
    sfontset[45] = 0xF0;
    sfontset[46] = 0x10;
    sfontset[47] = 0x10;
    sfontset[48] = 0x10;
    sfontset[49] = 0x10;
    //5
    sfontset[50] = 0xF0;
    sfontset[51] = 0xF0;
    sfontset[52] = 0x80;
    sfontset[53] = 0x80;
    sfontset[54] = 0xF0;
    sfontset[55] = 0xF0;
    sfontset[56] = 0x10;
    sfontset[57] = 0x10;
    sfontset[58] = 0xF0;
    sfontset[59] = 0xF0;
    //6
    sfontset[60] = 0xF0;
    sfontset[61] = 0xF0;
    sfontset[62] = 0x80;
    sfontset[63] = 0x80;
    sfontset[64] = 0xF0;
    sfontset[65] = 0xF0;
    sfontset[66] = 0x90;
    sfontset[67] = 0x90;
    sfontset[68] = 0xF0;
    sfontset[69] = 0xF0;
    //7
    sfontset[70] = 0xF0;
    sfontset[71] = 0xF0;
    sfontset[72] = 0x10;
    sfontset[73] = 0x10;
    sfontset[74] = 0x20;
    sfontset[75] = 0x20;
    sfontset[76] = 0x40;
    sfontset[77] = 0x40;
    sfontset[78] = 0x40;
    sfontset[79] = 0x40;
    //8
    sfontset[80] = 0xF0;
    sfontset[81] = 0xF0;
    sfontset[82] = 0x90;
    sfontset[83] = 0x90;
    sfontset[84] = 0xF0;
    sfontset[85] = 0xF0;
    sfontset[86] = 0x90;
    sfontset[87] = 0x90;
    sfontset[88] = 0xF0;
    sfontset[89] = 0xF0;
    //9
    sfontset[90] = 0xF0;
    sfontset[91] = 0xF0;
    sfontset[92] = 0x90;
    sfontset[93] = 0x90;
    sfontset[94] = 0xF0;
    sfontset[95] = 0xF0;
    sfontset[96] = 0x10;
    sfontset[97] = 0x10;
    sfontset[98] = 0xF0;
    sfontset[99] = 0xF0;
    //A
    sfontset[100] = 0xF0;
    sfontset[101] = 0xF0;
    sfontset[102] = 0x90;
    sfontset[103] = 0x90;
    sfontset[104] = 0xF0;
    sfontset[105] = 0xF0;
    sfontset[106] = 0x90;
    sfontset[107] = 0x90;
    sfontset[108] = 0x90;
    sfontset[109] = 0x90;
    //B
    sfontset[110] = 0xE0;
    sfontset[111] = 0xE0;
    sfontset[112] = 0x90;
    sfontset[113] = 0x90;
    sfontset[114] = 0xE0;
    sfontset[115] = 0xE0;
    sfontset[116] = 0x90;
    sfontset[117] = 0x90;
    sfontset[118] = 0xE0;
    sfontset[119] = 0xE0;
    //C
    sfontset[120] = 0xF0;
    sfontset[121] = 0xF0;
    sfontset[122] = 0x80;
    sfontset[123] = 0x80;
    sfontset[124] = 0x80;
    sfontset[125] = 0x80;
    sfontset[126] = 0x80;
    sfontset[127] = 0x80;
    sfontset[128] = 0xF0;
    sfontset[129] = 0xF0;
    //D
    sfontset[130] = 0xE0;
    sfontset[131] = 0xE0;
    sfontset[132] = 0x90;
    sfontset[133] = 0x90;
    sfontset[134] = 0x90;
    sfontset[135] = 0x90;
    sfontset[136] = 0x90;
    sfontset[137] = 0x90;
    sfontset[138] = 0xE0;
    sfontset[139] = 0xE0;
    //E
    sfontset[140] = 0xF0;
    sfontset[141] = 0xF0;
    sfontset[142] = 0x80;
    sfontset[143] = 0x80;
    sfontset[144] = 0xF0;
    sfontset[145] = 0xF0;
    sfontset[146] = 0x80;
    sfontset[147] = 0x80;
    sfontset[148] = 0xF0;
    sfontset[149] = 0xF0;
    //F
    sfontset[150] = 0xF0;
    sfontset[151] = 0xF0;
    sfontset[152] = 0x80;
    sfontset[153] = 0x80;
    sfontset[154] = 0xF0;
    sfontset[155] = 0xF0;
    sfontset[156] = 0x80;
    sfontset[157] = 0x80;
    sfontset[158] = 0x80;
    sfontset[159] = 0x80;
    /*SCHIP*/
}
