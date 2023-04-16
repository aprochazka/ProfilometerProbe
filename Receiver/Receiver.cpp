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
        cdcFile = open("/dev/ttyACM1", O_RDWR | O_NOCTTY);
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
    return 1;
}

int Receiver::readCdcData(uint8_t (*character)[CDC_FRAME_SIZE]) {
    memset(*character, 0x00, CDC_FRAME_SIZE);
    int bytesRead = read(cdcFile, character, CDC_FRAME_SIZE);
    if (bytesRead == -1) {
        std::cerr << "Error in read" << std::endl;
        return -1;
    }
    if (bytesRead == -1) {
        std::cerr << "Error in read" << std::endl;
        return -1;
    }
    #if 0 // PRINT WHAT IS RECEIVED

        for(int i = 0; i<CDC_FRAME_SIZE; i++)
        {
            printHex((*character)[i]);
            //std::cout << unsigned(character[i]);
            std::cout << " ";
        }
        std::cout << std::endl << std::endl;

    #endif

    return 0;
}

int Receiver::simulateRead(unsigned char (*character)[CDC_FRAME_SIZE]){
    memset(*character, 0x00, CDC_FRAME_SIZE);

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

    for(int i=0; i<CDC_FRAME_SIZE; i++){
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
                (*character)[i] = t2[simulateIdx-firstLen-1];
                simulateIdx++;
            }
        }
    }
    if(simulateIdx>completeLen) simulateIdx = 0;
    return 0;
}

int Receiver::getDistance(){
    uint16_t dist = 0;
    int distCorrupted = 1;
    unsigned char distanceFrame[12];
    int bytesRead = read(cdcFile, distanceFrame, 12);
    if(bytesRead == -1){
            std::cout<<"ERROR IN READ!!! ";
    }
    std::cout<<"distance: ";
    for(int i=0; i<7; i++){
        if(distanceFrame[i] == 0xff && distanceFrame[i+1] == 0x69 && distanceFrame [i+4] == 0xff && distanceFrame[i+5] == 0x69){
            
            
            
            

            dist = (distanceFrame[i+2] << 8 | distanceFrame[i+3]);


            std::cout << " - " << static_cast<int>(dist) << " - ";
            
            Receiver::printHex(distanceFrame[i+2]);
            std::cout << " ";
            Receiver::printHex(distanceFrame[i+3]);
            std::cout << std::endl;
            
            distCorrupted = 0;
            
            return 0;
        }
    }
    if(distCorrupted) {
            std::cout << "Corrupted :( -> ";
            for(int i = 0; i<12; i++) {
                Receiver::printHex(distanceFrame[i]);
                std::cout << " ";
            }
            std::cout << std::endl;
    }
    
    return 1;
}

int Receiver::fillBuffer(){
    unsigned char character[CDC_FRAME_SIZE];
    std::vector<uint8_t> tempVec{};
    
    int i = -1;
    do {
        Receiver::readCdcData(&character); 
        i = Receiver::findStart(&character);
    }
    while(i == -1); 

    tempVec.insert(tempVec.end(), &character[i], character + sizeof(character));

    i = -1;
    do{
        Receiver::readCdcData(&character);
        i = findEnd(&character);
        if(i == -1)
        tempVec.insert(tempVec.end(), character, character + sizeof(character));
    }
    while(i == -1);
    
    tempVec.insert(tempVec.end(), character, character + i + 2);
    
    Receiver::getDistance();

    //////////// SAVE FRAMES TO FILES
    /*
    std::ofstream outfile(std::to_string(debugFileIdx) + ".jpg", std::ios::out | std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Failed to create file " << debugFileIdx << ".jpg" << std::endl;
        return 1;
    }
    outfile.write(reinterpret_cast<const char*>(tempVec.data()), tempVec.size());
    outfile.close();
    debugFileIdx++;
    */
    ////////////

    currentBufferIndexMutex.lock();
    
    currentBufferIndex++;
    currentBufferIndex %= 3;
    
    currentBufferIndexMutex.unlock();
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

    return 1;
}

void Receiver::bufferToDisplay(){
    currentBufferIndexMutex.lock();
    int buffIdx = currentBufferIndex;
    currentBufferIndexMutex.unlock();
    switch(buffIdx){
        case 0:
            //std::cout << buffer1.size() << std::endl;
            dis->vectorToTexture(&buffer1);
            break;
        case 1:
            //std::cout << buffer2.size() << std::endl;
            dis->vectorToTexture(&buffer2);
            break;
        case 2:
            //std::cout << buffer3.size() << std::endl;
            dis->vectorToTexture(&buffer3);
            break;
        default:
            std::cout << "error 86" << std::endl;
    }
}

void Receiver::initTextures(){
    dis->vectorToTexture(&buffer_initial);
    dis->vectorToTexture(&buffer_initial);
    dis->vectorToTexture(&buffer_initial);
    dis->vectorToTexture(&buffer_initial);    
}

int Receiver::findSequence(unsigned char (*str)[CDC_FRAME_SIZE], unsigned char ch1, unsigned char ch2){
    if(sequenceEndedFF){
        sequenceEndedFF = 0;
        if((*str)[0] == ch2){
            return 0;
        }
    }
    for(int i=0; i<CDC_FRAME_SIZE; i++){
        if((*str)[i] == ch1 && i < CDC_FRAME_SIZE){
            if((*str)[i+1] == ch2){
                return i;
            }
        }
    }

    if((*str)[49] == 0xFF) sequenceEndedFF = 1;

    return -1;
}

int Receiver::findStart(unsigned char (*str)[CDC_FRAME_SIZE]){
    int res = findSequence(str, 0xFF, 0xD8);
    return res;
}

int Receiver::findEnd(unsigned char (*str)[CDC_FRAME_SIZE]){
    int res = findSequence(str, 0xFF, 0xD9);
    return res;
}
