#include "Transfer.hpp"

void printHex(unsigned char value) {
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(value);
}

void openStream(int *cdcFile){
    while(1){
    // Open the CDC device file for reading
        *cdcFile = open("/dev/ttyACM1", O_RDWR | O_NOCTTY);
        if (*cdcFile != -1) {
            return;
        }
        std::cerr << "Failed to open CDC device file" << std::endl;
    }
}

int initSerial(struct termios *tty, int *cdcFile){
    memset(tty, 0, sizeof(*tty));
    if (tcgetattr(*cdcFile, tty) != 0) {
        std::cerr << "Error in tcgetattr" << std::endl;
        return -1;
    }
    tcflush(*cdcFile, TCIFLUSH);
    cfsetospeed(tty, B115200);
    cfsetispeed(tty, B115200);
    tty->c_cflag |= (CLOCAL | CREAD);
    tty->c_cflag &= ~CSIZE;
    tty->c_cflag |= CS8;
    tty->c_cflag &= ~PARENB;
    tty->c_cflag &= ~CSTOPB;
    tty->c_cflag &= ~CRTSCTS;
    tty->c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty->c_iflag &= ~(IXON | IXOFF | IXANY);
    tty->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty->c_oflag &= ~OPOST;
    tty->c_cc[VMIN] = 1;
    tty->c_cc[VTIME] = 0;
    if (tcsetattr(*cdcFile, TCSANOW, tty) != 0) {
        std::cerr << "Error in tcsetattr" << std::endl;
        return -1;
    }

    std::cout << "OPENED!" << std::endl;
    tcflush(*cdcFile, TCIFLUSH);
    return 1;
}

int readCdcData(int * cdcFile) {

    unsigned char character[10];
    memset(character, 0x00, 10);
    // Read data from the CDC device file into the buffer
    int bytesRead = read(*cdcFile, character, 10);
    if (bytesRead == -1) {
        std::cerr << "Error in read" << std::endl;
        return -1;
    }
    for(int i = 0; i<10; i++)
    {
        printHex(character[i]);
        //std::cout << unsigned(character[i]);
        std::cout << " ";
    }
    
    std::cout << std::endl;
    std::cout << std::endl;

    return bytesRead;
}
