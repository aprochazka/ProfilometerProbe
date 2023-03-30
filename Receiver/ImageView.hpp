#include "main.hpp"

int createWindow(SDL_Window** windowPtr);
int renderWindow(SDL_Renderer** rendererPtr, SDL_Window* window);
int imageFromVector(SDL_RWops** imageRwopsPtr, std::vector<uint8_t>* img);
int createImageSurface(SDL_Surface** imageSurfacePtr, SDL_RWops* imageRwops);
int textureFromSurface(SDL_Texture** texturePtr, SDL_Renderer** renderer, SDL_Surface** imageSurface);

int windowDestroy(SDL_Window** windowPtr, SDL_Texture** texturePtr, SDL_Renderer** rendererPtr);
int windowLoop( SDL_Renderer** rendererPtr, 
                int* currentBufferIndex, 
                std::mutex& currentBufferIndexMutex,
                SDL_Texture** texturePtr1, 
                SDL_Texture** texturePtr2, 
                SDL_Texture** texturePtr3);

int vectorToTexture(SDL_Renderer** rendererPtr,
                    SDL_RWops** imageRwopsPtr, 
                    SDL_Surface** imageSurfacePtr, 
                    SDL_Texture** texturePtr,
                    std::vector<uint8_t>* img);

int flipThroughTextures(int * currentTexture, std::mutex& currentTextureMutex);