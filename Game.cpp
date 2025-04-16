#include "Game.h"
#include "TextureManager.h"
#include "Defines.h"
#include <iostream>
#include <string>
#include <SDL_ttf.h>
using namespace std;
Game::Game() : gWindow(nullptr), gRenderer(nullptr), gGridTexture(nullptr),gFont(nullptr),
               gXTexture(nullptr), gOTexture(nullptr), gPlayButtonTexture(nullptr), gQuitButtonTexture(nullptr),
               currentPlayer(PLAYER_X), gameState(STATE_MENU), isRunning(false)
{
    // Khởi tạo bàn cờ trống
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            board[i][j] = CELL_EMPTY;
        }
    }
      // Định nghĩa vị trí và kích thước cho các nút menu
    playButtonRect.w = 200;
    playButtonRect.h = 60;
    playButtonRect.x = (SCREEN_WIDTH - playButtonRect.w) / 2;
    playButtonRect.y = SCREEN_HEIGHT / 2 - playButtonRect.h - 15;

    quitButtonRect.w = 200;
    quitButtonRect.h = 60;
    quitButtonRect.x = (SCREEN_WIDTH - quitButtonRect.w) / 2;
    quitButtonRect.y = SCREEN_HEIGHT / 2 + 15;
}

Game::~Game() {close();}

bool Game::init(const char* title, int width, int height) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    // Khởi tạo SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
        SDL_Quit();
        return false;
    }
     if (TTF_Init() == -1) {
        cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << endl;
        IMG_Quit(); // Dọn dẹp những gì đã init trước đó
        SDL_Quit();
        return false;
    }
    // Tạo cửa sổ
    gWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }
    // Tạo renderer
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == nullptr) {
        cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << endl;
        SDL_DestroyWindow(gWindow);
        IMG_Quit();
        SDL_Quit();
        TTF_Quit();
        return false;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF); // Màu trắng

    // Load media
    if (!loadMedia()) {
        close();
        return false;
    }
    isRunning = true; //chạy vòng lặp game:))
    return true;
}

bool Game::loadMedia() {
    // Đường dẫn tương đối đến thư mục assets
    gGridTexture = TextureManager::LoadTexture("assets/grid.png", gRenderer);
    gXTexture = TextureManager::LoadTexture("assets/x.png", gRenderer);
    gOTexture = TextureManager::LoadTexture("assets/o.png", gRenderer);
    gPlayButtonTexture = TextureManager::LoadTexture("assets/play_button.png", gRenderer);
    gQuitButtonTexture = TextureManager::LoadTexture("assets/quit_button.png", gRenderer);
    gFont = TTF_OpenFont("assets/fonts/times.ttf", 36);
    if (gGridTexture == nullptr || gXTexture == nullptr || gOTexture == nullptr ||
        gPlayButtonTexture == nullptr || gQuitButtonTexture == nullptr || gFont == nullptr) {
        cerr << "Failed to load one or more textures!" << endl;
        return false;
    }
    return true;
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        render();
    }
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            isRunning = false;
            gameState = STATE_QUIT;
        }

        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            if (gameState == STATE_MENU) { // Xử lý click khi ở Menu
                SDL_Point mousePoint = {mouseX, mouseY};
                // Kiểm tra click vào nút Play
                if (SDL_PointInRect(&mousePoint, &playButtonRect)) {
                    cout << "Play button clicked!" << endl;
                    resetGame();
                    gameState = STATE_PLAYING;
                }
                // Kiểm tra click vào nút Quit
                else if (SDL_PointInRect(&mousePoint, &quitButtonRect)) {
                    cout << "Quit button clicked!" << endl;
                    isRunning = false;
                    gameState = STATE_QUIT;
                }
            }
            else if (gameState == STATE_PLAYING){
                int clickedRow = mouseY / CELL_SIZE;
                int clickedCol = mouseX / CELL_SIZE;

                if (placeMark(clickedRow, clickedCol)) {
                    int winResult = checkWin();
                    if (winResult != STATE_PLAYING) {
                        gameState = winResult;
                    } else if (checkDraw()) {
                        gameState = STATE_DRAW;
                    } else {
                        switchPlayer();
                    }
                }
            }
            else if (gameState == STATE_X_WINS || gameState == STATE_O_WINS || gameState == STATE_DRAW) {
                // Click khi game kết thúc -> Quay về Menu
                cout << "Game over clicked, returning to menu." << endl;
                gameState = STATE_MENU;
            }
        }
    }
}

