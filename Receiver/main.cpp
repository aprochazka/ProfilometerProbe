#include "main.hpp"
#include "Receiver.hpp"

#ifndef DISPLAYER_HPP
#include "Displayer.hpp"
#endif

#include "Transfer.hpp"

        int cdcFile;
        struct termios tty;
void readLoop(){
    openStream(&cdcFile);
    initSerial(&tty, &cdcFile);

    while (true) {
        readCdcData(&cdcFile);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
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
  dis.createWindow();
  dis.renderWindow();

  Receiver *rec = new Receiver(&dis);
  rec->openStream();
  rec->initSerial();
  rec->fillBuffer();
  rec->bufferToDisplay();

  std::thread t1(receiverLoop, &rec);
  
  dis.windowLoop();

  t1.join();


  //readLoop();
  return 0;
}
