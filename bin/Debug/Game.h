#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector> // Sử dụng vector thay mảng C-style nếu muốn linh hoạt hơn
#include "Defines.h"

class Game {
public:
    Game();
    ~Game(); // Hàm hủy

    // Khởi tạo SDL, cửa sổ, renderer, và load tài nguyên
    bool init(const char* title, int width, int height);

    // Vòng lặp chính của game
    void run();

    // Dọn dẹp tài nguyên
    void close();

private:
    // Xử lý sự kiện (input)
    void handleEvents();

    // Cập nhật trạng thái game (logic) - ít dùng trong Tic Tac Toe đơn giản
    // void update(); // Có thể bỏ qua nếu logic xử lý trong handleEvents

    // Vẽ mọi thứ lên màn hình (render)
    void render();

    // Load các textures cần thiết
    bool loadMedia();

    // Đặt dấu X hoặc O vào ô
    bool placeMark(int row, int col);

    // Kiểm tra thắng
    int checkWin();

    // Kiểm tra hòa
    bool checkDraw();

    // Chuyển lượt chơi
    void switchPlayer();

    // Reset game để chơi lại
    void resetGame();

    // Vẽ bàn cờ và các quân cờ
    void renderBoard();

    // Vẽ thông báo trạng thái game (lượt ai, ai thắng, hòa)
    void renderStatus(); // Sẽ cần SDL_ttf để vẽ chữ đẹp, tạm thời bỏ qua hoặc vẽ đơn giản

    // Biến thành viên
    SDL_Window* gWindow = nullptr;
    SDL_Renderer* gRenderer = nullptr;

    // Textures
    SDL_Texture* gGridTexture = nullptr;
    SDL_Texture* gXTexture = nullptr;
    SDL_Texture* gOTexture = nullptr;
    // Có thể thêm texture cho thông báo thắng/hòa/chơi lại sau

    // Trạng thái bàn cờ (dùng vector 2D hoặc mảng 2D)
    int board[GRID_SIZE][GRID_SIZE]; // 0: empty, 1: X, 2: O

    int currentPlayer; // Người chơi hiện tại (PLAYER_X hoặc PLAYER_O)
    int gameState;     // Trạng thái game (STATE_PLAYING, STATE_X_WINS, ...)
    bool isRunning;    // Cờ cho vòng lặp chính
};

#endif // GAME_H
