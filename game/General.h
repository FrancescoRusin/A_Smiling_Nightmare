#include "SDL.h"
#include "SDL_render.h"
#include "SDL_image.h"
#include "bits/stdc++.h"

#ifndef SS_GAME_MAP_H

#define MAX_PROTAGONIST_SPEED 4;

using namespace std;

enum Slide_direction {
    UP, DOWN, LEFT, RIGHT, NONE
};

enum Sprite_type {
    WALL, EMPTY_BOX, CLOWN, KARATEKA, PROTAGONIST
};

struct Entity {
    vector<int> position;
    vector<int> velocity;
    int radius;
    int hp;
    Sprite_type type;

    void render(SDL_Renderer *renderer, SDL_Texture *texture) const noexcept;
};

class General_handler {
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    vector<vector<bool>> room;
    map<Sprite_type, SDL_Texture *> sprite_map{};
    Entity protagonist;
    vector<Entity> entities{};

    Uint32 lastTick = 0;

    void shifted_render(const vector<vector<bool>> &new_room, const vector<Entity> &new_room_entities, int shift, Slide_direction direction) noexcept;
public:
    General_handler() {
        SDL_Init(SDL_INIT_VIDEO);
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
        SDL_CreateWindowAndRenderer(500, 500, 0, &window, &renderer);
        sprite_map[WALL] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Brick_wall.jpg)");
        sprite_map[EMPTY_BOX] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Yellow_texture.jpg)");
        sprite_map[CLOWN] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Clown.png)");
        sprite_map[KARATEKA] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Blackbelt.png)");
        sprite_map[PROTAGONIST] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Smile.png)");
        protagonist = Entity(vector<int>{250, 250}, vector<int>{0, 0}, 20, PROTAGONIST);
    }

    ~General_handler() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        for (int t = 0; t <= PROTAGONIST; ++t) {
            SDL_DestroyTexture(sprite_map[static_cast<Sprite_type>(t)]);
        }
    }

    void room_change_animation(const vector<vector<bool>> &new_room, const vector<Entity> &new_room_entities, Slide_direction, double t);

    void base_render() noexcept;

    bool poll_events_and_update_positions() noexcept;
};

#define SS_GAME_MAP_H

#endif
