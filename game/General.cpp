#include "General.h"

void General_handler::room_change_animation(const vector<vector<bool>> &new_room, const vector<Entity> &new_room_entities, Slide_direction direction, double t) {
    assert(new_room.size() == 10 && all_of(new_room.begin(), new_room.end(), [](const vector<bool> &v) { return v.size() == 10; }));
    if (room.empty()) {
        room = new_room;
        entities = new_room_entities;
        const double alpha_tick = 255 / (60 * t);
        SDL_SetTextureBlendMode(sprite_map[WALL], SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(sprite_map[EMPTY_BOX], SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(sprite_map[PROTAGONIST], SDL_BLENDMODE_BLEND);
        for (const Entity &e : entities) {
            SDL_SetTextureBlendMode(sprite_map[e.type], SDL_BLENDMODE_BLEND);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (double i = 0; i <= 255; i += alpha_tick) {
            SDL_SetTextureAlphaMod(sprite_map[WALL], i);
            SDL_SetTextureAlphaMod(sprite_map[EMPTY_BOX], i);
            SDL_SetTextureAlphaMod(sprite_map[PROTAGONIST], i);
            for (const Entity &e : entities) {
                SDL_SetTextureAlphaMod(sprite_map[e.type], i);
            }
            base_render();
        }
        SDL_SetTextureAlphaMod(sprite_map[WALL], 255);
        SDL_SetTextureAlphaMod(sprite_map[EMPTY_BOX], 255);
        SDL_SetTextureAlphaMod(sprite_map[PROTAGONIST], 255);
        for (const Entity &e : entities) {
            SDL_SetTextureAlphaMod(sprite_map[e.type], 255);
        }
        return;
    }
    const double slide_tick = 500 / (60 * t);
    for (double i = 0; i < 500; i += slide_tick) {
        shifted_render(new_room, new_room_entities, i, direction);
    }
    room = new_room;
    entities = new_room_entities;
}

void General_handler::base_render() noexcept {
    SDL_RenderClear(renderer);
    static auto map_crop = SDL_Rect(0, 0, 50, 50);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            map_crop.x = 50 * i;
            map_crop.y = 50 * j;
            SDL_RenderCopy(renderer, sprite_map[room[i][j] ? WALL : EMPTY_BOX], nullptr, &map_crop);
        }
    }
    protagonist.render(renderer, sprite_map[PROTAGONIST]);
    for (const Entity &e : entities) {
        e.render(renderer, sprite_map[e.type]);
    }
    SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - lastTick), 0.0));
    SDL_RenderPresent(renderer);
    lastTick = SDL_GetTicks();
}

void General_handler::shifted_render(const vector<vector<bool>> &new_room, const vector<Entity> &new_room_entities, const int shift, const Slide_direction direction) noexcept {
    SDL_RenderClear(renderer);
    static auto map_crop = SDL_Rect(0, 0, 50, 50);
    static int new_map_crop[2];
    static int tick[2];
    switch (direction) {
        case UP:
            tick[0] = 0;
            tick[1] = shift;
            new_map_crop[0] = 0;
            new_map_crop[1] = -500;
            break;
        case DOWN:
            tick[0] = 0;
            tick[1] = -shift;
            new_map_crop[0] = 0;
            new_map_crop[1] = 500;
            break;
        case RIGHT:
            tick[0] = -shift;
            tick[1] = 0;
            new_map_crop[0] = 500;
            new_map_crop[1] = 0;
            break;
        case LEFT:
            tick[0] = shift;
            tick[1] = 0;
            new_map_crop[0] = -500;
            new_map_crop[1] = 0;
            break;
    }
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            map_crop.x = 50 * i + tick[0];
            map_crop.y = 50 * j + tick[1];
            SDL_RenderCopy(renderer, sprite_map[room[i][j] ? WALL : EMPTY_BOX], nullptr, &map_crop);
            map_crop.x = 50 * i + new_map_crop[0] + tick[0];
            map_crop.y = 50 * j + new_map_crop[1] + tick[1];
            SDL_RenderCopy(renderer, sprite_map[new_room[i][j] ? WALL : EMPTY_BOX], nullptr, &map_crop);
        }
    }
    Entity protagonist_copy = Entity(protagonist);
    protagonist_copy.position[0] += tick[0];
    protagonist_copy.position[1] += tick[1];
    protagonist_copy.render(renderer, sprite_map[PROTAGONIST]);
    for (Entity e : entities) {
        e.position[0] += tick[0];
        e.position[1] += tick[1];
        e.render(renderer, sprite_map[e.type]);
    }
    for (Entity e : new_room_entities) {
        e.position[0] += tick[0];
        e.position[1] += tick[1];
        e.render(renderer, sprite_map[e.type]);
    }
    SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - lastTick), 0.0));
    SDL_RenderPresent(renderer);
    lastTick = SDL_GetTicks();
}

bool General_handler::poll_events_and_update_positions() noexcept {
    static SDL_Event event;
    Slide_direction shoot = NONE;
    bool swing = false;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        swing = true;
                        break;
                }
        }
    }
    const Uint8 *keyboard_status = SDL_GetKeyboardState(nullptr);
    if (keyboard_status[SDL_SCANCODE_W]) {
        protagonist.velocity[1] = -MAX_PROTAGONIST_SPEED;
    }
    if (keyboard_status[SDL_SCANCODE_S]) {
        protagonist.velocity[1] = MAX_PROTAGONIST_SPEED;
    }
    if (keyboard_status[SDL_SCANCODE_D]) {
        protagonist.velocity[0] = MAX_PROTAGONIST_SPEED;
    }
    if (keyboard_status[SDL_SCANCODE_A]) {
        protagonist.velocity[0] = -MAX_PROTAGONIST_SPEED;
    }
    if (keyboard_status[SDL_SCANCODE_LEFT]) {
        shoot = LEFT;
    }
    if (keyboard_status[SDL_SCANCODE_RIGHT]) {
        shoot = RIGHT;
    }
    if (keyboard_status[SDL_SCANCODE_UP]) {
        shoot = UP;
    }
    if (keyboard_status[SDL_SCANCODE_DOWN]) {
        shoot = DOWN;
    }
    if (swing) {
        cout << "swing!\n";
    } else if (shoot != NONE) {
        cout << "shoot!\n";
    }
    protagonist.position[0] += protagonist.velocity[0];
    protagonist.position[1] += protagonist.velocity[1];
    protagonist.velocity[0] = protagonist.velocity[1] = 0;
    //check collision
    for (int i = 0; i < entities.size(); ++i) {
        if (entities[i].hp <= 0) {
            entities.erase(next(entities.begin(), i));
            --i;
        }
    }
    return true;
}

void Entity::render(SDL_Renderer *renderer, SDL_Texture *texture) const noexcept {
    const auto mob_crop = SDL_Rect(position[0] - radius, position[1] - radius, 2 * radius, 2 * radius);
    SDL_RenderCopy(renderer, texture, nullptr, &mob_crop);
}