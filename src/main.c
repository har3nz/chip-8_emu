#include "main.h"
#include "chip8.h"
#include <stdio.h>

int main(int argc, char *argv[]){

    if (argc == 0){
        return 1;
    }
    struct window window;
    struct chip8 chip8;
    bool running = true;

    initialize(&chip8);

    load_rom(&chip8, argv[1]);
    

    if (!sdl_initialize(&window)) {
        fprintf(stderr, "Failed to initialize SDL\n");
        running = false;
    }


    SDL_Texture* texture = SDL_CreateTexture(window.renderer, 
                                        SDL_PIXELFORMAT_RGBA8888, 
                                        SDL_TEXTUREACCESS_STREAMING, 
                                        64, 32);

    while(running){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                            
                        default:
                            break;
                    }
                default:
                    break;
            }
        }

        update_timers(&chip8);

        for (int i = 0; i < 11; i++)
            decode_opcode(&chip8);

        SDL_SetRenderDrawColor(window.renderer, 0, 0, 0, 250);
        SDL_RenderClear(window.renderer);
        
        if(chip8.draw_flag){
            uint32_t *pixels = NULL;
            int pitch = 0;

            SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch) ;
            for (int y = 0; y < 32; y++){
                for(int x = 0; x < 64; x++){
                    uint32_t color = chip8.display[x][y] ?  0xFFFFFFFF : 0x00000000;
                    pixels[y * (pitch / sizeof(uint32_t)) + x] = color;
                    //printf("0x%08X\n", chip8.display[x][y]);
                }
            }
            SDL_UnlockTexture(texture);
        }

        SDL_RenderCopy(window.renderer, texture, NULL, NULL);
        SDL_RenderPresent(window.renderer);

        SDL_Delay(16);
    }

    SDL_DestroyWindow(window.window);
    SDL_DestroyRenderer(window.renderer);
    SDL_Quit();


    return 0;
}


bool sdl_initialize(struct window *window) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    window->window = SDL_CreateWindow("Chip-8 Emulator",
                                      SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED,
                                      640, 320,
                                      SDL_WINDOW_SHOWN);
    if (!window->window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED);
    if (!window->renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window->window);
        SDL_Quit();
        return false;
    }

    return true;
}