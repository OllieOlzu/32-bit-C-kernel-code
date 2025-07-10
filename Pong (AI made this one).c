#include <stdint.h>
#include <stddef.h>

// VGA text mode constants
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

// Colors
#define COLOR_BLACK 0
#define COLOR_WHITE 0xF
#define COLOR_GREEN 0x2

// Game constants
#define PADDLE_HEIGHT 5
#define BALL_SPEED 1

// Forward declarations
void itoa(int num, char* str, int base);
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t val);

// VGA text mode buffer
volatile uint16_t* vga_buffer = (uint16_t*)VGA_ADDRESS;

// Game state
int player_paddle = VGA_HEIGHT/2 - PADDLE_HEIGHT/2;
int enemy_paddle = VGA_HEIGHT/2 - PADDLE_HEIGHT/2;
int ball_x = VGA_WIDTH/2;
int ball_y = VGA_HEIGHT/2;
int ball_dx = BALL_SPEED;
int ball_dy = BALL_SPEED;
int player_score = 0;
int enemy_score = 0;

// Clear screen
void clear_screen(uint8_t color) {
    uint16_t blank = ' ' | (color << 8);
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = blank;
    }
}

// Draw a character at position
void put_char(char c, uint8_t color, size_t x, size_t y) {
    vga_buffer[y * VGA_WIDTH + x] = c | (color << 8);
}

// Draw the game elements
void draw_game() {
    // Clear screen
    clear_screen(COLOR_BLACK);
    
    // Draw paddles
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        put_char('|', COLOR_WHITE, 1, player_paddle + i);
        put_char('|', COLOR_WHITE, VGA_WIDTH - 2, enemy_paddle + i);
    }
    
    // Draw ball
    put_char('O', COLOR_WHITE, ball_x, ball_y);
    
    // Draw scores
    char score_str[20];
    itoa(player_score, score_str, 10);
    for (int i = 0; score_str[i]; i++) {
        put_char(score_str[i], COLOR_WHITE, VGA_WIDTH/2 - 5 + i, 1);
    }
    
    put_char('-', COLOR_WHITE, VGA_WIDTH/2, 1);
    
    itoa(enemy_score, score_str, 10);
    for (int i = 0; score_str[i]; i++) {
        put_char(score_str[i], COLOR_WHITE, VGA_WIDTH/2 + 1 + i, 1);
    }
}

// Simple itoa implementation
void itoa(int num, char* str, int base) {
    int i = 0;
    int is_negative = 0;
    
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }
    
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    
    if (is_negative) {
        str[i++] = '-';
    }
    
    str[i] = '\0';
    
    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

// Keyboard input handling
void handle_keyboard() {
    // Read keyboard status port
    unsigned char status = inb(0x64);
    
    if (status & 0x1) {
        unsigned char keycode = inb(0x60);
        
        // Handle key presses
        if (keycode == 0x11) { // W key
            if (player_paddle > 1) player_paddle--;
        } else if (keycode == 0x1F) { // S key
            if (player_paddle < VGA_HEIGHT - PADDLE_HEIGHT - 1) player_paddle++;
        }
    }
}

// Port I/O functions
uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Update game state
void update_game() {
    // Move ball
    ball_x += ball_dx;
    ball_y += ball_dy;
    
    // Ball collision with top and bottom
    if (ball_y <= 0 || ball_y >= VGA_HEIGHT - 1) {
        ball_dy = -ball_dy;
    }
    
    // Ball collision with paddles
    if (ball_x <= 2) {
        if (ball_y >= player_paddle && ball_y < player_paddle + PADDLE_HEIGHT) {
            ball_dx = -ball_dx;
            // Add some randomness to the bounce
            if (ball_y < player_paddle + PADDLE_HEIGHT/3) ball_dy = -BALL_SPEED;
            else if (ball_y > player_paddle + 2*PADDLE_HEIGHT/3) ball_dy = BALL_SPEED;
            else ball_dy = 0;
        } else {
            // Player missed
            enemy_score++;
            ball_x = VGA_WIDTH/2;
            ball_y = VGA_HEIGHT/2;
            ball_dx = BALL_SPEED;
            ball_dy = BALL_SPEED;
        }
    }
    
    if (ball_x >= VGA_WIDTH - 3) {
        if (ball_y >= enemy_paddle && ball_y < enemy_paddle + PADDLE_HEIGHT) {
            ball_dx = -ball_dx;
            // Add some randomness to the bounce
            if (ball_y < enemy_paddle + PADDLE_HEIGHT/3) ball_dy = -BALL_SPEED;
            else if (ball_y > enemy_paddle + 2*PADDLE_HEIGHT/3) ball_dy = BALL_SPEED;
            else ball_dy = 0;
        } else {
            // Enemy missed
            player_score++;
            ball_x = VGA_WIDTH/2;
            ball_y = VGA_HEIGHT/2;
            ball_dx = -BALL_SPEED;
            ball_dy = BALL_SPEED;
        }
    }
    
    // Simple AI for enemy paddle
    if (enemy_paddle + PADDLE_HEIGHT/2 < ball_y && enemy_paddle < VGA_HEIGHT - PADDLE_HEIGHT - 1) {
        enemy_paddle++;
    } else if (enemy_paddle + PADDLE_HEIGHT/2 > ball_y && enemy_paddle > 1) {
        enemy_paddle--;
    }
}

// Kernel main function
void kernel_main() {
    clear_screen(COLOR_BLACK);
    
    // Main game loop
    while (1) {
        handle_keyboard();
        update_game();
        draw_game();
        
        // Simple delay
        for (volatile int i = 0; i < 10000000; i++);
    }
}
