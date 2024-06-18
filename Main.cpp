#include "Map.cpp"

int main(int, char**) {
    unique_ptr<SDL_Window, SDL_Destroyer> window;
    unique_ptr<SDL_Renderer, SDL_Destroyer> renderer;
    SDL_Setup(window, renderer);
    return 0;
}
