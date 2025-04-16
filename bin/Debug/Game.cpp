
#include "Game.h"
#include "TextureManager.h" // Bao gồm TextureManager
#include "Defines.h"
#include <iostream>

Game::Game() : gWindow(nullptr), gRenderer(nullptr), gGridTexture(nullptr),
               gXTexture(nullptr), gOTexture(nullptr),
               currentPlayer(PLAYER_X), gameState(STATE_PLAYING), isRunning(false)
{
    // Khởi tạo bàn cờ trống
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            board[i][j] = CELL_EMPTY;
        }
    }
}

Game::~Game() {
    // Hàm hủy không cần gọi close() trực tiếp ở đây
    // close() sẽ được gọi rõ ràng khi kết thúc game
}

bool Game::init(const char* title, int width, int height) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Khởi tạo SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Tạo cửa sổ
    gWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (gWindow == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    // Tạo renderer
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(gWindow);
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    // Đặt màu vẽ mặc định (không bắt buộc)
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF); // Màu trắng

    // Load media
    if (!loadMedia()) {
        close(); // Dọn dẹp nếu load media thất bại
        return false;
    }

    isRunning = true; // Sẵn sàng chạy vòng lặp game
    return true;
}

bool Game::loadMedia() {
    // Đường dẫn tương đối đến thư mục assets
    gGridTexture = TextureManager::LoadTexture("assets/grid.png", gRenderer);
    gXTexture = TextureManager::LoadTexture("assets/x.png", gRenderer);
    gOTexture = TextureManager::LoadTexture("assets/o.png", gRenderer);

    if (gGridTexture == nullptr || gXTexture == nullptr || gOTexture == nullptr) {
        std::cerr << "Failed to load one or more textures!" << std::endl;
        return false;
    }
    return true;
}

void Game::run() {
    while (isRunning) {
        handleEvents();
        // update(); // Không cần update phức tạp cho game này
        render();
    }
}

void Game::handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        // Người dùng yêu cầu thoát
        if (e.type == SDL_QUIT) {
            isRunning = false;
            gameState = STATE_QUIT; // Đánh dấu là thoát
        }

        // Xử lý click chuột
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            // Nếu game đang diễn ra
            if (gameState == STATE_PLAYING) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // Chuyển đổi tọa độ chuột sang chỉ số ô (row, col)
                int clickedRow = mouseY / CELL_SIZE;
                int clickedCol = mouseX / CELL_SIZE;

                // Đặt quân cờ nếu hợp lệ
                if (placeMark(clickedRow, clickedCol)) {
                    // Kiểm tra thắng sau khi đặt
                    int winResult = checkWin();
                    if (winResult != STATE_PLAYING) {
                        gameState = winResult; // Cập nhật trạng thái thắng
                    } else if (checkDraw()) { // Kiểm tra hòa nếu chưa ai thắng
                        gameState = STATE_DRAW;
                    } else {
                        // Nếu chưa kết thúc, chuyển lượt
                        switchPlayer();
                    }
                }
            }
            // Nếu game đã kết thúc (thắng hoặc hòa) và người dùng click
            else if (gameState == STATE_X_WINS || gameState == STATE_O_WINS || gameState == STATE_DRAW) {
                 // Cho phép click để chơi lại
                 resetGame();
            }
        }
         // Có thể thêm xử lý nhấn phím (ví dụ: R để reset)
        // if (e.type == SDL_KEYDOWN) {
        //     if (e.key.keysym.sym == SDLK_r) {
        //         resetGame();
        //     }
        // }
    }
}


bool Game::placeMark(int row, int col) {
    // Kiểm tra ô có hợp lệ và còn trống không
    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE && board[row][col] == CELL_EMPTY) {
        board[row][col] = currentPlayer;
        return true; // Đặt thành công
    }
    return false; // Ô không hợp lệ hoặc đã có quân
}

int Game::checkWin() {
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
     // Đặt lại bàn cờ
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            board[i][j] = CELL_EMPTY;
        }
    }
    // Đặt lại người chơi và trạng thái
    currentPlayer = PLAYER_X; // X luôn đi trước trong ván mới
    gameState = STATE_PLAYING;
    std::cout << "Game Reset!" << std::endl; // Thông báo trên console
}


void Game::render() {
    // Xóa màn hình về màu trắng
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(gRenderer);

    // Vẽ lưới
    if (gGridTexture) {
        TextureManager::Draw(gGridTexture, 0, 0, gRenderer);
    }

    // Vẽ các quân X và O
    renderBoard();

    // Vẽ trạng thái game (sẽ cần cải thiện với SDL_ttf)
    renderStatus();

    // Cập nhật màn hình
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
    // Đây là phần đơn giản, chỉ in ra console
    // Để hiển thị trên cửa sổ SDL, bạn cần dùng thư viện SDL_ttf
    // Hoặc tạo các texture chứa chữ sẵn (ví dụ: "X Wins!", "O Wins!", "Draw!", "Click to Play Again")
    // và vẽ chúng khi game kết thúc.

    switch (gameState) {
        case STATE_X_WINS:
            // Vẽ texture "X Wins!" hoặc "Click to Play Again"
            // std::cout << "X Wins! Click to play again." << std::endl; // Tạm thời in console
            // Ví dụ nếu có texture gXWinsTexture:
            // TextureManager::Draw(gXWinsTexture, SCREEN_WIDTH / 2 - w/2, SCREEN_HEIGHT / 2 - h/2, gRenderer);
            break;
        case STATE_O_WINS:
             // Vẽ texture "O Wins!" hoặc "Click to Play Again"
             // std::cout << "O Wins! Click to play again." << std::endl;
            break;
        case STATE_DRAW:
             // Vẽ texture "Draw!" hoặc "Click to Play Again"
             // std::cout << "Draw! Click to play again." << std::endl;
            break;
        case STATE_PLAYING:
            // Có thể hiển thị lượt của ai nếu muốn (cần SDL_ttf)
            // std::cout << (currentPlayer == PLAYER_X ? "X's Turn" : "O's Turn") << std::endl;
            break;
        default:
            break; // Không làm gì khi đang thoát
    }
     // Tạm thời: In console để dễ debug trạng thái
    static int lastPrintedState = -1; // Chỉ in khi trạng thái thay đổi
    if (gameState != lastPrintedState && gameState != STATE_PLAYING && gameState != STATE_QUIT) {
        if(gameState == STATE_X_WINS) std::cout << "X Wins! Click to play again." << std::endl;
        else if(gameState == STATE_O_WINS) std::cout << "O Wins! Click to play again." << std::endl;
        else if(gameState == STATE_DRAW) std::cout << "Draw! Click to play again." << std::endl;
        lastPrintedState = gameState;
    } else if (gameState == STATE_PLAYING && lastPrintedState != STATE_PLAYING) {
        lastPrintedState = gameState; // Reset khi bắt đầu chơi lại
    }

}

void Game::close() {
    std::cout << "Closing game..." << std::endl;
    // Giải phóng textures
    SDL_DestroyTexture(gGridTexture);
    SDL_DestroyTexture(gXTexture);
    SDL_DestroyTexture(gOTexture);
    gGridTexture = nullptr;
    gXTexture = nullptr;
    gOTexture = nullptr;
    // Thêm các texture khác nếu có

    // Hủy renderer và cửa sổ
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gRenderer = nullptr;
    gWindow = nullptr;

    // Thoát khỏi các hệ thống con của SDL
    IMG_Quit();
    SDL_Quit();
     std::cout << "Game closed." << std::endl;
}
