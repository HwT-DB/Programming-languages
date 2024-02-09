#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <openssl/sha.h>

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 300

#define SHA256_DIGEST_LENGTH 32

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* inputTexture = NULL;

void calculate_hash(const char* text) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(text, strlen(text), hash);

    char hex_hash[2 * SHA256_DIGEST_LENGTH + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hex_hash + 2 * i, "%02x", hash[i]);
    }
    hex_hash[2 * SHA256_DIGEST_LENGTH] = '\0';

    printf("SHA-256 Hash: %s\n", hex_hash);
}

void handle_input(SDL_Event* event) {
    if (event->type == SDL_QUIT) {
        SDL_Quit();
        exit(0);
    } else if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_RETURN) {
            char* text = NULL;
            SDL_Surface* surface = SDL_GetWindowSurface(window);
            SDL_Rect rect;
            SDL_GetClipRect(surface, &rect);
            rect.h -= 25; // adjust height to accommodate button
            SDL_RenderReadPixels(renderer, &rect, SDL_PIXELFORMAT_RGBA8888, text, rect.w * 4);
            calculate_hash(text);
            free(text);
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("SHA-256 Hash Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Rect inputRect = {50, 50, 300, 25};
    inputTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, inputRect.w, inputRect.h);

    SDL_Event event;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            handle_input(&event);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(inputTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}