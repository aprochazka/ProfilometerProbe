#include "main.hpp"
#include "Displayer.hpp"


int Displayer::createWindow() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    return 0;
}

int Displayer::renderWindow(){
    renderer = SDL_CreateRenderer(window, -1, 0);
    
    if (!renderer) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    return 0;
}

int Displayer::imageFromVector(std::vector<uint8_t>* img){
    imageRwops = SDL_RWFromMem(img->data(), img->size());

    return 0;
}

int Displayer::createImageSurface(){
    imageSurface = IMG_Load_RW(imageRwops, 0);
    if (!imageSurface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to load image: %s", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    return 0;
}

int Displayer::textureFromSurface(){
    
    currentTextureIndexMutex.lock();

    switch(currentTextureIndex){
        case 0:
            texture2 = SDL_CreateTextureFromSurface(renderer, imageSurface);
            break;
        case 1:
            texture3 = SDL_CreateTextureFromSurface(renderer, imageSurface);
            break;
        case 2:
            texture1 = SDL_CreateTextureFromSurface(renderer, imageSurface);
            break;
        default:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to asess currentTextureIndex: %s", SDL_GetError());
            return 1;
    }

    currentTextureIndex++;
    currentTextureIndex %= 3;

    currentTextureIndexMutex.unlock();
    SDL_FreeSurface(imageSurface);
    
    if (!texture1 || !texture2 || !texture3) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create texture: %s", SDL_GetError());
        return 1;
    }

    return 0;
}

int Displayer::windowDestroy(){
    SDL_DestroyTexture(texture1);
    SDL_DestroyTexture(texture2);
    SDL_DestroyTexture(texture3);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int Displayer::windowLoop(){
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_RenderClear(renderer);
        
        currentTextureIndexMutex.lock();
        
        switch(currentTextureIndex) {
            case 0:
                SDL_RenderCopy(renderer, texture1, NULL, NULL);
                break;
            case 1:
                SDL_RenderCopy(renderer, texture2, NULL, NULL);
                break;
            case 2:
                SDL_RenderCopy(renderer, texture3, NULL, NULL);
                break;
            default:
                std::cout << "error 18" << std::endl;
                break;
        }   
        SDL_RenderPresent(renderer);

        currentTextureIndexMutex.unlock();

        
    }

    return 0;
}

int Displayer::vectorToTexture(std::vector<uint8_t>* img){
        
        imageFromVector(img);
        createImageSurface();
        textureFromSurface();
        
        return 0;
}

int Displayer::flipThroughTextures(){
    int flipIdx = 0;
    while(1){
        if(flipIdx){
            vectorToTexture(&t1);
        }
        else{
            vectorToTexture(&t2);
        }
        flipIdx++;
        flipIdx%=2;
    }
}

