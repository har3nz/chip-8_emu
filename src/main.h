#include <SDL2/SDL.h>
#include <stdbool.h>

struct window{
    SDL_Window* window;
    SDL_Renderer* renderer;
};

bool sdl_initialize(struct window *window);