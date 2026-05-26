#include <stdbool.h>
#include <stdint.h>

#define NNN(x) (x & 0xFFF)
#define NN(x) (x & 0xFF)
#define N(x) (x & 0xF)
#define X(x) ((x & 0xF00) >> 8)
#define Y(x) ((x & 0xF0) >> 4)


struct chip8{
    uint8_t memory[4096];
    uint8_t display[64][32];
    uint8_t V[16];
    uint8_t stack[16];
    uint8_t keys[16];
    uint16_t opcode;
    uint16_t I;
    uint16_t pc;
    uint8_t dt;
    uint8_t st;
    uint8_t sp;
    bool draw_flag;
};

void initialize(struct chip8 *chip8);
bool load_rom(struct chip8 *chip8, char *file_name);
void clear_display(struct chip8 *chip8);
void update_timers(struct chip8 *chip8);
void decode_opcode(struct chip8 *chip8);