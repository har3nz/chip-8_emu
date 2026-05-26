#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "chip8.h"

static unsigned const char font_set[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void initialize(struct chip8 *chip8){
    memset(chip8->memory, 0, sizeof(chip8->memory));
    clear_display(chip8);
    memset(chip8->V, 0, sizeof(chip8->V));
    memset(chip8->stack, 0, sizeof(chip8->stack));
    memset(chip8->keys, 0, sizeof(chip8->keys));
    memcpy(chip8->memory, font_set, sizeof(font_set));
    chip8->pc = 0x200;
    chip8->I = 0;
    chip8->opcode = 0;
    chip8->dt = 0;
    chip8->st = 0;
    chip8->sp = 0;
    chip8->draw_flag = false;



    srand(time(NULL));
}


bool load_rom(struct chip8 *chip8, char *file_name) {
    FILE *file = fopen(file_name, "rb");
    if (!file) {
        perror("Failed to open ROM");
        return false;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    if (size > (4096 - 0x200)) {
        fprintf(stderr, "ROM too large!\n");
        fclose(file);
        return false;
    }

    unsigned char *buffer = malloc((size + 1) * sizeof(unsigned char));
    fread(buffer, size, 1, file);
    

    for(int i = 0; i < size; i++) {
        chip8->memory[0x200 + i] = buffer[i];
    }
    fclose(file);
    free(buffer);
    
    return true;
}


void clear_display(struct chip8 *chip8){
    memset(chip8-> display, 0, sizeof(chip8->display));
    chip8->draw_flag = true;
}


void update_timers(struct chip8 *chip8){
    if(chip8->dt > 0)
        chip8->dt--;
    
    if(chip8->st > 0)
        chip8->st--;
}


void handle_input(struct chip8 *chip8, char key){

}


void decode_opcode(struct chip8 *chip8){
    int cur_pc = chip8->pc;
    chip8->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    chip8->pc += 2;
    
    printf("PC: %x | Opcode: %x\n", cur_pc, chip8->opcode);
    fflush(stdout);

    switch(chip8->opcode & 0xF000){
        case 0x0:
            switch(NNN(chip8->opcode)){
                case 0xE0:
                    clear_display(chip8);
                    break;
                
                case 0xEE:
                    chip8->sp--;
                    chip8->pc = chip8->stack[chip8->sp];
            }
            break;
        
        case 0x1000:
            chip8->pc = NNN(chip8->opcode);
            break;

        case 0x2000:
            chip8->stack[chip8->sp++] = chip8->pc;
            chip8->pc = NNN(chip8->opcode);
            break;
        
        case 0x3000:
            if (NN(chip8->opcode) == chip8->V[X(chip8->opcode)])
                chip8->pc += 2;
            break;
        
        case 0x4000:
            if (NN(chip8->opcode) != chip8->V[X(chip8->opcode)])
                chip8->pc += 2;
            break;

        case 0x5000:
            if(N(chip8->opcode) == 0){
                if(chip8->V[X(chip8->opcode)] == chip8->V[Y(chip8->opcode)])
                    chip8->pc += 2;
            }
            break;

        case 0x6000:
            chip8->V[X(chip8->opcode)] = NN(chip8->opcode);
            break;

        case 0x7000:
            chip8->V[X(chip8->opcode)] += NN(chip8->opcode);
            break;

        case 0x8000:
            switch(N(chip8->opcode)){
                case 0x0:
                    chip8->V[X(chip8->opcode)] = chip8->V[Y(chip8->opcode)];
                    break;

                case 0x1:
                    chip8->V[X(chip8->opcode)] |= chip8->V[Y(chip8->opcode)];
                    chip8->V[0xF] = 0;
                    break;  

                case 0x2:
                    chip8->V[X(chip8->opcode)] &= chip8->V[Y(chip8->opcode)];
                    chip8->V[0xF] = 0;
                    break;  

                case 0x3:
                    chip8->V[X(chip8->opcode)] ^= chip8->V[Y(chip8->opcode)];
                    chip8->V[0xF] = 0;
                    break;
                        
                case 0x4:{
                    uint16_t result = chip8->V[X(chip8->opcode)] + chip8->V[Y(chip8->opcode)];
                    chip8->V[X(chip8->opcode)] = result & 0xFF;
                    chip8->V[0xF] = result > 0xFF ? 1 : 0;
                    break;
                }
                        
                case 0x5:{
                    bool underflow = chip8->V[Y(chip8->opcode)] > chip8->V[X(chip8->opcode)];
                    chip8->V[X(chip8->opcode)] -= chip8->V[Y(chip8->opcode)];
                    if (underflow)
                        chip8->V[0xF] = 0;
                    else
                        chip8->V[0xF] = 1;
                    break;
                }
                        
                case 0x6:{
                    uint8_t vxbuff = chip8->V[X(chip8->opcode)];
                    chip8->V[X(chip8->opcode)] = vxbuff >> 1;
                    chip8->V[0xF] = vxbuff & 1;
                    break;
                }
                        
                case 0x7:{
                    uint8_t vx = chip8->V[X(chip8->opcode)];
                    uint8_t vy = chip8->V[Y(chip8->opcode)];
                    chip8->V[X(chip8->opcode)] = vy - vx;
                    chip8->V[0xF] = vy >= vx ? 1 : 0;
                    break;
                }
                        
                case 0xE:{
                    uint8_t vxbuff = chip8->V[X(chip8->opcode)];
                    chip8->V[X(chip8->opcode)] = vxbuff << 1;
                    chip8->V[0xF] = vxbuff >> 7;
                    break;
                }
                    
            }
            break;
        
        case 0x9000:
            if(N(chip8->opcode) == 0){
                if (chip8->V[X(chip8->opcode)] != chip8->V[Y(chip8->opcode)])
                    chip8->pc += 2;
            }
            break;
        
        case 0xA000:
            chip8->I = NNN(chip8->opcode);
            break;

        case 0xB000:
            chip8->pc = NNN(chip8->opcode) + chip8->V[0x0];
            break;

        case 0xC000:
            chip8->V[X(chip8->opcode)] = rand() & NN(chip8->opcode);
            break;

        case 0xD000:{
            uint8_t pos_x = chip8->V[X(chip8->opcode)] & 63;
            uint8_t pos_y = chip8->V[Y(chip8->opcode)] & 31;
            chip8->V[0xF] = 0;
            for(uint8_t y = 0; y < N(chip8->opcode); y++){
                if (pos_y + y > 31){
                    break;
                }

                int line = chip8->memory[chip8->I + y];

                for(uint8_t x = 0; x < 8; x++){
                    if(pos_x + x > 63){
                        break;
                    }
                    
                    if (((line >> (7 - x)) & 1) == 0) continue;
                    uint8_t* pixel = &chip8->display[pos_x + x][pos_y + y];
                    if (*pixel)
                        chip8->V[0xF] = 1;
                    *pixel ^= 1;
                }
            }
            break;
        }

        case 0xE000:
            switch(NN(chip8->opcode)){
                case 0x9E:
                    if (chip8->keys[chip8->V[X(chip8->opcode)]] == 1)
                        chip8->pc += 2;
                    break;
                
                case 0xA1:
                    if (chip8->keys[chip8->V[X(chip8->opcode)]] == 0)
                        chip8->pc += 2;
                    break;
            }
            break;

        case 0xF000:
            switch(NN(chip8->opcode)){
                case 0x07:
                    chip8->V[X(chip8->opcode)] = chip8->dt;
                    break;

                case 0x0A:{
                    bool key_pressed = false;
                    for (int i = 0; i < 16; i++){
                        if(chip8->keys[i]){
                            chip8->V[X(chip8->opcode)] = i;
                            key_pressed = true;
                            break;
                        }
                    }
                    if (!key_pressed)
                        chip8->pc -= 2;
                    break;
                }

                case 0x15:
                    chip8->dt = chip8->V[X(chip8->opcode)];
                    break;

                case 0x18:
                    chip8->st = chip8->V[X(chip8->opcode)];
                    break;

                case 0x1E:
                    chip8->I += chip8->V[X(chip8->opcode)];
                    break;
                    
                case 0x29:
                    chip8->I = chip8->V[X(chip8->opcode)] * 5;
                    break;
                    
                case 0x33:
                    chip8->memory[chip8->I] = chip8->V[X(chip8->opcode)] / 100;
                    chip8->memory[chip8->I + 1] = (chip8->V[X(chip8->opcode)] / 10) % 10;
                    chip8->memory[chip8->I + 2] = chip8->V[X(chip8->opcode)] % 10;
                    break;
                    
                case 0x55:
                    for(int i = 0; i <= X(chip8->opcode); i++)
                        chip8->memory[chip8->I + i] = chip8->V[i];
                    chip8->I += X(chip8->opcode) + 1;
                    break;

                case 0x65:
                    for(int i = 0; i <= X(chip8->opcode); i++)
                        chip8->V[i] = chip8->memory[chip8->I + i];
                    chip8->I += X(chip8->opcode) + 1;
                    break;
            }
            break;
    }
}