bool Game::placeMark(int row, int col) {
    // Kiểm tra ô có hợp lệ và còn trống không
    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE && board[row][col] == CELL_EMPTY) {
        board[row][col] = currentPlayer;
        return true; // Đặt thành công
    }
    return false;
}
int Game::checkWin() {
    winType = NONE; // Reset trước khi kiểm tra
    winIndex = -1;
    // Kiểm tra hàng ngang
    for (int i = 0; i < GRID_SIZE; ++i) {
        if (board[i][0] != CELL_EMPTY && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return board[i][0] == PLAYER_X ? STATE_X_WINS : STATE_O_WINS;
        }
    }

    // Kiểm tra hàng dọc
    for (int j = 0; j < GRID_SIZE; ++j) {
        if (board[0][j] != CELL_EMPTY && board[0][j] == board[1][j] && board[1][j] == board[2][j]) {
            return board[0][j] == PLAYER_X ? STATE_X_WINS : STATE_O_WINS;
        }
    }

    // Kiểm tra đường chéo chính
    if (board[0][0] != CELL_EMPTY && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return board[0][0] == PLAYER_X ? STATE_X_WINS : STATE_O_WINS;
    }

    // Kiểm tra đường chéo phụ
    if (board[0][2] != CELL_EMPTY && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return board[0][2] == PLAYER_X ? STATE_X_WINS : STATE_O_WINS;
    }

    return STATE_PLAYING; // Chưa ai thắng
}

bool Game::checkDraw() {
    // Nếu đã có người thắng thì không phải hòa
    if (gameState == STATE_X_WINS || gameState == STATE_O_WINS) {
        return false;
    }
    // Kiểm tra xem còn ô trống nào không
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (board[i][j] == CELL_EMPTY) {
                return false; // Còn ô trống, chưa hòa
            }
        }
    }
    return true; // Tất cả các ô đã được điền và không ai thắng -> Hòa
}
void Game::switchPlayer() {
    currentPlayer = (currentPlayer == PLAYER_X) ? PLAYER_O : PLAYER_X;
}

