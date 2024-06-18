#include "Map.h"
#include "Entity.h"

void View_handler::set_wall(const string& path) {
    wall_texture = IMG_LoadTexture(renderer, path.c_str());
}

void View_handler::set_empty_box(const string& path) {
    empty_box_texture = IMG_LoadTexture(renderer, path.c_str());
}

void View_handler::room_change_animation(const vector<vector<bool>> &new_room, const vector<Entity> &new_room_entities, Slide_direction direction, double t) {
    assert(new_room.size() == 10 && all_of(new_room.begin(), new_room.end(), [](const vector<bool> &v) { return v.size() == 10; }));
    if (room.empty()) {
        room = new_room;
        entities = new_room_entities;
        const double alpha_tick = 255 / (60 * t);
        SDL_SetTextureBlendMode(wall_texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(empty_box_texture, SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(protagonist.texture, SDL_BLENDMODE_BLEND);
        for (const Entity &e : entities) {
            SDL_SetTextureBlendMode(e.texture, SDL_BLENDMODE_BLEND);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (double i = 0; i <= 255; i += alpha_tick) {
            SDL_SetTextureAlphaMod(wall_texture, i);
            SDL_SetTextureAlphaMod(empty_box_texture, i);
            SDL_SetTextureAlphaMod(protagonist.texture, i);
            for (const Entity &e : entities) {
                SDL_SetTextureAlphaMod(e.texture, i);
            }
            base_render();
        }
        SDL_SetTextureAlphaMod(wall_texture, 255);
        SDL_SetTextureAlphaMod(empty_box_texture, 255);
        SDL_SetTextureAlphaMod(protagonist.texture, 255);
        for (const Entity &e : entities) {
            SDL_SetTextureAlphaMod(e.texture, 255);
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

void View_handler::base_render() const noexcept {
    SDL_RenderClear(renderer);
    static auto map_crop = SDL_Rect(0, 0, 50, 50);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            map_crop.x = 50 * i;
            map_crop.y = 50 * j;
            SDL_RenderCopy(renderer, room[i][j] ? wall_texture : empty_box_texture, nullptr, &map_crop);
        }
    }
    //protagonist.render(renderer);
    for (const Entity &e : entities) {
        e.render(renderer);
    }
    SDL_RenderPresent(renderer);
}

void View_handler::shifted_render(const vector<vector<bool>> &new_room, const vector<Entity> &new_room_entities, const int shift, const Slide_direction direction) const noexcept {
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
        case LEFT:
            tick[0] = -shift;
            tick[1] = 0;
            new_map_crop[0] = 500;
            new_map_crop[1] = 0;
            break;
        case RIGHT:
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
            SDL_RenderCopy(renderer, room[i][j] ? wall_texture : empty_box_texture, nullptr, &map_crop);
            map_crop.x = 50 * i + new_map_crop[0] + tick[0];
            map_crop.y = 50 * j + new_map_crop[1] + tick[1];
            SDL_RenderCopy(renderer, new_room[i][j] ? wall_texture : empty_box_texture, nullptr, &map_crop);
        }
    }
    map_crop.x = protagonist.position[0] + new_map_crop[0] + tick[0];
    map_crop.y = protagonist.position[1] + new_map_crop[1] + tick[1];
    //SDL_RenderCopy(renderer, protagonist.texture, nullptr, &map_crop);
    for (const Entity &e : entities) {
        map_crop.x = e.position[0] + tick[0];
        map_crop.y = e.position[1] + tick[1];
        SDL_RenderCopy(renderer, e.texture, nullptr, &map_crop);
    }
    for (const Entity &e : new_room_entities) {
        map_crop.x = e.position[0] + new_map_crop[0] + tick[0];
        map_crop.y = e.position[1] + new_map_crop[1] + tick[1];
        SDL_RenderCopy(renderer, e.texture, nullptr, &map_crop);
    }
    SDL_RenderPresent(renderer);
}