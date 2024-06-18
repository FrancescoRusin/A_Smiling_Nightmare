#include "SDL.h"
#include "SDL_render.h"
#include "SDL_image.h"

#ifndef SS_GAME_ENTITY_H
#define SS_GAME_ENTITY_H
using namespace std;


class Entity {
    enum Type {
        PROTAGONIST, ENEMY, STATIC
    };
    const Type type;
    vector<double> position;
    void render(SDL_Renderer *renderer);
};


#endif //SS_GAME_ENTITY_H
