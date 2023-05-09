/**
 * @file main.hpp
 * @brief Header file for the main application.
 * @author Adam Prochazka <xproch0f>
 *
 * This file contains the includes, definitions, and declarations required for the main application.
 */

#include <iostream>
#include <cstring>
#include <cstdint>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <map>
#include <iomanip>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <thread>
#include <chrono>
#include <mutex>
#include <fstream>

#define CDC_FRAME_SIZE 64
#define CDC_FRAME_DELAY 1
