#include "Map.h"

int main(int, char**) {
    View_handler view;
    view.set_empty_box(R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Yellow_texture.jpg)");
    view.set_wall(R"(C:\Users\Francesco\Desktop\Progetti\SS_game\Brick_wall.jpg)");
    view.room_change_animation(vector<vector<bool>>(10, vector<bool>(10)), vector<Entity>(), UP, 15);
    vector<vector<bool>> new_room(10, vector<bool>(10));
    int counter = 0;
    while (counter < 100) {
        view.base_render();
        new_room[counter / 10][counter % 10] = true;
        view.room_change_animation(new_room, vector<Entity>(), UP, 5);
        for (int i = 0; i < 60; ++i) {
            view.base_render();
        }
        ++counter;
    }
    return 0;
}
