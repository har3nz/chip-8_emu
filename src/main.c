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
                                    
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    static int current_sine_sample = 0;
    
    const int minimum_audio = (8000 * sizeof (float)) / 2;

    while(running){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type){
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    switch (event.key.key) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;

                        case SDLK_1: handle_input(&chip8, KEY_1, true); break;
                        case SDLK_2: handle_input(&chip8, KEY_2, true); break;
                        case SDLK_3: handle_input(&chip8, KEY_3, true); break;
                        case SDLK_4: handle_input(&chip8, KEY_C, true); break;

                        case SDLK_Q: handle_input(&chip8, KEY_4, true); break;
                        case SDLK_W: handle_input(&chip8, KEY_5, true); break;
                        case SDLK_E: handle_input(&chip8, KEY_6, true); break;
                        case SDLK_R: handle_input(&chip8, KEY_D, true); break;

                        case SDLK_A: handle_input(&chip8, KEY_7, true); break;
                        case SDLK_S: handle_input(&chip8, KEY_8, true); break;
                        case SDLK_D: handle_input(&chip8, KEY_9, true); break;
                        case SDLK_F: handle_input(&chip8, KEY_E, true); break;

                        case SDLK_Z: handle_input(&chip8, KEY_A, true); break;
                        case SDLK_X: handle_input(&chip8, KEY_0, true); break;
                        case SDLK_C: handle_input(&chip8, KEY_B, true); break;
                        case SDLK_V: handle_input(&chip8, KEY_F, true); break;
                        default: break;
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    switch(event.key.key){
                        case SDLK_1: handle_input(&chip8, KEY_1, false); break;
                        case SDLK_2: handle_input(&chip8, KEY_2, false); break;
                        case SDLK_3: handle_input(&chip8, KEY_3, false); break;
                        case SDLK_4: handle_input(&chip8, KEY_C, false); break;

                        case SDLK_Q: handle_input(&chip8, KEY_4, false); break;
                        case SDLK_W: handle_input(&chip8, KEY_5, false); break;
                        case SDLK_E: handle_input(&chip8, KEY_6, false); break;
                        case SDLK_R: handle_input(&chip8, KEY_D, false); break;

                        case SDLK_A: handle_input(&chip8, KEY_7, false); break;
                        case SDLK_S: handle_input(&chip8, KEY_8, false); break;
                        case SDLK_D: handle_input(&chip8, KEY_9, false); break;
                        case SDLK_F: handle_input(&chip8, KEY_E, false); break;

                        case SDLK_Z: handle_input(&chip8, KEY_A, false); break;
                        case SDLK_X: handle_input(&chip8, KEY_0, false); break;
                        case SDLK_C: handle_input(&chip8, KEY_B, false); break;
                        case SDLK_V: handle_input(&chip8, KEY_F, false); break;
                        default: break;
                    }
                default:
                    break;
            }
        }

        update_timers(&chip8, &window);

        for (int i = 0; i < 11; i++)
            decode_opcode(&chip8);

        if(chip8.sp > 0){
            if (SDL_GetAudioStreamQueued(window.stream) < minimum_audio) {
                static float samples[512];
                int i;

                for (i = 0; i < SDL_arraysize(samples); i++) {
                    const int freq = 440;
                    const float phase = current_sine_sample * freq / 8000.0f;
                    samples[i] = SDL_sinf(phase * 2 * SDL_PI_F);
                    current_sine_sample++;
                }

                current_sine_sample %= 8000;

                SDL_PutAudioStreamData(window.stream, samples, sizeof (samples));
            }
        }
        

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
                }
            }
            SDL_UnlockTexture(texture);
        }

        SDL_RenderTexture(window.renderer, texture, NULL, NULL);

        SDL_RenderPresent(window.renderer);

        SDL_Delay(16);
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_DestroyWindow(window.window);
    SDL_DestroyRenderer(window.renderer);
    SDL_Quit();


    return 0;
}


bool sdl_initialize(struct window *window) {
    SDL_AudioSpec spec;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return false;
    }

    window->window = SDL_CreateWindow("Chip-8 Emulator",
                                      640, 320,
                                      SDL_WINDOW_BORDERLESS);
    if (!window->window) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    window->renderer = SDL_CreateRenderer(window->window, NULL);
    if (!window->renderer) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window->window);
        SDL_Quit();
        return false;
    }

    spec.channels = 1;
    spec.format = SDL_AUDIO_F32;
    spec.freq = 8000;
    window->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!window->stream) {
        SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_ResumeAudioStreamDevice(window->stream);


    return true;
}