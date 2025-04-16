#include "Game.h"
#include "Defines.h"
#include <iostream>

int main(int argc, char* args[]){
    Game ticTacToeGame;

    if (ticTacToeGame.init("Tic Tac Toe ", SCREEN_WIDTH, SCREEN_HEIGHT)) {
        ticTacToeGame.run(); // Bắt đầu vòng lặp :v
    } else {
        std::cerr << "Failed to initialize the game!" << std::endl;
    }

    ticTacToeGame.close();

    return 0;
}
