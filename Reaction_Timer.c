// It's pretty self-explainatory, but when the screen goes green press the space bar to mesure your reaction speed.
// It's not very acurate though, as the first time it is much faster and the timing can go very high, but as you go it gets slower.
// You can run these on Linux at https://github.com/OllieOlzu/Easy-32-bit-C-kernel-to-ISO.



#include <stdint.h>

#define V ((uint16_t*)0xB8000)
#define inb(p) ({ uint8_t r; __asm__ volatile ("inb %1, %0" : "=a"(r) : "Nd"(p)); r; })
#define HIGHT 24
#define LENTH 80

void display(int X, int Y, uint8_t L, uint8_t fg, uint8_t bg) {
    int y = (HIGHT / 2) - Y;
    int x = (LENTH / 2) + X;
    uint8_t attr = (bg << 4) | (fg & 0x0F);
    V[LENTH * y + x] = ((uint16_t)attr << 8) | L;
}

void fill(uint8_t L, uint8_t fg, uint8_t bg) {
    uint8_t attr = (bg << 4) | (fg & 0x0F);
    uint16_t val = ((uint16_t)attr << 8) | L;
    for (int i = 0; i < LENTH * (HIGHT + 1); i++) {
        V[i] = val;
    }
}

void line(char D, int X, int Y, int S, uint8_t L, uint8_t fg, uint8_t bg) {
    if (D == 'X') {
        for (int i = 0; i < S; i++) {
            display(X + i, Y, L, fg, bg);
        }
    } else if (D == 'Y') {
        for (int i = 0; i < S; i++) {
            display(X, Y + i, L, fg, bg);
        }
    }
}

void rec(int X, int Y, int XS, int YS, uint8_t L, uint8_t fg, uint8_t bg) {
    for (int i = 0; i < YS; i++) {
            line('X', X, Y - i, XS, L, fg, bg);
    }
}

void print(int X, int Y, const char* word, uint8_t fg, uint8_t bg) {
    for (int i = 0; word[i] != '\0'; i++) {
        display(X + i, Y, word[i], fg, bg);
    }
}

void drawLogo(int X, int Y, uint8_t fg, uint8_t bg) { // Hehe ascii art I couldnt resist
    print(X, Y - 0, " _____", fg, bg); // Show message
    print(X, Y - 1, "| ___ |", fg, bg); // Show message
    print(X, Y - 2, "||   ||", fg, bg); // Show message
    print(X, Y - 3, "||___||", fg, bg); // Show message
    print(X, Y - 4, "|_____|", fg, bg); // Show message
    print(X, Y - 5, "| |", fg, bg); // Show message
    print(X, Y - 6, "| |___", fg, bg); // Show message
    print(X, Y - 7, "|_____|", fg, bg); // Show message
    print(X, Y - 8, "| |", fg, bg); // Show message
    print(X, Y - 9, "| |___", fg, bg); // Show message
    print(X, Y - 10, "|_____|", fg, bg); // Show message
    print(X, Y - 11, "|__ __|", fg, bg); // Show message
    print(X, Y - 12, "  | |  ", fg, bg); // Show message
    print(X, Y - 13, " _| |_ ", fg, bg); // Show message
    print(X, Y - 14, "|_____|", fg, bg); // Show message
    print(X, Y - 15, "|  ___|", fg, bg); // Show message
    print(X, Y - 16, "| |___ ", fg, bg); // Show message
    print(X, Y - 17, "| ____|", fg, bg); // Show message
    print(X, Y - 18, "| |___ ", fg, bg); // Show message
    print(X, Y - 19, "|_____|", fg, bg); // Show message
    
}

void itoa(int num, char* buffer) { // Yeah I got this function from chatGPT, I don't know what it really does but it works
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) buffer[i++] = '-';

    buffer[i] = '\0';

    // Reverse the buffer
    for (int j = 0; j < i / 2; j++) {
        char tmp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = tmp;
    }
}

void kernel_main() {

    int StartColor = 0x04;
    int MidColor = 0x02;
    int EndColor = 0x03;
    int i;

    int key = 0x39; // Key code for A but you can change this
    
    fill(219, StartColor, 0x00);
    drawLogo(27, 10, 0x0F, StartColor);

    print(-15, 0, "When screen turns green press space...", 0x0F, StartColor); // Show message

    for (volatile int i = 0; i < 1000000000; i++);

    fill(219, MidColor, 0x00);
    drawLogo(27, 10, 0x0F, MidColor);

    print(0, 0, "Now!", 0x0F, MidColor); // Show message

    while (inb(0x60) != key) {
        i++;
    }

    fill(219, EndColor, 0x00);

    drawLogo(27, 10, 0x0F, EndColor);

    char scoreText[12];
    itoa(i / 10000, scoreText); // Convert score to string
    print(-5, 1, "You got ", 0x0F, EndColor); // Show it
    print(3, 1, scoreText, 0x0F, EndColor); // Show it
    print(-7, 0, "X 10,000 clock cycles!", 0x0F, EndColor); // Show it

    print(-9, -1, "Press space bar to restart!", 0x0F, EndColor); // Show it

    while (inb(0x60) == key); //Wait for space bar to be released

    while (inb(0x60) != key); //Wait for space bar

    kernel_main(); // Restart the game

}
