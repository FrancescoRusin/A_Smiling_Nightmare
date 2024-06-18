#include "SDL.h"
#include "SDL_render.h"
#include "SDL_image.h"
#include "Entity.h"

#ifndef SS_GAME_MAP_H

class Map_handler {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *wall_texture;
    SDL_Texture *empty_box_texture;
    vector<vector<bool>> room;
    vector<vector<Entity>> entities;

public:
    Map_handler() {
        SDL_Init(0);
        window = SDL_CreateWindow("Application", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  500, 500, static_cast<SDL_WindowFlags>(0));
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    }

    ~Map_handler() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
    }

    void set_wall(const string &path);

    void set_empty_box(const string &path);

    void room_change_animation(const vector<vector<bool>> &new_room, double t);

    void bossfight_animation(double t);

    void render_map(int black);
};

#define SS_GAME_MAP_H

#endif
