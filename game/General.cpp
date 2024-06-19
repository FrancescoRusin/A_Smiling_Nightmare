#include "General.h"

void General_handler::initialize() {
    rng = mt19937_64(duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    SDL_CreateWindowAndRenderer(800, 800, 0, &window, &renderer);
    sprite_map[WALL] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Brick_wall.jpg)");
    sprite_map[EMPTY_BOX] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Yellow_texture.jpg)");
    sprite_map[CLOWN] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Clown.png)");
    sprite_map[KARATEKA] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Blackbelt.png)");
    sprite_map[PROTAGONIST] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Smile.png)");
    sprite_map[PROTAGONIST_SHOT] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Orange_ball.png)");
    sprite_map[ENEMY_SHOT] = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Blue_ball.png)");
    swing_texture = IMG_LoadTexture(renderer, R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Swing.png)");
    protagonist = Entity(vector<int>{400, 400}, 35, 35, 0, PROTAGONIST);
    enemies = {};
    game_stats = Game_stats(20, 30, 1.0 / 120, 1.0 / 30, 20);}

void General_handler::room_change_animation(const vector<vector<bool>> &new_room, const vector<Enemy> &new_room_enemies, Direction direction) {
    assert(new_room.size() == 10 && all_of(new_room.begin(), new_room.end(), [](const vector<bool> &v) { return v.size() == 10; }));
    constexpr double alpha_tick = 1.5;
    constexpr double slide_tick = 3;
    if (room.empty()) {
        room = new_room;
        enemies = new_room_enemies;
        SDL_SetTextureBlendMode(sprite_map[WALL], SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(sprite_map[EMPTY_BOX], SDL_BLENDMODE_BLEND);
        SDL_SetTextureBlendMode(sprite_map[PROTAGONIST], SDL_BLENDMODE_BLEND);
        for (const Entity &e : enemies) {
            SDL_SetTextureBlendMode(sprite_map[e.type], SDL_BLENDMODE_BLEND);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (double i = 0; i <= 255; i += alpha_tick) {
            SDL_SetTextureAlphaMod(sprite_map[WALL], i);
            SDL_SetTextureAlphaMod(sprite_map[EMPTY_BOX], i);
            SDL_SetTextureAlphaMod(sprite_map[PROTAGONIST], i);
            for (const Entity &e : enemies) {
                SDL_SetTextureAlphaMod(sprite_map[e.type], i);
            }
            base_render();
        }
        SDL_SetTextureAlphaMod(sprite_map[WALL], 255);
        SDL_SetTextureAlphaMod(sprite_map[EMPTY_BOX], 255);
        SDL_SetTextureAlphaMod(sprite_map[PROTAGONIST], 255);
        for (const Entity &e : enemies) {
            SDL_SetTextureAlphaMod(sprite_map[e.type], 255);
        }
        return;
    }
    for (double i = 0; i < 800; i += slide_tick) {
        shifted_render(new_room, new_room_enemies, i, direction);
    }
    room = new_room;
    enemies = new_room_enemies;
}

void General_handler::base_render() noexcept {
    SDL_RenderClear(renderer);
    static auto map_crop = SDL_Rect(0, 0, 80, 80);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            map_crop.x = 80 * i;
            map_crop.y = 80 * j;
            SDL_RenderCopy(renderer, sprite_map[room[i][j] ? WALL : EMPTY_BOX], nullptr, &map_crop);
        }
    }
    SDL_SetTextureColorMod(sprite_map[PROTAGONIST], 255, 255 * (1 - protagonist.hit_tick / 60.0), 255 * (1 - protagonist.hit_tick / 60.0));
    protagonist.render(renderer, sprite_map[PROTAGONIST]);
    if (protagonist_swing) {
        SDL_SetTextureAlphaMod(swing_texture, 255 * 30 / protagonist_swing);
        double angle = atan2(protagonist_swing_direction[1], protagonist_swing_direction[0]) * 180 / M_PI + 45;
        const auto rect = SDL_Rect(protagonist.position[0] - 2 * protagonist.radius, protagonist.position[1] - 2 * protagonist.radius, 4 * protagonist.radius, 4 * protagonist.radius);
        SDL_RenderCopyEx(renderer, swing_texture, nullptr, &rect, angle, nullptr, SDL_FLIP_NONE);
        --protagonist_swing;
    }
    for (const Enemy &e: enemies) {
        SDL_SetTextureColorMod(sprite_map[e.type], 255, 255  * (1 - e.hit_tick / 60.0), 255  * (1 - e.hit_tick / 60.0));
        e.render(renderer, sprite_map[e.type]);
    }
    for (const Entity &e: protagonist_shots) {
        e.render(renderer, sprite_map[e.type]);
    }
    for (const Entity &e: enemy_shots) {
        e.render(renderer, sprite_map[e.type]);
    }
    SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
    SDL_RenderPresent(renderer);
    framerate_last_tick = SDL_GetTicks();
}

