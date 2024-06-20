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
        cout << "UP\n";
        directions.push_back(UP);
    }
    if (room_number % 8 == 0 || find(floor.begin(), floor.end(), room_number - 1) == floor.end()) {
        cout << "LEFT\n";
        directions.push_back(LEFT);
    }
    if (room_number % 8 == 7 || find(floor.begin(), floor.end(), room_number + 1) == floor.end()) {
        cout << "RIGHT\n";
        directions.push_back(RIGHT);
    }
    if (find(floor.begin(), floor.end(), room_number + 8) == floor.end()) {
        cout << "DOWN\n";
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
    int final_room_number = floor[final_room_extractor(rng)];
    while (final_room_number == 28 || final_room_number == 35 || final_room_number == 36 || final_room_number == 37 || final_room_number == 44) {
        final_room_number = floor[final_room_extractor(rng)];
    }
    vector<Direction> directions = find_closed_directions(floor, 36);
    vector<vector<bool>> room = build_room(directions);
    vector<Enemy> enemies = build_enemies(room);
    handler.room_change_animation(room, enemies, NONE);
    bool keep_open = true;
    for (int i = 0; i < 8; ++i) {
        printf("%d ", floor[i]);
    }
    cout << "\n";
    while (keep_open) {
        while (handler.protagonist.hp > 0 && (!handler.enemies.empty() ||
                                              (abs(handler.protagonist.position[0] - 400) < 350 && abs(handler.protagonist.position[1] - 400) < 350)) &&
               (!handler.enemies.empty() || room_number != final_room_number ||
                abs(handler.protagonist.position[0] - 400) > 25 || abs(handler.protagonist.position[1] - 400) > 25)) {
            handler.base_render();
            keep_open = handler.poll_events_and_update_positions();
        }
        if (handler.protagonist.hp <= 0) {
            handler.room_change_animation(vector<vector<bool>>{}, vector<Enemy>{}, NONE);
            keep_open = false;
        } else {
            if (handler.final_room && abs(handler.protagonist.position[0] - 400) < 25 && abs(handler.protagonist.position[1] - 400) < 25) {
                handler.room_change_animation(vector<vector<bool>>{}, vector<Enemy>{}, NONE);
                floor = build_floor();
                final_room_number = floor[final_room_extractor(rng)];
                while (final_room_number == 26 || final_room_number == 35 || final_room_number == 36 || final_room_number == 37 || final_room_number == 46) {
                    final_room_number = floor[final_room_extractor(rng)];
                }
                room_number = 36;
                directions = find_closed_directions(floor, room_number);
                room = build_room(directions);
                handler.final_room = false;
                handler.room_change_animation(room, build_enemies(room), NONE);
            } else {
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
                handler.final_room = (room_number == final_room_number);
                directions = find_closed_directions(floor, room_number);
                room = build_room(directions);
                cout << room_number << "\n";
                handler.room_change_animation(room, cleared[room_number] ? vector<Enemy>{} : build_enemies(room), direction);
            }
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

vector<Enemy> Game_handler::build_enemies(const vector<vector<bool>> &room) {
    uniform_real_distribution<double> uniform_unit = uniform_real_distribution<double>();
    uniform_int_distribution<int> uniform800 = uniform_int_distribution<int>(0, 799);
    const int n_of_enemies = min(average_num_of_enemies + static_cast<int>(round(normal_distribution<double>()(rng))), 6);
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
            enemies.emplace_back(position, 25, 20, ++(handler.id_counter), KARATEKA, 7, 2);
        } else {
            enemies.emplace_back(position, 25, 20, ++(handler.id_counter), CLOWN, 0, .8);
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
                for (int i = 0; i < 16; ++i) {
                    room[i][0] = true;
                }
                break;
            case DOWN:
                for (int i = 0; i < 16; ++i) {
                    room[i][15] = true;
                }
                break;
            case LEFT:
                for (int i = 0; i < 16; ++i) {
                    room[0][i] = true;
                }
                break;
            case RIGHT:
                for (int i = 0; i < 16; ++i) {
                    room[15][i] = true;
                }
                break;
            case NONE:
                break;
        }
    }
    return room;
}