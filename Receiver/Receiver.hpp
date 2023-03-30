#include "main.hpp"

#ifndef DISPLAYER_HPP
#include "Displayer.hpp"
#endif

class Receiver{
    private:
        int cdcFile;
        struct termios tty;
        int currentBufferIndex = 0;
        std::mutex currentBufferIndexMutex;
        std::vector<uint8_t> buffer1;
        std::vector<uint8_t> buffer2;
        std::vector<uint8_t> buffer3;
        Displayer *dis;

        int sequenceEndedFF = 0;
        int simulateIdx = 0;
    
    public:
        Receiver(Displayer *displayerPtr);
        void openStream();
        int initSerial();
        int readCdcData(unsigned char (*character)[10]);

        void printHex(unsigned char value);
        int fillBuffer();

        void bufferToDisplay();

        int findSequence(unsigned char (*str)[10], unsigned char ch1, unsigned char ch2);
        int findStart(unsigned char (*str)[10]);
        int findEnd(unsigned char (*str)[10]);
        
        int simulateRead(unsigned char (*character)[10]);
};