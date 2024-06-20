#include "Game_handler.h"

vector<int> find_neighbors(int r) {
    vector<int> neighbors{};
    if (r > 8) {
        neighbors.push_back(r - 8);
    }
    if (r < 56) {
        neighbors.push_back(r + 8);
    }
    if (r % 8 != 0) {
        neighbors.push_back(r - 1);
    }
    if (r % 8 != 7) {
        neighbors.push_back(r + 1);
    }
    return neighbors;
}

void Game_handler::game() {
    mt19937_64 rng = mt19937_64(duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
    uniform_int_distribution<int> uniform4 = uniform_int_distribution<int>(0, 4);
    uniform_real_distribution<double> uniform_unit = uniform_real_distribution<double>();
    handler.initialize();
    vector<int> queue(1, 36);
    vector<int> reachable(1, 36);
    while(reachable.size() < 8) {
        vector<int> queue_copy(queue);
        for (const int &r: queue_copy) {
            for (const int &n: find_neighbors(r)) {
                if (find(reachable.begin(), reachable.end(), n) == reachable.end()) {
                    vector<int> neighbor_neighbors = find_neighbors(n);
                    if (uniform_unit(rng) < .5 && reachable.size() < 8 &&
                    count_if(reachable.begin(), reachable.end(), [&](int nn) {
                        return find(neighbor_neighbors.begin(), neighbor_neighbors.end(), nn) != neighbor_neighbors.end();
                    }) < 3) {
                        reachable.push_back(n);
                        queue.push_back(n);
                    }
                }
            }
        }
    }
    vector<Direction> directions{};
    if (find(reachable.begin(), reachable.end(), 26) != reachable.end()) {
        directions.push_back(DOWN);
    }
    if (find(reachable.begin(), reachable.end(), 35) != reachable.end()) {
        directions.push_back(LEFT);
    }
    if (find(reachable.begin(), reachable.end(), 37) != reachable.end()) {
        directions.push_back(RIGHT);
    }
    if (find(reachable.begin(), reachable.end(), 46) != reachable.end()) {
        directions.push_back(UP);
    }
    vector<vector<bool>> room = build_room(directions);
    vector<Enemy> enemies = build_enemies();
    handler.room_change_animation(room, enemies, NONE);
    bool keep_open = true;
    while (keep_open) {
        while (handler.protagonist.hp > 0 && (!handler.enemies.empty() ||
                (abs(handler.protagonist.position[0] - 400) < 300 && abs(handler.protagonist.position[1] - 400) < 300))) {
            handler.base_render();
            keep_open = handler.poll_events_and_update_positions();
        }
        if (handler.protagonist.hp < 0) {
            handler.room_change_animation(vector<vector<bool>>(0), vector<Enemy>{}, NONE);
            keep_open = false;
        } else {
            Direction direction = NONE;
            if (handler.protagonist.position[0] > 700) {
                handler.protagonist.position[0] = 800 - handler.protagonist.position[0];
                direction = RIGHT;
            } else if (handler.protagonist.position[0] < 100) {
                handler.protagonist.position[0] = 800 - handler.protagonist.position[0];
                direction = LEFT;
            } else if (handler.protagonist.position[1] > 700) {
                handler.protagonist.position[1] = 800 - handler.protagonist.position[1];
                direction = DOWN;
            } else if (handler.protagonist.position[1] < 100) {
                handler.protagonist.position[1] = 800 - handler.protagonist.position[1];
                direction = UP;
            }
            handler.room_change_animation(build_room(vector<Direction>()), build_enemies(), direction);
        }
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