#include "SDL.h"
#include "SDL_render.h"
#include "SDL_image.h"
#include "bits/stdc++.h"

#ifndef SS_GAME_MAP_H

#define PROTAGONIST_SPEED 5;

using namespace std;

enum Slide_direction {
    UP, DOWN, LEFT, RIGHT, NONE
};

enum Sprite_type {
    WALL, EMPTY_BOX, CLOWN, KARATEKA, PROTAGONIST, PROTAGONIST_SHOT, ENEMY_SHOT
};

struct Entity {
    vector<int> position{0, 0};
    vector<int> velocity{0, 0};
    int radius{};
    int hp{};
    int id{};
    int8_t hit_tick{};
    int8_t action_tick{};
    Sprite_type type;

    Entity() {}

    Entity(const vector<int> &position, int radius, int hp, int id, Sprite_type type) {
        this->position[0] = position[0];
        this->position[1] = position[1];
        this->velocity[0] = this->velocity[1] = 0;
        this->radius = radius;
        this->hp = hp;
        this->id = id;
        this->hit_tick = this->action_tick = 0;
        this->type = type;
    }

    void render(SDL_Renderer *renderer, SDL_Texture *texture) const noexcept;
};

struct Enemy : public Entity {
    double movement_average{};
    double movement_control{};
    double special_action_probability{};

    Enemy() : Entity() {}

    Enemy(const vector<int> &position, int radius, int hp, int id, Sprite_type type,
          double movement_average, double movement_control, double special_action_probability) : Entity(position, radius, hp, id, type) {
        this->movement_average = movement_average;
        this->movement_control = movement_control;
        this->special_action_probability = special_action_probability;
    }
};

class General_handler {
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    vector<vector<bool>> room;
    map<Sprite_type, SDL_Texture *> sprite_map{};
    map<int, int> karateka_kick_animation{};
    map<int, vector<int>> karateka_kick_velocity{};
    Entity protagonist;
    vector<Enemy> enemies{};
    vector<Entity> enemy_shots{};
    vector<Entity> protagonist_shots{};
    mt19937_64 rng;
    normal_distribution<double> gauss = normal_distribution<double>();
    uniform_real_distribution<double> uniform_real = uniform_real_distribution<double>(0, 1);
    uniform_int_distribution<int> coin_flip = uniform_int_distribution<int>(0, 1);

    Uint32 framerate_last_tick = 0;
    int id_counter = 0;


    void shifted_render(const vector<vector<bool>> &new_room, const vector<Enemy> &new_room_enemies, int shift, Slide_direction direction) noexcept;
public:
    General_handler() {
        rng = mt19937_64(duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
        SDL_Init(SDL_INIT_VIDEO);
        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
        SDL_CreateWindowAndRenderer(500, 500, 0, &window, &renderer);
        sprite_map[WALL] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Brick_wall.jpg)");
        sprite_map[EMPTY_BOX] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Yellow_texture.jpg)");
        sprite_map[CLOWN] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Clown.png)");
        sprite_map[KARATEKA] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Blackbelt.png)");
        sprite_map[PROTAGONIST] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Smile.png)");
        sprite_map[PROTAGONIST_SHOT] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Orange_ball.png)");
        sprite_map[ENEMY_SHOT] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Blue_ball.png)");
        protagonist = Entity(vector<int>{250, 250}, 20, 20, 0, PROTAGONIST);
        enemies = {Enemy(vector<int>{100, 100}, 20, 20, ++id_counter, KARATEKA, 4, 1, 1 / 120.0), Enemy(vector<int>{100, 400}, 20, 20, ++id_counter, CLOWN, 1, .33, .01)};
    }

    ~General_handler() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        for (const auto &[type, texture] : sprite_map) {
            SDL_DestroyTexture(texture);
        }
    }

    void room_change_animation(const vector<vector<bool>> &new_room, const vector<Enemy> &new_room_enemies, Slide_direction);

    void base_render() noexcept;

    bool poll_events_and_update_positions() noexcept;

    bool avoid_wall_collision(Entity &entity);

    bool collide(const map<int, vector<int>> &previous_positions, const Entity &entity1, const Entity &entity2);
};

double line_point_distance(int *line, const vector<int> &point);
vector<double> line_line_intersection(int *line1, int *line2);
double point_point_distance(const vector<int> &p1, const vector<int> &p2);

#define SS_GAME_MAP_H

#endif
