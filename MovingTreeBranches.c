//Use the A key to move the branches down the tree. 
//This is my my test for a bigger project I'm working on for a tree chopping game, that is simalar to Bob and chainsaw. 
//You can run these on Linux at https://github.com/OllieOlzu/Easy-32-bit-C-kernel-to-ISO.

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
    for (int i = 0; i < LENTH * HIGHT; i++) {
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

void drawBranch(char D, int spot) {
    int Y = (spot * 6) - 13;
    if (D == 'L') {
        rec(-20, Y, 17, 2, 219, 0x06, 0x00); // Draw wood
        rec(-20, Y + 1, 8, 4, 219, 0x02, 0x00); // Draw leaves
    }

    if (D == 'R') {
        rec(4, Y, 17, 2, 219, 0x06, 0x00); // Draw wood
        rec(13, Y + 1, 8, 4, 219, 0x02, 0x00); // Draw leaves
    }
}

void eraceBranch(char D, int spot) {
    int Y = (spot * 6) - 13;
    if (D == 'L') {
        rec(-20, Y, 17, 2, 219, 0x01, 0x00); // Erace wood
        rec(-20, Y + 1, 8, 4, 219, 0x01, 0x00); // Erace leaves
    }

    if (D == 'R') {
        rec(4, Y, 17, 2, 219, 0x01, 0x00); // Erace wood
        rec(13, Y + 1, 8, 4, 219, 0x01, 0x00); // Erace leaves
    }
}

void delay(int count) {
    for (volatile int i = 0; i < count; i++);
}


void kernel_main() {
    int Branches[5] = {'R', 'L', 'R', 'R', 'L'}; //Which sides branches are on

    int length = sizeof(Branches) / sizeof(Branches[0]); // Find lenth of branches array
    
    fill(219, 0x01, 0x00);    // Clear screen with blue

    rec(-3, 12, 7, 25, 219, 0x06, 0x00); // Draw trunk
    rec(-40, -11, 80, 2, 219, 0x02, 0x00); // Draw ground

    int F = 1; // Branch level starts at 1
    int key = 0x1E; // Key code for A but you can change this

    while (1) { // forever loop

        while (inb(0x60) != key); // Wait for key press, currently set to A

        for (int i = 0; i < length; i++) { // Loop through branches
            eraceBranch(Branches[i], F + (1 + i)); // Erace previous branch
            drawBranch(Branches[i], F + i); // Draw next branch
            rec(-40, -11, 80, 2, 219, 0x02, 0x00); // Draw ground again to not show any eracements
        }
        
        while (inb(0x60) == key); // Wait for key release
        
        F--; // Decrease branch level

        

    }
}

