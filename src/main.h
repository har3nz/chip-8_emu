#include <SDL3/SDL.h>
#include <stdbool.h>

struct window{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_AudioStream* stream;
};

bool sdl_initialize(struct window *window);