void General_handler::shifted_render(const vector<vector<bool>> &new_room, const vector<Enemy> &new_room_enemies, const int shift, const Direction direction) noexcept {
    SDL_RenderClear(renderer);
    static auto map_crop = SDL_Rect(0, 0, 80, 80);
    static int new_map_crop[2];
    static int tick[2];
    switch (direction) {
        case UP:
            tick[0] = 0;
            tick[1] = shift;
            new_map_crop[0] = 0;
            new_map_crop[1] = -800;
            break;
        case DOWN:
            tick[0] = 0;
            tick[1] = -shift;
            new_map_crop[0] = 0;
            new_map_crop[1] = 800;
            break;
        case RIGHT:
            tick[0] = -shift;
            tick[1] = 0;
            new_map_crop[0] = 800;
            new_map_crop[1] = 0;
            break;
        case LEFT:
            tick[0] = shift;
            tick[1] = 0;
            new_map_crop[0] = -800;
            new_map_crop[1] = 0;
            break;
    }
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            map_crop.x = 80 * i + tick[0];
            map_crop.y = 80 * j + tick[1];
            SDL_RenderCopy(renderer, sprite_map[room[i][j] ? WALL : EMPTY_BOX], nullptr, &map_crop);
            map_crop.x = 80 * i + new_map_crop[0] + tick[0];
            map_crop.y = 80 * j + new_map_crop[1] + tick[1];
            SDL_RenderCopy(renderer, sprite_map[new_room[i][j] ? WALL : EMPTY_BOX], nullptr, &map_crop);
        }
    }
    Entity protagonist_copy = Entity(protagonist);
    protagonist_copy.position[0] += tick[0];
    protagonist_copy.position[1] += tick[1];
    protagonist_copy.render(renderer, sprite_map[PROTAGONIST]);
    for (Entity e : enemies) {
        e.position[0] += tick[0];
        e.position[1] += tick[1];
        e.render(renderer, sprite_map[e.type]);
    }
    for (Entity e : new_room_enemies) {
        e.position[0] += tick[0];
        e.position[1] += tick[1];
        e.render(renderer, sprite_map[e.type]);
    }
    SDL_Delay(max(100.0 / 6.0 - static_cast<double>(SDL_GetTicks() - framerate_last_tick), 0.0));
    SDL_RenderPresent(renderer);
    framerate_last_tick = SDL_GetTicks();
}

