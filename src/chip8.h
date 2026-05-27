#include <stdbool.h>
#include <stdint.h>

#define NNN(x) (x & 0xFFF)
#define NN(x) (x & 0xFF)
#define N(x) (x & 0xF)
#define X(x) ((x & 0xF00) >> 8)
#define Y(x) ((x & 0xF0) >> 4)

enum chip_key {
  KEY_0 = 0x0,
  KEY_1 = 0x1,
  KEY_2 = 0x2,
  KEY_3 = 0x3,
  KEY_4 = 0x4,
  KEY_5 = 0x5,
  KEY_6 = 0x6,
  KEY_7 = 0x7,
  KEY_8 = 0x8,
  KEY_9 = 0x9,
  KEY_A = 0xA,
  KEY_B = 0xB,
  KEY_C = 0xC,
  KEY_D = 0xD,
  KEY_E = 0xE,
  KEY_F = 0xF
};

struct chip8{
    uint16_t stack[16];
    uint8_t memory[4096];
    uint8_t display[64][32];
    uint8_t V[16];
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
void handle_input(struct chip8 *chip8, enum chip_key key, bool is_down);
void clear_display(struct chip8 *chip8);
void update_timers(struct chip8 *chip8);
void decode_opcode(struct chip8 *chip8);