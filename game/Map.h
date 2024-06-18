#include "SDL.h"
#include "SDL_render.h"
#include "SDL_image.h"
#include "Entity.h"
#include "bits/stdc++.h"

#ifndef SS_GAME_MAP_H

enum Slide_direction {
    UP, DOWN, LEFT, RIGHT
};

class View_handler {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *wall_texture;
    SDL_Texture *empty_box_texture;
    vector<vector<bool>> room;
    vector<Entity> entities;
    Entity protagonist;

    void shifted_render(const vector<vector<bool>> &new_room, const vector<Entity> &new_room_entities, const int shift, const Slide_direction direction) const noexcept;
public:
    View_handler() {
        SDL_Init(0);
        window = SDL_CreateWindow("Application", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  500, 500, static_cast<SDL_WindowFlags>(0));
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    }

    ~View_handler() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyTexture(wall_texture);
        SDL_DestroyTexture(empty_box_texture);
    }

    void set_wall(const string &path);

    void set_empty_box(const string &path);

    void room_change_animation(const vector<vector<bool>> &new_room, const vector<Entity> &new_room_entities, Slide_direction, double t);

    void bossfight_animation(double t);

    void base_render() const noexcept;
};

#define SS_GAME_MAP_H

#endif
