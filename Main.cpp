#include "SDL.h"
#include "SDL_render.h"

int main(int, char**) {
    SDL_CreateWindow("Application", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 500, static_cast<SDL_WindowFlags>(0));
    return 0;
}