bool General_handler::poll_events_and_update_positions() noexcept {
    static SDL_Event event;
    Direction shoot = NONE;
    bool swing = false;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
    }
    const Uint8 *keyboard_status = SDL_GetKeyboardState(nullptr);
    if (keyboard_status[SDL_SCANCODE_W]) {
        protagonist.velocity[1] = -PROTAGONIST_SPEED;
    }
    if (keyboard_status[SDL_SCANCODE_S]) {
        protagonist.velocity[1] = PROTAGONIST_SPEED;
    }
    if (keyboard_status[SDL_SCANCODE_D]) {
        protagonist.velocity[0] = PROTAGONIST_SPEED;
    }
    if (keyboard_status[SDL_SCANCODE_A]) {
        protagonist.velocity[0] = -PROTAGONIST_SPEED;
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
    if (keyboard_status[SDL_SCANCODE_SPACE]) {
        swing = true;
    }
    if (protagonist.action_tick) {
        --protagonist.action_tick;
    } else if (swing) {
        protagonist.action_tick = 30;
        protagonist_swing = 6;
        protagonist_swing_direction = vector<int>(2);
        switch (shoot) {
            case UP:
                protagonist_swing_direction[0] = 0;
                protagonist_swing_direction[1] = -1;
                break;
            case DOWN:
                protagonist_swing_direction[0] = 0;
                protagonist_swing_direction[1] = 1;
                break;
            case RIGHT:
                protagonist_swing_direction[0] = 1;
                protagonist_swing_direction[1] = 0;
                break;
            case LEFT:
                protagonist_swing_direction[0] = -1;
                protagonist_swing_direction[1] = 0;
                break;
            case NONE:
                protagonist.action_tick = 0;
                protagonist_swing = 0;
        }
        for (Enemy &e : enemies) {
            double distance_norm = point_point_distance(protagonist.position, e.position);
            if (distance_norm < 3 * protagonist.radius + e.radius) {
                vector<int> distance{e.position[0] - protagonist.position[0], e.position[1] - protagonist.position[1]};
                if (distance[0] * protagonist_swing_direction[0] + distance[1] * protagonist_swing_direction[1] > distance_norm * sqrt(.5)) {
                    e.hit_tick = 60;
                    e.hp -= 5;
                }
            }
        }
    } else {
        protagonist.action_tick = 20;
        if (shoot != NONE) {
            protagonist_shots.emplace_back(protagonist.position, 10, 0, ++id_counter, PROTAGONIST_SHOT);
        }
        switch (shoot) {
            case UP:
                protagonist_shots.rbegin()->velocity[0] = 0;
                protagonist_shots.rbegin()->velocity[1] = -20;
                break;
            case DOWN:
                protagonist_shots.rbegin()->velocity[0] = 0;
                protagonist_shots.rbegin()->velocity[1] = 20;
                break;
            case RIGHT:
                protagonist_shots.rbegin()->velocity[0] = 20;
                protagonist_shots.rbegin()->velocity[1] = 0;
                break;
            case LEFT:
                protagonist_shots.rbegin()->velocity[0] = -20;
                protagonist_shots.rbegin()->velocity[1] = 0;
                break;
            case NONE:
                protagonist.action_tick = 0;
        }
    }

    //protagonist movement
    map<int, vector<int>> previous_positions{{protagonist.id, vector<int>{protagonist.position[0], protagonist.position[1]}}};
    avoid_wall_collision(protagonist);
    protagonist.position[0] += protagonist.velocity[0];
    protagonist.position[1] += protagonist.velocity[1];

    //enemy movement
    for (Enemy &e: enemies) {
        if (e.hit_tick != 0) {
            --e.hit_tick;
        }
        previous_positions[e.id] = vector<int>(e.position);
        vector<double> protagonist_direction{static_cast<double>(protagonist.position[0] - e.position[0]), static_cast<double>(protagonist.position[1] - e.position[1])};
        double norm = sqrt(protagonist_direction[0] * protagonist_direction[0] + protagonist_direction[1] * protagonist_direction[1]);
        if (norm != 0) {
            protagonist_direction[0] /= norm;
            protagonist_direction[1] /= norm;
        } else {
            protagonist_direction[0] = protagonist_direction[1] = 0;
        }
        switch (e.type) {
            case KARATEKA:
                if (karateka_kick_animation[e.id] == 0) {
                    if (uniform_real(rng) < game_stats.karateka_kick_probability) {
                        karateka_kick_animation[e.id] = 1;
                    } else {
                        e.velocity[0] = static_cast<int>(protagonist_direction[0] * e.movement_average + e.movement_control * gauss(rng));
                        e.velocity[1] = static_cast<int>(protagonist_direction[1] * e.movement_average + e.movement_control * gauss(rng));
                    }
                    avoid_wall_collision(e);
                    e.position[0] += e.velocity[0];
                    e.position[1] += e.velocity[1];
                } else {
                    if (++karateka_kick_animation[e.id] > game_stats.karateka_kick_delay) {
                        if (avoid_wall_collision(e)) {
                            karateka_kick_animation[e.id] = 0;
                        }
                        e.position[0] += e.velocity[0];
                        e.position[1] += e.velocity[1];
                    } else if (karateka_kick_animation[e.id] < game_stats.karateka_kick_delay * .67) {
                        e.velocity[0] = static_cast<int>(protagonist_direction[0] * game_stats.karateka_kick_speed);
                        e.velocity[1] = static_cast<int>(protagonist_direction[1] * game_stats.karateka_kick_speed);
                    }
                }
                break;
            case CLOWN:
                if (uniform_real(rng) < game_stats.clown_shoot_probability) {
                    enemy_shots.emplace_back(e.position, 10, 0, ++id_counter, ENEMY_SHOT);
                    enemy_shots.rbegin()->velocity[0] = static_cast<int>(protagonist_direction[0] * game_stats.clown_shoot_speed);
                    enemy_shots.rbegin()->velocity[1] = static_cast<int>(protagonist_direction[1] * game_stats.clown_shoot_speed);
                }
                else if (uniform_real(rng) < e.movement_control) {
                    e.velocity[0] = static_cast<int>(gauss(rng)) + (2 * coin_flip(rng) - 1) * e.movement_average;
                    e.velocity[1] = static_cast<int>(gauss(rng)) + (2 * coin_flip(rng) - 1) * e.movement_average;
                }
                avoid_wall_collision(e);
                e.position[0] += e.velocity[0];
                e.position[1] += e.velocity[1];
                break;
            default:
                break;
        }
    }
    for (int i = 0; i < enemy_shots.size(); ++i) {
        previous_positions[enemy_shots[i].id] = vector<int>(enemy_shots[i].position);
        if (avoid_wall_collision(enemy_shots[i])) {
            enemy_shots.erase(next(enemy_shots.begin(), i));
            --i;
        } else {
            enemy_shots[i].position[0] += enemy_shots[i].velocity[0];
            enemy_shots[i].position[1] += enemy_shots[i].velocity[1];
        }
    }
    for (int i = 0; i < protagonist_shots.size(); ++i) {
        previous_positions[protagonist_shots[i].id] = vector<int>(protagonist_shots[i].position);
        if (avoid_wall_collision(protagonist_shots[i])) {
            protagonist_shots.erase(next(protagonist_shots.begin(), i));
            --i;
        } else {
            protagonist_shots[i].position[0] += protagonist_shots[i].velocity[0];
            protagonist_shots[i].position[1] += protagonist_shots[i].velocity[1];
        }
    }
    for (int i = 0; i < enemies.size(); ++i) {
        if (enemies[i].hp <= 0) {
            enemies.erase(next(enemies.begin(), i));
            --i;
        }
    }
    if (protagonist.hit_tick) {
        --protagonist.hit_tick;
    } else {
        for (const Enemy &e: enemies) {
            if (collide(previous_positions, protagonist, e)) {
                protagonist.hit_tick = 60;
                protagonist.hp -= 3;
                break;
            }
        }
        if (!protagonist.hit_tick) {
            for (int i = 0; i < enemy_shots.size(); ++i) {
                if (collide(previous_positions, enemy_shots[i], protagonist)) {
                    enemy_shots.erase(next(enemy_shots.begin(), i));
                    protagonist.hit_tick = 60;
                    --i;
                }
            }
        }
    }
    for (Enemy &e: enemies) {
        for (int i = 0; i < protagonist_shots.size(); ++i) {
            if (collide(previous_positions, protagonist_shots[i], e)) {
                protagonist_shots.erase(next(protagonist_shots.begin(), i));
                e.hp -= 5;
                e.hit_tick = 30;
                break;
            }
        }
    }
    protagonist.velocity[0] = protagonist.velocity[1] = 0;
    if (protagonist.hp <= 0) {
        cout << "GAME OVER";
        return false;
    }
    return true;
}

