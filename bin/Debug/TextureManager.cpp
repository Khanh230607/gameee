
#include "TextureManager.h"
#include <iostream> // Để in lỗi

SDL_Texture* TextureManager::LoadTexture(const std::string &filePath, SDL_Renderer* renderer) {
    SDL_Texture* texture = nullptr;
    texture = IMG_LoadTexture(renderer, filePath.c_str());
    if (texture == nullptr) {
        std::cerr << "Failed to load texture: " << filePath << "! SDL_image Error: " << IMG_GetError() << std::endl;
    }
    return texture;
}

void TextureManager::Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, tex, &src, &dest);
}

void TextureManager::Draw(SDL_Texture* tex, int x, int y, SDL_Renderer* renderer) {
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;
    // Lấy kích thước gốc của texture
    SDL_QueryTexture(tex, NULL, NULL, &destRect.w, &destRect.h);
    SDL_RenderCopy(renderer, tex, NULL, &destRect); // Vẽ toàn bộ texture (src = NULL)
}