void Game::resetGame() {
    cout << "Resetting game board and player..." << endl;
    // Đặt lại bàn cờ
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            board[i][j] = CELL_EMPTY;
        }
    }
    // Đặt lại người chơi
    currentPlayer = PLAYER_X;
    winType = NONE; // CHỈNH SỬA: Reset thông tin đường thắng
    winIndex = -1;
    cout << "Game Reset for Playing!" << endl;
}
void Game::render() {
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);
    if (gameState == STATE_MENU) {
            renderMenu();
    } else {
        if (gGridTexture) {
            TextureManager::Draw(gGridTexture, 0, 0, gRenderer);
        }
        // Vẽ các quân X và O
        renderBoard();
        // Vẽ trạng thái game (Thắng/Hòa/Lượt)
        renderStatus();
    }
    SDL_RenderPresent(gRenderer);
}
void Game::renderBoard() {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            int cellX = j * CELL_SIZE;
            int cellY = i * CELL_SIZE;

            // Canh giữa X/O trong ô (giả sử X/O có kích thước nhỏ hơn ô)
            int textureW, textureH;
            int offsetX = 0, offsetY = 0;

            if (board[i][j] == PLAYER_X) {
                if (gXTexture) {
                    SDL_QueryTexture(gXTexture, NULL, NULL, &textureW, &textureH);
                    offsetX = (CELL_SIZE - textureW) / 2;
                    offsetY = (CELL_SIZE - textureH) / 2;
                    TextureManager::Draw(gXTexture, cellX + offsetX, cellY + offsetY, gRenderer);
                }
            } else if (board[i][j] == PLAYER_O) {
                 if (gOTexture) {
                    SDL_QueryTexture(gOTexture, NULL, NULL, &textureW, &textureH);
                    offsetX = (CELL_SIZE - textureW) / 2;
                    offsetY = (CELL_SIZE - textureH) / 2;
                    TextureManager::Draw(gOTexture, cellX + offsetX, cellY + offsetY, gRenderer);
                 }
            }
        }
    }
}
void Game::renderStatus() {
    if (gFont == nullptr) {
        cerr << "Warning: gFont is null in renderStatus. Cannot render text." << endl;
        return;
    }
    string message = "";
    bool showEndGameMessage = false;
    // Xác định thông điệp dựa trên gameState
    switch (gameState) {
        case STATE_X_WINS:
            message = "X Wins!";
            showEndGameMessage = true;
            break;
        case STATE_O_WINS:
            message = "O Wins!";
            showEndGameMessage = true;
            break;
        case STATE_DRAW:
            message = "Draw!";
            showEndGameMessage = true;
            break;
        case STATE_PLAYING:
            message = (currentPlayer == PLAYER_X) ? "Player X's Turn" : "Player O's Turn";
            break;
        default:
            // Không hiển thị gì cho các trạng thái khác (như STATE_MENU)
            break;
    }
    if (!message.empty()) {
        SDL_Color textColor = {0, 0, 0, 255}; // Màu đen (RGBA)
        if (showEndGameMessage) {
             textColor = { 200, 0, 0, 255 }; // Màu đỏ cho thông báo kết thúc
        }
        SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, message.c_str(), textColor);
        if (textSurface == nullptr) {
            cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << endl;
        } else {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
            if (textTexture == nullptr) {
                cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << endl;
            } else {
                // Tính toán vị trí vẽ
                SDL_Rect renderQuad = {0, 0, textSurface->w, textSurface->h};

                if (showEndGameMessage) {
                    // Căn giữa màn hình cho thông báo kết thúc
                    renderQuad.x = (SCREEN_WIDTH - renderQuad.w) / 2;
                    renderQuad.y = (SCREEN_HEIGHT - renderQuad.h) / 2;
                } else {
                    //  Đặt thông báo lượt chơi ở góc trên cùng bên trái
                    renderQuad.x = 10;
                    renderQuad.y = 10;
                }
                // Vẽ texture lên màn hình
                SDL_RenderCopy(gRenderer, textTexture, nullptr, &renderQuad);

                // Quan trọng: Hủy texture vì nó được tạo mỗi frame
                SDL_DestroyTexture(textTexture);
                textTexture = nullptr;
            }
            //Giải phóng surface
            SDL_FreeSurface(textSurface);
            textSurface = nullptr;
        }
    }
     // Tùy chọn: Vẽ thêm thông báo "Press R to Play Again" khi game kết thúc
    if (showEndGameMessage) {
        string playAgainMsg = "Click to Play Again";
        SDL_Color againColor = { 50, 50, 50, 255 }; // Màu xám đậm
        SDL_Surface* againSurface = TTF_RenderText_Solid(gFont, playAgainMsg.c_str(), againColor);
        if (againSurface) {
            SDL_Texture* againTexture = SDL_CreateTextureFromSurface(gRenderer, againSurface);
            if (againTexture) {
                SDL_Rect againQuad = {0, 0, againSurface->w, againSurface->h};
                againQuad.x = (SCREEN_WIDTH - againQuad.w) / 2;
                againQuad.y = (SCREEN_HEIGHT / 2) + 30; // Đặt dưới thông báo chính một chút
                SDL_RenderCopy(gRenderer, againTexture, nullptr, &againQuad);
                SDL_DestroyTexture(againTexture);
            }
            SDL_FreeSurface(againSurface);
        }
    }
}
void Game::renderMenu() {
         SDL_SetRenderDrawColor(gRenderer, 200, 200, 255, 255); // Màu xanh nhạt
         SDL_RenderClear(gRenderer);
         cout << "Rendering Menu..." << endl; // Kiểm tra hàm có được gọi
    // Vẽ nút Play
    if (gPlayButtonTexture) {
        // Lấy kích thước gốc của texture nút để vẽ vào đúng Rect đã định nghĩa
        // Tham số thứ 3 là NULL để vẽ toàn bộ texture, tham số thứ 4 là con trỏ đến Rect đích
         cout << "Drawing Play Button at x=" << playButtonRect.x << " y=" << playButtonRect.y << endl;
         SDL_RenderCopy(gRenderer, gPlayButtonTexture, NULL, &playButtonRect);
       } else {
        cout << "Play Button Texture is NULL!" << endl; // Kiểm tra texture có bị null

    }
     // Vẽ nút Quit
    if (gQuitButtonTexture) {
         SDL_RenderCopy(gRenderer, gQuitButtonTexture, NULL, &quitButtonRect);
    }
}
void Game::close() {
    SDL_DestroyTexture(gGridTexture);
    SDL_DestroyTexture(gXTexture);
    SDL_DestroyTexture(gOTexture);
    gGridTexture = nullptr;
    gXTexture = nullptr;
    gOTexture = nullptr;
    SDL_DestroyTexture(gPlayButtonTexture);
    SDL_DestroyTexture(gQuitButtonTexture);
    gPlayButtonTexture = nullptr;
    gQuitButtonTexture = nullptr;
}
