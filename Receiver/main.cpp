#include "main.hpp"
#include "Receiver.hpp"

#ifndef DISPLAYER_HPP
#include "Displayer.hpp"
#endif

#include "Transfer.hpp"

        int cdcFile;
        struct termios tty;

void readLoop(Receiver ** receiverPtr){
    (*receiverPtr)->openStream();
    (*receiverPtr)->initSerial();

    while (true) {
        unsigned char character[10];
        (*receiverPtr)->readCdcData(&character);
        if((*receiverPtr)->findStart(&character) != -1)
        {std::cout << "start" << std::endl;}
        else if((*receiverPtr)->findEnd(&character) != -1)
        {std::cout << "stop" << std::endl;}
        else{
          std::cout << "-";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    close(cdcFile);
}


void windowFlipThread(Displayer * displayerPtr){
  displayerPtr->flipThroughTextures();
}

void receiverLoop(Receiver ** receiverPtr){
  while(1){
    (*receiverPtr)->fillBuffer();
    (*receiverPtr)->bufferToDisplay();
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
  }
}

int main()
{
  Displayer dis;
  Receiver *rec = new Receiver(&dis);

  /*
  dis.createWindow();
  dis.renderWindow();

  rec->openStream();
  rec->initSerial();
  rec->fillBuffer();
  rec->bufferToDisplay();

  std::thread t1(receiverLoop, &rec);
  
  dis.windowLoop();

  t1.join();
  */

  readLoop(&rec);
  return 0;
}