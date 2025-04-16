#include "Game.h"
#include "Defines.h" // Bao gồm Defines để dùng SCREEN_WIDTH, SCREEN_HEIGHT
#include <iostream>  // Thêm iostream

int main(int argc, char* args[]) { // Chú ý tham số của main cho SDL
    Game ticTacToeGame;

    if (ticTacToeGame.init("Tic Tac Toe ", SCREEN_WIDTH, SCREEN_HEIGHT)) {
        ticTacToeGame.run(); // Bắt đầu vòng lặp game
    } else {
        std::cerr << "Failed to initialize the game!" << std::endl;
    }

    ticTacToeGame.close(); // Dọn dẹp khi kết thúc

    return 0;
}
