#include "ImageView.hpp"

int createWindow(SDL_Window** windowPtr){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

     *windowPtr = window;

    return 0;
}
int renderWindow(SDL_Renderer** rendererPtr, SDL_Window* window){
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    *rendererPtr = renderer;
    return 0;
}

int imageFromVector(SDL_RWops** imageRwopsPtr, std::vector<uint8_t>* img){
        SDL_RWops* imageRwops = SDL_RWFromMem(img->data(), img->size());
        
        *imageRwopsPtr = imageRwops;
        return 0;
}

int createImageSurface(SDL_Surface** imageSurfacePtr, SDL_RWops* imageRwops){
    SDL_Surface* imageSurface = IMG_Load_RW(imageRwops, 0);
    if (!imageSurface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load image: %s", IMG_GetError());
        //SDL_DestroyRenderer(renderer);
        //SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    *imageSurfacePtr = imageSurface;
    return 0;
}

int textureFromSurface(SDL_Texture** texturePtr, SDL_Renderer** renderer, SDL_Surface** imageSurface){
    SDL_Texture* texture = SDL_CreateTextureFromSurface(*renderer, *imageSurface);
    SDL_FreeSurface(*imageSurface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture: %s", SDL_GetError());
        //SDL_DestroyRenderer(renderer);
        //SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    *texturePtr = texture;
    return 0;
}

int windowDestroy(SDL_Window** windowPtr, SDL_Texture** texturePtr, SDL_Renderer** rendererPtr){
    SDL_DestroyTexture(*texturePtr);
    SDL_DestroyRenderer(*rendererPtr);
    SDL_DestroyWindow(*windowPtr);
    SDL_Quit();
    return 0;
}


int windowLoop(SDL_Renderer** rendererPtr, 
                int* currentBufferIndex,
                std::mutex& currentBufferIndexMutex,
                SDL_Texture** texturePtr1, 
                SDL_Texture** texturePtr2, 
                SDL_Texture** texturePtr3){
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_RenderClear(*rendererPtr);
        currentBufferIndexMutex.lock();
        switch(*currentBufferIndex) {
            case 0:
                SDL_RenderCopy(*rendererPtr, *texturePtr1, NULL, NULL);
                break;
            case 1:
                SDL_RenderCopy(*rendererPtr, *texturePtr2, NULL, NULL);
                break;
            case 2:
                SDL_RenderCopy(*rendererPtr, *texturePtr3, NULL, NULL);
                break;
            default:
                std::cout << "error 18" << std::endl;
                break;
        }   
        currentBufferIndexMutex.unlock();

        SDL_RenderPresent(*rendererPtr);
    }

    return 0;
}

int vectorToTexture(SDL_Renderer** rendererPtr,
                    SDL_RWops** imageRwopsPtr, 
                    SDL_Surface** imageSurfacePtr, 
                    SDL_Texture** texturePtr,
                    std::vector<uint8_t>* img){
        
        imageFromVector(imageRwopsPtr, img);
        createImageSurface(imageSurfacePtr, *imageRwopsPtr);
        textureFromSurface(texturePtr, rendererPtr, imageSurfacePtr);
        
        return 0;
}

int flipThroughTextures(int * currentTexture, std::mutex& currentTextureMutex){
    while(1){
        currentTextureMutex.lock();
        *currentTexture = *currentTexture + 1;
        *currentTexture %=3;
        currentTextureMutex.unlock();
    }
}