#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <vector>
#include "Defines.h"

class Game {
public:
    Game();
    ~Game(); // Hàm hủy

    // Khởi tạo SDL, cửa sổ, renderer, và load tài nguyên
    bool init(const char* title, int width, int height);
    void run();
    void close();

private:
    // Xử lý sự kiện (input)
    void handleEvents();
    void renderBoard();
    void renderMenu();
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
    // Vẽ thông báo trạng thái game (lượt ai, ai thắng, hòa)
    void renderStatus();
    // Biến thành viên
    SDL_Window* gWindow = nullptr;
    SDL_Renderer* gRenderer = nullptr;
    // Textures
    SDL_Texture* gGridTexture = nullptr;
    SDL_Texture* gXTexture = nullptr;
    SDL_Texture* gOTexture = nullptr;
    SDL_Texture* gPlayButtonTexture = nullptr;
    SDL_Texture* gQuitButtonTexture = nullptr;
    // Vị trí các nút (SDL_Rect)
    SDL_Rect playButtonRect;
    SDL_Rect quitButtonRect;
    // Trạng thái bàn cờ (dùng vector 2D hoặc mảng 2D)
    int board[GRID_SIZE][GRID_SIZE]; // 0: empty, 1: X, 2: O

    int currentPlayer; // Người chơi hiện tại (PLAYER_X hoặc PLAYER_O)
    int gameState;     // Trạng thái game (STATE_PLAYING, STATE_X_WINS, ...)
    bool isRunning;    // Cờ cho vòng lặp chính
    TTF_Font* gFont; // Font để vẽ chữ

    // Biến lưu thông tin đường thắng (tùy chọn nâng cao)
    enum WinLineType { NONE, ROW, COLUMN, DIAG_MAIN, DIAG_ANTI };
    WinLineType winType = NONE;
    int winIndex = -1; // Chỉ số hàng/cột hoặc 0/1 cho đường chéo
};

#endif // GAME_H
