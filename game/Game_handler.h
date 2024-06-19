#include "General.h"

#ifndef SS_GAME_GAME_HANDLER_H
#define SS_GAME_GAME_HANDLER_H


class Game_handler {
    General_handler handler{};
public:
    Game_handler() = default;

    void game();

    vector<vector<bool>> build_room(const vector<Direction> &directions);

    vector<Enemy> build_enemies();
};


#endif //SS_GAME_GAME_HANDLER_H
