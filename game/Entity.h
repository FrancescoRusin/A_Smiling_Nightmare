#include "SDL.h"
#include "SDL_render.h"
#include "SDL_image.h"


#ifndef SS_GAME_ENTITY_H
#define SS_GAME_ENTITY_H

using namespace std;

class Entity {
    enum Type {
        PROTAGONIST, ENEMY, STATIC, ENEMY_SHOT, PROTAGONIST_SHOT
    };
    Type type;

public:
    Entity() = default;
    Entity(Type type, const int *position, const int *velocity, const int radius, SDL_Texture *texture) : type(type), texture(texture) {
        this->position[0] = position[0];
        this->position[1] = position[1];
        this->velocity[0] = velocity[0];
        this->velocity[1] = velocity[1];
        this->radius = radius;
    };
    int position[2]{};
    int velocity[2]{};
    int radius;
    SDL_Texture *texture;

    void set_texture(SDL_Texture *texture) {
        this->texture = texture;
    }

    void render(SDL_Renderer *renderer) const noexcept;
};


#endif //SS_GAME_ENTITY_H
