#include "General.h"

int main(int, char**) {
    General_handler view;
    view.room_change_animation(vector<vector<bool>>(10, vector<bool>(10)), vector<Enemy>(), UP);
    vector<vector<bool>> new_room(10, vector<bool>(10));
    bool keep_open = true;
    int counter = 0;
    while (keep_open) {
        view.base_render();
        keep_open = view.poll_events_and_update_positions();
        new_room[counter / 10][counter % 10] = true;
        //view.room_change_animation(new_room, vector<Enemy>(), LEFT);
        ++counter;
        counter %= 100;
    }
    return 0;
}
