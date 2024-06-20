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

vector<Direction> find_closed_directions(const vector<int> &floor, int room_number) {
    vector<Direction> directions(0);
    if (find(floor.begin(), floor.end(), room_number - 8) == floor.end()) {
        directions.push_back(UP);
    }
    if (room_number % 8 == 0 || find(floor.begin(), floor.end(), room_number - 1) == floor.end()) {
        directions.push_back(LEFT);
    }
    if (room_number % 8 == 7 || find(floor.begin(), floor.end(), room_number + 1) == floor.end()) {
        directions.push_back(RIGHT);
    }
    if (find(floor.begin(), floor.end(), room_number + 8) == floor.end()) {
        directions.push_back(DOWN);
    }
    return directions;
}

void Game_handler::game() {
    uniform_int_distribution<int> final_room_extractor = uniform_int_distribution<int>(0, 7);
    handler.initialize(false);
    int room_number = 36;
    vector<int> floor = build_floor();
    map<int, bool> cleared{};
    map<int, vector<vector<bool>>> rooms{};
    for (int r: floor) {
        rooms[r] = build_room(find_closed_directions(floor, r));
    }
    int final_room_number = floor[final_room_extractor(rng)];
    while (final_room_number == 28 || final_room_number == 35 || final_room_number == 36 || final_room_number == 37 || final_room_number == 44) {
        final_room_number = floor[final_room_extractor(rng)];
    }
    vector<Enemy> enemies = build_enemies(rooms[36]);
    handler.room_change_animation(rooms[36], enemies, NONE);
    bool keep_open = true;
    while (keep_open) {
        // base loop
        while (keep_open && (!handler.enemies.empty() ||
                             (abs(handler.protagonist.position[0] - 400) < 350 && abs(handler.protagonist.position[1] - 400) < 350)) &&
               (!handler.enemies.empty() || room_number != final_room_number ||
                abs(handler.protagonist.position[0] - 400) > 25 || abs(handler.protagonist.position[1] - 400) > 25)) {
            handler.base_render();
            keep_open = handler.poll_events_and_update_positions();
        }
        // if player is dead or game was quitted
        if (!keep_open) {
            adapt();
            handler.room_change_animation(vector<vector<bool>>{}, vector<Enemy>{}, NONE);
        } else {
            // if player took the trapdoor for the next floor
            if (handler.final_room && abs(handler.protagonist.position[0] - 400) < 25 && abs(handler.protagonist.position[1] - 400) < 25) {
                handler.room_change_animation(vector<vector<bool>>{}, vector<Enemy>{}, NONE);
                adapt();
                handler.floor_data.reset();
                handler.protagonist.hp = 30;
                floor = build_floor();
                cleared.clear();
                rooms.clear();
                for (int r: floor) {
                    rooms[r] = build_room(find_closed_directions(floor, r));
                }
                final_room_number = floor[final_room_extractor(rng)];
                while (final_room_number == 26 || final_room_number == 35 || final_room_number == 36 || final_room_number == 37 || final_room_number == 46) {
                    final_room_number = floor[final_room_extractor(rng)];
                }
                room_number = 36;
                handler.final_room = false;
                handler.room_change_animation(rooms[room_number], build_enemies(rooms[room_number]), NONE);
            } else {
                // if player just changed room
                cleared[room_number] = true;
                Direction direction = NONE;
                if (handler.protagonist.position[0] > 750) {
                    handler.protagonist.position[0] = 50 + handler.protagonist.radius;
                    direction = RIGHT;
                    room_number += 1;
                } else if (handler.protagonist.position[0] < 50) {
                    handler.protagonist.position[0] = 750 - handler.protagonist.radius;
                    direction = LEFT;
                    room_number -= 1;
                } else if (handler.protagonist.position[1] > 750) {
                    handler.protagonist.position[1] = 50 + handler.protagonist.radius;
                    direction = DOWN;
                    room_number += 8;
                } else if (handler.protagonist.position[1] < 50) {
                    handler.protagonist.position[1] = 750 - handler.protagonist.radius;
                    direction = UP;
                    room_number -= 8;
                }
                if (!cleared[room_number]) {
                    handler.protagonist.hp = min(30, handler.protagonist.hp + 10);
                }
                handler.final_room = (room_number == final_room_number);
                handler.room_change_animation(rooms[room_number], cleared[room_number] ? vector<Enemy>{} : build_enemies(rooms[room_number]), direction);
            }
        }
    }
}

