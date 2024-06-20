#include "SDL.h"
#include "SDL_render.h"
#include "SDL_image.h"
#include "bits/stdc++.h"

#ifndef SS_GAME_MAP_H

#define PROTAGONIST_SPEED 8;

using namespace std;

enum Direction {
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

    Entity() : type(EMPTY_BOX) {};

    Entity(const Entity &entity) = default;

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

    Entity& operator=(const Entity &entity) {
        this->position[0] = entity.position[0];
        this->position[1] = entity.position[1];
        this->velocity[0] = entity.velocity[0];
        this->velocity[1] = entity.velocity[1];
        this->radius = entity.radius;
        this->hp = entity.hp;
        this->id = entity.id;
        this->hit_tick = entity.hit_tick;
        this->action_tick = entity.action_tick;
        this->type = entity.type;
        return *this;
    }

    void render(SDL_Renderer *renderer, SDL_Texture *texture) const noexcept;
};

struct Enemy : public Entity {
    double movement_average{};
    double movement_control{};

    Enemy() : Entity() {}

    Enemy(const vector<int> &position, int radius, int hp, int id, Sprite_type type,
          double movement_average, double movement_control) : Entity(position, radius, hp, id, type) {
        this->movement_average = movement_average;
        this->movement_control = movement_control;
    }
};

struct Game_stats {
    int karateka_kick_speed;
    int karateka_kick_delay;
    double karateka_kick_probability;
    double clown_shoot_probability;
    int clown_shoot_speed;
    double clown_shot_precision;
};

class General_handler {
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    vector<vector<bool>> room;
    map<Sprite_type, SDL_Texture *> sprite_map{};
    map<int, int> karateka_kick_animation{};
    vector<Entity> enemy_shots{};
    vector<Entity> protagonist_shots{};
    int protagonist_swing{};
    vector<int> protagonist_swing_direction{};
    SDL_Texture *swing_texture{};

    mt19937_64 rng;
    normal_distribution<double> gauss = normal_distribution<double>();
    uniform_real_distribution<double> uniform_real = uniform_real_distribution<double>(0, 1);
    uniform_int_distribution<int> coin_flip = uniform_int_distribution<int>(0, 1);

    Uint32 framerate_last_tick = 0;

    Game_stats game_stats{};

    void shifted_render(const vector<vector<bool>> &new_room, int shift, Direction direction) noexcept;

    bool avoid_wall_collision(Entity &entity);

    bool collide(const map<int, vector<int>> &previous_positions, const Entity &entity1, const Entity &entity2);
public:
    int id_counter = 0;
    Entity protagonist;
    vector<Enemy> enemies{};
    General_handler() = default;

    ~General_handler() {
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        for (const auto &[type, texture] : sprite_map) {
            SDL_DestroyTexture(texture);
        }
    }

    void initialize();

    void room_change_animation(const vector<vector<bool>> &new_room, const vector<Enemy> &new_room_enemies, Direction);

    void base_render() noexcept;

    bool poll_events_and_update_positions() noexcept;
};

double line_point_distance(int *line, const vector<int> &point);
vector<double> line_line_intersection(int *line1, int *line2);
double point_point_distance(const vector<int> &p1, const vector<int> &p2);

#define SS_GAME_MAP_H

#endif
