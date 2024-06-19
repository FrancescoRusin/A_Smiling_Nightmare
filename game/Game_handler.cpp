#include "Game_handler.h"

void Game_handler::game() {
    mt19937_64 rng = mt19937_64(duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
    uniform_int_distribution<int> uniform4 = uniform_int_distribution<int>(0, 4);
    vector<Direction> all_directions{UP, DOWN, LEFT, RIGHT};
    handler.initialize();
    vector<vector<vector<Direction>>> floor(3, vector<vector<Direction>>(3, vector<Direction>(0)));
    vector<vector<bool>> reachable(3, vector<bool>(3));
    floor[0][0] = vector<Direction>{UP, RIGHT};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            vector<Direction>
        }
    }
    vector<vector<bool>> room = build_room(vector<Direction>());
    vector<Enemy> enemies = build_enemies();
    handler.room_change_animation(room, enemies, NONE);
    bool keep_open = true;
    while (keep_open) {
        while (keep_open && !handler.enemies.empty()) {
            handler.base_render();
            keep_open = handler.poll_events_and_update_positions();
        }
        handler.room_change_animation(build_room(vector<Direction>()), build_enemies(), UP);
    }
}

vector<Enemy> Game_handler::build_enemies() {
    mt19937_64 rng = mt19937_64(duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
    uniform_int_distribution<int> uniform5 = uniform_int_distribution<int>(2, 5);
    uniform_int_distribution<int> uniform800 = uniform_int_distribution<int>(0, 799);
    vector<Enemy> enemies(0);
    for (int i = uniform5(rng); i >= 0; --i) {
        if (uniform800(rng) < 400) {
            enemies.emplace_back(vector<int>{uniform800(rng), uniform800(rng)}, 35, 20, ++(handler.id_counter), CLOWN, 0, .8);
        } else {
            enemies.emplace_back(vector<int>{uniform800(rng), uniform800(rng)}, 35, 20, ++(handler.id_counter), KARATEKA, 7, 2);
        }
    }
    return enemies;
}

vector<vector<bool>> Game_handler::build_room(const vector<Direction> &directions) {
    vector<vector<bool>> room(10, vector<bool>(10));
    room[0][0] = room[9][0] = room[0][9] = room[9][9] = true;
    return room;
}