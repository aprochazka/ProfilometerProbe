#include "Receiver.hpp"

Receiver::Receiver(Displayer *displayerPtr){
    dis = displayerPtr;
}

void Receiver::printHex(unsigned char value) {
    std::cout << "0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(value);
}

void Receiver::openStream(){
    while(1){
    // Open the CDC device file for reading
        cdcFile = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
        if(cdcFile == -1){
            std::cerr << "Failed to open CDC device file" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }
    
        std::cout << "successfully opened stream" << cdcFile << std::endl;
        break;
    }
}

int Receiver::initSerial(){
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(cdcFile, &tty) != 0) {
        std::cerr << "Error in tcgetattr" << std::endl;
        return -1;
    }
    //tcflush(cdcFile, TCIFLUSH);
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);
    tty.c_oflag &= ~OPOST;
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    if (tcsetattr(cdcFile, TCSANOW, &tty) != 0) {
        std::cerr << "Error in tcsetattr" << std::endl;
        return -1;
    }

    std::cout << "OPENED!" << std::endl;
    //tcflush(cdcFile, TCIFLUSH);
    return 1;
}

int Receiver::readCdcData(unsigned char (*character)[10]) {
    memset(*character, 0x00, 10);
    int bytesRead = read(cdcFile, character, 10);
    if (bytesRead == -1) {
        std::cerr << "Error in read" << std::endl;
        return -1;
    }
    if (bytesRead == -1) {
        std::cerr << "Error in read" << std::endl;
        return -1;
    }
    for(int i = 0; i<10; i++)
    {
        printHex((*character)[i]);
        //std::cout << unsigned(character[i]);
        //std::cout << " ";
    }
    std::cout << std::endl;
    return 0;
}

int Receiver::simulateRead(unsigned char (*character)[10]){
    memset(*character, 0x00, 10);

    int firstLen = (int)t1.size();
    int secondLen = (int)t2.size();
    int completeLen = firstLen + secondLen;

    int vecId;
    if(Receiver::simulateIdx < firstLen){
        vecId = 1;
    } 
    else{
        vecId = 2;
    }

    for(int i=0; i<10; i++){
        if(vecId == 1){
            if(simulateIdx > firstLen){
                (*character)[i] = 0x00;
            }
            else{
                (*character)[i] = t1[simulateIdx];
                simulateIdx++;
            }
        }
        else{
            if(simulateIdx > completeLen){
                (*character)[i] = 0x00;
            }
            else{
                //printHex(t1[simulateIdx-firstLen]);
                (*character)[i] = t2[simulateIdx-firstLen-1];
                simulateIdx++;
            }
        }
    }
    if(simulateIdx>completeLen) simulateIdx = 0;
    return 0;
}

int Receiver::fillBuffer(){
    unsigned char character[10];
    std::vector<uint8_t> tempVec{};
    
    Receiver::readCdcData(&character);
    
    //Receiver::simulateRead(&character);

    while(Receiver::findStart(&character) == -1) Receiver::readCdcData(&character); //Receiver::simulateRead(&character); 

    tempVec.insert(tempVec.end(), character, character + sizeof(character));

    do{
        Receiver::readCdcData(&character);
        //Receiver::simulateRead(&character);
        tempVec.insert(tempVec.end(), character, character + sizeof(character));
    }
    while(findEnd(&character) == -1);
    currentBufferIndexMutex.lock();
    
    currentBufferIndex++;
    currentBufferIndex %= 3;
    
    currentBufferIndexMutex.unlock();
    std::cout << "=" << currentBufferIndex;
    switch(currentBufferIndex){
        case 0:
            buffer1 = tempVec;
            break;
        case 1:
            buffer2 = tempVec;
            break;
        case 2:
            buffer3 = tempVec;
            break;
        default:
            std::cout << "error 25" << std::endl;
    }

    //for(int i=0; i<(int)tempVec.size(); i++) printHex(tempVec[i]);
    //std::cout << std::endl;
    return 1;
}

void Receiver::bufferToDisplay(){
    currentBufferIndexMutex.lock();
    int buffIdx = currentBufferIndex;
    currentBufferIndexMutex.unlock();
    switch(buffIdx){
        case 0:
            dis->vectorToTexture(&buffer1);
            break;
        case 1:
            dis->vectorToTexture(&buffer2);
            break;
        case 2:
            dis->vectorToTexture(&buffer3);
            break;
        default:
            std::cout << "error 86" << std::endl;
    }
}

int Receiver::findSequence(unsigned char (*str)[10], unsigned char ch1, unsigned char ch2){
    if(sequenceEndedFF){
        sequenceEndedFF = 0;
        if((*str)[0] == ch2){
            return 0;
        }
    }
    for(int i=0; i<10; i++){
        if((*str)[i] == ch1 && i < 10){
            if((*str)[i+1] == ch2){
                return i;
            }
        }
    }

    if((*str)[9] == 0xFF) sequenceEndedFF = 1;

    return -1;
}

int Receiver::findStart(unsigned char (*str)[10]){
    int res = findSequence(str, 0xFF, 0xD8);
    //if(res != -1)std::cout << "s";
    return res;
    //return findSequence(str, 0xFF, 0xD8);
}

int Receiver::findEnd(unsigned char (*str)[10]){
    int res = findSequence(str, 0xFF, 0xD9);
    //if(res != -1)std::cout << "f";
    return res;
    //return findSequence(str, 0xFF, 0xD9);
}
