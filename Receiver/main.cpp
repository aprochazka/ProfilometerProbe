/**
 * @file main.cpp
 * @brief Source file for the main application.
 * @author Adam Prochazka <xproch0f>
 *
 * This file contains the implementation of the main application, which integrates the Receiver
 * and Displayer classes, manages their threads, and handles the overall execution flow.
 */

#include "main.hpp"
#include "Receiver.hpp"

#ifndef DISPLAYER_HPP
#include "Displayer.hpp"
#endif

int cdcFile;
bool quit = false;
/**
* @brief Reads and processes data from the serial communication in a loop. - for debug purposes
* @param receiverPtr Pointer to a pointer to the Receiver class instance.
* @param port Serial port to be opened.
*/
void readLoop(Receiver ** receiverPtr, char * port){
    (*receiverPtr)->openStream(port);
    (*receiverPtr)->initSerial();

    while (!quit) {
        unsigned char character[CDC_FRAME_SIZE];
        (*receiverPtr)->readCdcData(&character);
        if((*receiverPtr)->findStart(&character) != -1)
        {std::cout << "start" << std::endl;}
        else if((*receiverPtr)->findEnd(&character) != -1)
        {std::cout << "stop" << std::endl;}
        else{
          std::cout << "-"; 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }

    close(cdcFile);
}

/**
* @brief Flips through the textures in a separate thread.
* @param displayerPtr Pointer to the Displayer class instance.
*/
void windowFlipThread(Displayer * displayerPtr){
  displayerPtr->flipThroughTextures();
}

/**
* @brief Wrapper function to call bufferToDisplay() method of the Receiver instance.
* @param receiverPtr Pointer to a pointer to the Receiver instance.
*/
void WrapperBufferToDisplay(Receiver ** receiverPtr){
  (*receiverPtr)->bufferToDisplay();
}

/**
 * @brief Main loop for the Receiver instance.
 * @param receiverPtr Pointer to a pointer to the Receiver instance.
 */
void receiverLoop(Receiver ** receiverPtr, bool *q, bool printRaw, bool saveToFile){
  std::vector<std::thread> displayThreads;
  while(!(*q)){

    (*receiverPtr)->fillBuffer(saveToFile);
    (*receiverPtr)->bufferToDisplay(printRaw);

    //running in a thread also works, but for simplicity is not used. 
    //std::thread t2(WrapperBufferToDisplay, receiverPtr);
    //t2.detach();
  }
}

int main(int argc, char * argv[])
{
  
  char * port = nullptr;
  bool printRaw = false;
  bool saveToFile = false;

  int opt;
  while ((opt = getopt(argc, argv, "p:r:s")) != -1) {
      switch (opt) {
          case 'p':
              port = optarg;
              break;
          case 'r':
              printRaw = true;
              break;
          case 's':
              saveToFile = true;
              break;
          default:
              std::cerr << "Usage: sudo ./main -p <serial_port> [-r to print raw data] [-s to save images]" << std::endl;
              std::cerr << "Example: sudo ./main -p /dev/ttyACM0 -r -s" << std::endl;
              return 1;
      }
  }

  if (!port) {
      std::cerr << "Serial port not specified" << std::endl;
      return 1;
  }

  std::cout << "Serial port: " << port << std::endl;
  std::cout << "Print raw data: " << (printRaw ? "true" : "false") << std::endl;
  std::cout << "Save images: " << (saveToFile ? "true" : "false") << std::endl;

  Displayer dis;
  Receiver *rec = new Receiver(&dis);

  
  dis.createWindow();
  dis.renderWindow();

  rec->openStream(port);
  rec->initSerial();
  
  rec->initTextures();

  rec->fillBuffer();
  rec->bufferToDisplay();

  std::thread t1(receiverLoop, &rec, &quit, std::ref(printRaw), std::ref(saveToFile));
  t1.detach();

  dis.windowLoop(&quit);

  rec->closeStream();

  return 0;
}
