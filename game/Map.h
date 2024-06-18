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
    Map_handler() = default;

    ~Map_handler() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
    }

    void setup() noexcept;

    void set_wall(const string &path);

    void set_empty_box(const string &path);

    void room_change_animation(const vector<vector<bool>> &new_room, double t);

    void render_map(int black);
};

#define SS_GAME_MAP_H

#endif