bool General_handler::collide(const map<int, vector<int>> &previous_positions, const Entity &entity1, const Entity &entity2) {
    vector<double> distances(4);
    distances[0] = point_point_distance(entity1.position, entity2.position);
    distances[1] = point_point_distance(entity1.position, previous_positions.at(entity2.id));
    distances[2] = point_point_distance(previous_positions.at(entity1.id), entity2.position);
    distances[3] = point_point_distance(previous_positions.at(entity1.id), previous_positions.at(entity2.id));
    return (*min_element(distances.begin(), distances.end())) < (entity1.radius + entity2.radius);
}

bool General_handler::avoid_wall_collision(Entity &entity) {
    bool collision = false;
    vector<int> position_cell{entity.position[0] / 80, entity.position[1] / 80};
    if (position_cell[0] < 0 || position_cell[0] > 9 || position_cell[1] < 0 || position_cell[1] > 9) {
        entity.velocity[0] = 0;
        entity.velocity[1] = 0;
        return true;
    }
    if (entity.position[0] % 80 <= entity.radius &&
        ((position_cell[0] > 0 && room[position_cell[0] - 1][position_cell[1]]) ||
         (position_cell[0] == 0))) {
        if (entity.velocity[0] < 0) {
            collision = true;
            entity.velocity[0] = 0;
        }
    }
    if (entity.position[0] % 80 >= 80 - entity.radius &&
        ((position_cell[0] < 9 && room[position_cell[0] + 1][position_cell[1]]) ||
         (position_cell[0] == 9))) {
        if (entity.velocity[0] > 0) {
            collision = true;
            entity.velocity[0] = 0;
        }
    }
    if (entity.position[1] % 80 <= entity.radius &&
        ((position_cell[1] > 0 && room[position_cell[0]][position_cell[1] - 1]) ||
         (position_cell[1] == 0))) {
        if (entity.velocity[1] < 0) {
            collision = true;
            entity.velocity[1] = 0;
        }
    }
    if (entity.position[1] % 80 >= 80 - entity.radius &&
        ((position_cell[1] < 9 && room[position_cell[0]][position_cell[1] + 1]) ||
         (position_cell[1] == 9))) {
        if (entity.velocity[1] > 0) {
            collision = true;
            entity.velocity[1] = 0;
        }
    }
    return collision;
}

double line_point_distance(int *line, const vector<int> &point) {
    return abs(line[0] * point[0] + line[1] * point[1] + line[2]) / sqrt(point[0] * point[0] + point[1] * point[1]);
}

vector<double> line_line_intersection(int *line1, int *line2) {
    int cramer_det = line1[0] * line2[1] - line1[1] * line2[0];
    if (cramer_det == 0) {
        return vector<double>{-1, -1};
    }
    return vector<double>{
            static_cast<double>(line2[1] * line1[2] - line2[2] * line1[1]) / cramer_det,
            static_cast<double>(line2[2] * line1[0] - line2[0] * line1[2]) / cramer_det
    };
}

double point_point_distance(const vector<int> &p1, const vector<int> &p2) {
    return sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) + (p1[1] - p2[1]) * (p1[1] - p2[1]));
}

void Entity::render(SDL_Renderer *renderer, SDL_Texture *texture) const noexcept {
    const auto mob_crop = SDL_Rect(position[0] - radius, position[1] - radius, 2 * radius, 2 * radius);
    SDL_RenderCopy(renderer, texture, nullptr, &mob_crop);
}