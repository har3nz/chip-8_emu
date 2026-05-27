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
                    switch (event.key.keysym.scancode) {
                        case SDL_SCANCODE_ESCAPE:
                            running = false;
                            break;

                        case SDL_SCANCODE_1: handle_input(&chip8, KEY_1, true); break;
                        case SDL_SCANCODE_2: handle_input(&chip8, KEY_2, true); break;
                        case SDL_SCANCODE_3: handle_input(&chip8, KEY_3, true); break;
                        case SDL_SCANCODE_4: handle_input(&chip8, KEY_C, true); break;

                        case SDL_SCANCODE_Q: handle_input(&chip8, KEY_4, true); break;
                        case SDL_SCANCODE_W: handle_input(&chip8, KEY_5, true); break;
                        case SDL_SCANCODE_E: handle_input(&chip8, KEY_6, true); break;
                        case SDL_SCANCODE_R: handle_input(&chip8, KEY_D, true); break;

                        case SDL_SCANCODE_A: handle_input(&chip8, KEY_7, true); break;
                        case SDL_SCANCODE_S: handle_input(&chip8, KEY_8, true); break;
                        case SDL_SCANCODE_D: handle_input(&chip8, KEY_9, true); break;
                        case SDL_SCANCODE_F: handle_input(&chip8, KEY_E, true); break;

                        case SDL_SCANCODE_Z: handle_input(&chip8, KEY_A, true); break;
                        case SDL_SCANCODE_X: handle_input(&chip8, KEY_0, true); break;
                        case SDL_SCANCODE_C: handle_input(&chip8, KEY_B, true); break;
                        case SDL_SCANCODE_V: handle_input(&chip8, KEY_F, true); break;
                        default: break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_1: handle_input(&chip8, KEY_1, false); break;
                        case SDL_SCANCODE_2: handle_input(&chip8, KEY_2, false); break;
                        case SDL_SCANCODE_3: handle_input(&chip8, KEY_3, false); break;
                        case SDL_SCANCODE_4: handle_input(&chip8, KEY_C, false); break;

                        case SDL_SCANCODE_Q: handle_input(&chip8, KEY_4, false); break;
                        case SDL_SCANCODE_W: handle_input(&chip8, KEY_5, false); break;
                        case SDL_SCANCODE_E: handle_input(&chip8, KEY_6, false); break;
                        case SDL_SCANCODE_R: handle_input(&chip8, KEY_D, false); break;

                        case SDL_SCANCODE_A: handle_input(&chip8, KEY_7, false); break;
                        case SDL_SCANCODE_S: handle_input(&chip8, KEY_8, false); break;
                        case SDL_SCANCODE_D: handle_input(&chip8, KEY_9, false); break;
                        case SDL_SCANCODE_F: handle_input(&chip8, KEY_E, false); break;

                        case SDL_SCANCODE_Z: handle_input(&chip8, KEY_A, false); break;
                        case SDL_SCANCODE_X: handle_input(&chip8, KEY_0, false); break;
                        case SDL_SCANCODE_C: handle_input(&chip8, KEY_B, false); break;
                        case SDL_SCANCODE_V: handle_input(&chip8, KEY_F, false); break;
                        default: break;
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