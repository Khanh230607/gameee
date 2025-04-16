#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>

class TextureManager {
public:
    // Hàm static để load texture
    static SDL_Texture* LoadTexture(const std::string &filePath, SDL_Renderer* renderer);

    // Hàm static để vẽ texture (đơn giản)
    static void Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_Renderer* renderer);

     // Hàm static để vẽ toàn bộ texture tại vị trí (x, y)
    static void Draw(SDL_Texture* tex, int x, int y, SDL_Renderer* renderer);
};

#endif // TEXTUREMANAGER_H
