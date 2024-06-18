#include "bits/stdc++.h"
#include "Map.h"
#include "Entity.h"

void Map_handler::set_wall(const string& path) {
    wall_texture = IMG_LoadTexture(renderer, path.c_str());
}

void Map_handler::set_empty_box(const string& path) {
    empty_box_texture = IMG_LoadTexture(renderer, path.c_str());
}

void Map_handler::room_change_animation(const vector<vector<bool>> &new_room, double t) {
    assert(new_room.size() == 10 && all_of(new_room.begin(), new_room.end(), [](const vector<bool> &v) { return v.size() != 10; }));
    if (room.empty()) {
        room = new_room;
    }
    const int alpha_tick = max(1, static_cast<int>(255 / (60 * t)));
    for (int i = 0; i <= 255; i += alpha_tick) {
        render_map(i);
    }
}

void Map_handler::render_map(int black) {
    SDL_SetTextureAlphaMod(wall_texture, black);
    SDL_SetTextureAlphaMod(empty_box_texture, black);
    static auto base_crop = SDL_Rect(0, 0, 50, 50);
    static auto new_crop = SDL_Rect(0, 0, 50, 50);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            new_crop.x = 50 * i;
            new_crop.y = 50 * j;
            SDL_RenderCopy(renderer, room[i][j] ? wall_texture : empty_box_texture, &base_crop, &new_crop);
        }
    }
}