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
    uniform_int_distribution<int> final_room_extractor = uniform_int_distribution<int>(0, 7);
    handler.initialize(true);
    int room_number = 36;
    vector<Direction> directions{};
    vector<int> floor = build_floor();
    int final_room_number = floor[final_room_extractor(rng)];
    while (final_room_number == 26 || final_room_number == 35 || final_room_number == 36 || final_room_number == 37 || final_room_number == 46) {
        final_room_number = floor[final_room_extractor(rng)];
    }
    if (find(floor.begin(), floor.end(), 26) == floor.end()) {
        directions.push_back(DOWN);
    }
    if (find(floor.begin(), floor.end(), 35) == floor.end()) {
        directions.push_back(LEFT);
    }
    if (find(floor.begin(), floor.end(), 37) == floor.end()) {
        directions.push_back(RIGHT);
    }
    if (find(floor.begin(), floor.end(), 46) == floor.end()) {
        directions.push_back(UP);
    }
    vector<vector<bool>> room = build_room(directions);
    vector<Enemy> enemies = build_enemies();
    handler.room_change_animation(room, enemies, NONE);
    bool keep_open = true;
    while (keep_open) {
        while (handler.protagonist.hp > 0 && (!handler.enemies.empty() ||
                (abs(handler.protagonist.position[0] - 400) < 350 && abs(handler.protagonist.position[1] - 400) < 350)) &&
                (!handler.enemies.empty() || room_number != final_room_number ||
                abs(handler.protagonist.position[0] - 400) > 25 || abs(handler.protagonist.position[1] - 400) > 25)) {
            handler.base_render();
            keep_open = handler.poll_events_and_update_positions();
        }
        if (handler.protagonist.hp < 0) {
            handler.room_change_animation(vector<vector<bool>>(0), vector<Enemy>{}, NONE);
            keep_open = false;
        } else {
            Direction direction = NONE;
            if (handler.protagonist.position[0] > 750) {
                handler.protagonist.position[0] = handler.protagonist.radius;
                direction = RIGHT;
                room_number -= 1;
            } else if (handler.protagonist.position[0] < 50) {
                handler.protagonist.position[0] = 800 - handler.protagonist.radius;
                direction = LEFT;
                room_number += 1;
            } else if (handler.protagonist.position[1] > 750) {
                handler.protagonist.position[1] = handler.protagonist.radius;
                direction = DOWN;
                room_number += 8;
            } else if (handler.protagonist.position[1] < 50) {
                handler.protagonist.position[1] = 800 - handler.protagonist.radius;
                direction = UP;
                room_number -= 8;
            }
            handler.final_room = (room_number == final_room_number);
            directions.clear();
            if (find(floor.begin(), floor.end(), room_number - 8) == floor.end()) {
                directions.push_back(UP);
            }
            if (room_number % 8 == 0 && find(floor.begin(), floor.end(), room_number - 1) == floor.end()) {
                directions.push_back(LEFT);
            }
            if (room_number % 8 == 7 && find(floor.begin(), floor.end(), room_number + 1) == floor.end()) {
                directions.push_back(RIGHT);
            }
            if (find(floor.begin(), floor.end(), room_number + 8) == floor.end()) {
                directions.push_back(DOWN);
            }
            handler.room_change_animation(build_room(directions), build_enemies(), direction);
        }
    }
}

vector<int> Game_handler::build_floor() {
    uniform_int_distribution<int> uniform4 = uniform_int_distribution<int>(0, 4);
    uniform_real_distribution<double> uniform_unit = uniform_real_distribution<double>();
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
    return reachable;

}

vector<Enemy> Game_handler::build_enemies() {
    mt19937_64 rng = mt19937_64(duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
    uniform_int_distribution<int> uniform5 = uniform_int_distribution<int>(2, 5);
    uniform_int_distribution<int> uniform800 = uniform_int_distribution<int>(0, 799);
    vector<Enemy> enemies(0);
    for (int i = uniform5(rng); i >= 0; --i) {
        if (uniform800(rng) < 400) {
            enemies.emplace_back(vector<int>{uniform800(rng), uniform800(rng)}, 25, 20, ++(handler.id_counter), CLOWN, 0, .8);
        } else {
            enemies.emplace_back(vector<int>{uniform800(rng), uniform800(rng)}, 25, 20, ++(handler.id_counter), KARATEKA, 7, 2);
        }
    }
    return enemies;
}

//TODO
vector<vector<bool>> Game_handler::build_room(const vector<Direction> &directions) {
    vector<vector<bool>> room(16, vector<bool>(16));
    for (const Direction &direction: directions) {
        switch (direction) {
            case UP:
                fill(room[0].begin(), room[0].end(), true);
                break;
            case DOWN:
                fill(room[15].begin(), room[15].end(), true);
                break;
            case LEFT:
                for (int i = 0; i < 16; ++i) {
                    room[0][i] = true;
                }
                break;
            case RIGHT:
                for (int i = 0; i < 16; ++i) {
                    room[9][i] = true;
                }
                break;
            case NONE:
                break;
        }
    }
    return room;
}