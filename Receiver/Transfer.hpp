#include "main.hpp"

void printHex(unsigned char value);
void openStream(int *cdcFile);
int initSerial(struct termios *tty, int *cdcFile);
int readCdcData(int * cdcFile);