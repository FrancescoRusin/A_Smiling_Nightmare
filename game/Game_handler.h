#include "General.h"

#ifndef SS_GAME_GAME_HANDLER_H
#define SS_GAME_GAME_HANDLER_H


class Game_handler {
    General_handler handler{};
    mt19937_64 rng;
public:
    Game_handler() {
        rng = mt19937_64(duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
    }

    void game();

    vector<int> build_floor();

    vector<vector<bool>> build_room(const vector<Direction> &directions);

    vector<Enemy> build_enemies();
};

vector<int> find_neighbors(int r);

#endif //SS_GAME_GAME_HANDLER_H