vector<int> Game_handler::build_floor() {
    uniform_int_distribution<int> uniform4 = uniform_int_distribution<int>(0, 4);
    uniform_real_distribution<double> uniform_unit = uniform_real_distribution<double>();
    vector<int> queue(1, 36);
    vector<int> reachable(1, 36);
    while (reachable.size() < 8) {
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

vector<Enemy> Game_handler::build_enemies(const vector<vector<bool>> &room) {
    uniform_int_distribution<int> uniform800 = uniform_int_distribution<int>(0, 799);
    const int n_of_enemies = min(average_num_of_enemies + static_cast<int>(round(normal_distribution<double>()(rng))), 5);
    vector<Enemy> enemies(0);
    vector<int> position(2);
    for (int i = 0; i < n_of_enemies; ++i) {
        position[0] = uniform800(rng);
        position[1] = uniform800(rng);
        while (point_point_distance(position, handler.protagonist.position) < 8 * handler.protagonist.radius ||
        room[position[0] / 50][position[1] / 50]) {
            position[0] = uniform800(rng);
            position[1] = uniform800(rng);
        }
        if (uniform_unit(rng) < karateka_probability) {
            enemies.emplace_back(position, 25, 20, ++(handler.id_counter), KARATEKA, handler.game_stats.karateka_average_speed, 2);
        } else {
            enemies.emplace_back(position, 25, 10, ++(handler.id_counter), CLOWN, 0, .8);
        }
    }
    return enemies;
}

vector<vector<bool>> Game_handler::build_room(const vector<Direction> &directions) {
    vector<vector<bool>> room(16, vector<bool>(16));
    for (const Direction &direction: directions) {
        switch (direction) {
            case UP:
                for (int i = 1; i < 15; ++i) {
                    room[i][0] = true;
                }
                break;
            case DOWN:
                for (int i = 1; i < 15; ++i) {
                    room[i][15] = true;
                }
                break;
            case LEFT:
                for (int i = 1; i < 15; ++i) {
                    room[0][i] = true;
                }
                break;
            case RIGHT:
                for (int i = 1; i < 15; ++i) {
                    room[15][i] = true;
                }
                break;
            case NONE:
                break;
        }
    }
    room[0][0] = room[0][15] = room[15][0] = room[15][15] = true;
    for (int i = 1; i < 7; ++i) {
        room[i][0] = room[0][i] = room[15][i] = room[i][15] = room[15 - i][0] = room[0][15 - i] = room[15][15 - i] = room[15 - i][15] = true;
    }
    room[1][1] = room[1][14] = room[14][1] = room[14][14] = true;
    for (int dist = 1; dist < 6; ++dist) {
        for (int i = 0; i < dist + 1; ++i) {
            room[1 + i][1 + dist - i] = room[1 + i][dist - i] && room[i][1 + dist - i] && uniform_unit(rng) < .75;
            room[14 - i][1 + dist - i] = room[14 - i][dist - i] && room[15 - i][1 + dist - i] && uniform_unit(rng) < .75;
            room[1 + i][14 - dist + i] = room[1 + i][15 - dist + i] && room[i][14 - dist + i] && uniform_unit(rng) < .75;
            room[14 - i][14 - dist + i] = room[14 - i][15 - dist + i] && room[15 - i][14 - dist + i] && uniform_unit(rng) < .75;
        }
    }
    return room;
}

void Game_handler::adapt() {
    file.open("game_stats.txt", ios::out | ios::app);
    file << format("{},{},{},{},{},{},{},{}\n",
                   handler.protagonist.hp,
                   handler.floor_data.protagonist_shots_fired,
                   handler.floor_data.protagonist_shots_hit,
                   handler.floor_data.protagonist_swings,
                   handler.floor_data.protagonist_swings_hit,
                   handler.floor_data.enemy_shots_fired,
                   handler.floor_data.enemy_shots_hit,
                   handler.floor_data.enemy_contact_hits
    );
    file.close();
    double enemy_shot_precision = handler.floor_data.enemy_shots_hit / handler.floor_data.enemy_shots_fired;
    double protagonist_shot_precision = handler.floor_data.protagonist_shots_hit / handler.floor_data.protagonist_shots_fired;
    double protagonist_swing_preference = handler.floor_data.protagonist_swings / (handler.floor_data.protagonist_shots_fired + handler.floor_data.protagonist_swings);
    double protagonist_status = handler.protagonist.hp / 30;
}

