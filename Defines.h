#ifndef DEFINES_H
#define DEFINES_H
// Kích thước màn hình và lưới
const int SCREEN_WIDTH = 600 ;
const int SCREEN_HEIGHT = 600;
const int GRID_SIZE = 3;
const int CELL_SIZE = SCREEN_WIDTH / GRID_SIZE;
// Trạng thái ô
const int CELL_EMPTY = 0;
const int CELL_X = 1;
const int CELL_O = 2;
// Người chơi
const int PLAYER_X = CELL_X;
const int PLAYER_O = CELL_O;
// Trạng thái game
const int STATE_MENU = -1;
const int STATE_PLAYING = 0;
const int STATE_X_WINS = 1;
const int STATE_O_WINS = 2;
const int STATE_DRAW = 3;
const int STATE_QUIT = 4;
#endif // DEFINES_H
