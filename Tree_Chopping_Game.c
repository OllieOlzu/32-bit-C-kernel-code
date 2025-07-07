// Use the A and D keys to move the branches down the tree. 
// This is a game that I took WAY too long coding and is quite simalar to Bob and chainsaw. 
// You can run these on Linux at https://github.com/OllieOlzu/Easy-32-bit-C-kernel-to-ISO.


#include <stdint.h>

#define V ((uint16_t*)0xB8000)
#define inb(p) ({ uint8_t r; __asm__ volatile ("inb %1, %0" : "=a"(r) : "Nd"(p)); r; })
#define HIGHT 24
#define LENTH 80
int score = 0; // Initialize score
int F; // Global variable for branch level


char Branches[50];
unsigned int rand_seed = 12345;

char random_branch() {
    rand_seed = rand_seed * 1103515245 + 12345;
    return (rand_seed >> 16) & 1 ? 'R' : 'L';
}

void init_branches() {
    for (int i = 0; i < 50; i++) {
        Branches[i] = random_branch();
    }
}

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

void drawBranch(char D, int spot) {
    if (spot < 1 || spot > 5) return; // Ensure spot is within valid range
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
    if (spot < 1 || spot > 5) return; // Ensure spot is within valid range
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

void eraceBottom() {
    rec(-20, -4, 41, 7, 219, 0x01, 0x00); // Erace wood
    rec(-3, 12, 7, 25, 219, 0x06, 0x00); // redraw trunk
}

void delay(int count) {
    for (volatile int i = 0; i < count; i++);
}

void print(int X, int Y, const char* word, uint8_t fg, uint8_t bg) {
    for (int i = 0; word[i] != '\0'; i++) {
        display(X + i, Y, word[i], fg, bg);
    }
}

void gameover() {
    fill(219, 0x00, 0x00);    // Clear screen with black
    print(-10, 0, "Game Over! You scored ", 0x0F, 0x00); // Message

    char scoreText[12];
    itoa(score, scoreText); // Convert score to string
    print(12, 0, scoreText, 0x0F, 0x00); // Show it
    print(-12, -1, "Press space bar to restart!", 0x0F, 0x00); // Show it

    while (inb(0x60) != 0x39); // Wait for space
    kernel_main(); // Restart game
}

void guyOnLeft() {
    rec(-9, -7, 2, 1, 219, 0x0E, 0x00); // Draw right arm
    rec(-8, -6, 3, 3, 219, 0x09, 0x00); // Draw body
    display(-7, -5, 2, 0x00, 0x0E); // Draw head

    display(-8, -10, 219, 0x0E, 0x00); // Draw left leg
    display(-8, -9, 219, 0x0E, 0x00); // Draw left leg

    display(-6, -10, 219, 0x0E, 0x00); // Draw right leg
    display(-6, -9, 219, 0x0E, 0x00); // Draw right leg

    rec(-5, -7, 2, 1, 219, 0x0E, 0x00); // Draw right arm

    rec(-4, -6, 1, 2, 219, 0x07, 0x00); // Draw axe handle

    display(-3, -6, 219, 0x00, 0x00); // Draw axe head
}

void guyOnRight() {
    rec(8, -7, 2, 1, 219, 0x0E, 0x00); // Draw right arm
    rec(6, -6, 3, 3, 219, 0x09, 0x00); // Draw body
    display(7, -5, 2, 0x00, 0x0E); // Draw head

    display(8, -10, 219, 0x0E, 0x00); // Draw left leg
    display(8, -9, 219, 0x0E, 0x00); // Draw left leg

    display(6, -10, 219, 0x0E, 0x00); // Draw right leg
    display(6, -9, 219, 0x0E, 0x00); // Draw right leg

    rec(4, -7, 2, 1, 219, 0x0E, 0x00); // Draw right arm

    rec(4, -6, 1, 2, 219, 0x07, 0x00); // Draw axe handle

    display(3, -6, 219, 0x00, 0x00); // Draw axe head
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

void drawLogo() { // Hehe ascii art I couldnt resist
    print(27, 12, " _____", 0x0F, 0x01); // Show message
    print(27, 11, "| ___ |", 0x0F, 0x01); // Show message
    print(27, 10, "||   ||", 0x0F, 0x01); // Show message
    print(27, 9, "||___||", 0x0F, 0x01); // Show message
    print(27, 8, "|_____|", 0x0F, 0x01); // Show message
    print(27, 7, "| |", 0x0F, 0x01); // Show message
    print(27, 6, "| |___", 0x0F, 0x01); // Show message
    print(27, 5, "|_____|", 0x0F, 0x01); // Show message
    print(27, 4, "| |", 0x0F, 0x01); // Show message
    print(27, 3, "| |___", 0x0F, 0x01); // Show message
    print(27, 2, "|_____|", 0x0F, 0x01); // Show message
    print(27, 1, "|__ __|", 0x0F, 0x01); // Show message
    print(27, 0, "  | |  ", 0x0F, 0x01); // Show message
    print(27, -1, " _| |_ ", 0x0F, 0x01); // Show message
    print(27, -2, "|_____|", 0x0F, 0x01); // Show message
    print(27, -3, "|  ___|", 0x0F, 0x01); // Show message
    print(27, -4, "| |___ ", 0x0F, 0x01); // Show message
    print(27, -5, "| ____|", 0x0F, 0x01); // Show message
    print(27, -6, "| |___ ", 0x0F, 0x01); // Show message
    print(27, -7, "|_____|", 0x0F, 0x01); // Show message
    
}

void kernel_main() {

    score = 0; // Reset score

    init_branches();
    int current_side = 'L';
    int length = sizeof(Branches) / sizeof(Branches[0]); // Find lenth of branches array
    
    fill(219, 0x01, 0x00);    // Clear screen with blue

    print(-35, 2, "Press A or D to play!", 0x0F, 0x01); // Show message

    drawLogo(); // Draw logo

    rec(-3, 12, 7, 25, 219, 0x06, 0x00); // Draw trunk

    rec(-40, -11, 80, 2, 219, 0x02, 0x00); // Draw ground
    print(-40, -12, "Copyright Ollie Young, all rights reserved 2025.", 0x00, 0x02); // Show message

    guyOnRight(); // Draw guy on right

    print(-30, 5, "Score: ", 0x0F, 0x01); // Print score

    char scoreText[12];
    itoa(score, scoreText); // Convert score to string
    print(-23, 5, scoreText, 0x0F, 0x01); // Show it

    F = 4; // Branch level starts at 4
    int Lkey = 0x1E; // Key code for A but you can change this
    int Rkey = 0x20; // Key code for D but you can change this
    int LRel = 0x9E; // Key code for A release but you can change this
    int RRel = 0xA0; // Key code for D release but you can change this

    

    while (1) { // forever loop

        if (inb(0x60) == Lkey) { // Wait for key press, currently set to A
            if (Branches[1 - F] != 'L') {
                eraceBottom(); // Erace bottom of screen

                for (int i = 0; i < length; i++) { // Loop through branches
                    eraceBranch(Branches[i], F + (1 + i)); // Erace previous branch
                    
                    drawBranch(Branches[i], F + i); // Draw next branch
                }

                rec(-40, -11, 80, 2, 219, 0x02, 0x00); // Draw ground again to not show any eracements
                print(-40, -12, "Copyright Ollie Young, all rights reserved 2025.", 0x00, 0x02); // Show message
                
                guyOnLeft(); // Draw guy on Left

                score++; // Increase score

                print(-30, 5, "Score: ", 0x0F, 0x01); // Print score

                char scoreText[12];
                itoa(score, scoreText); // Convert score to string
                print(-23, 5, scoreText, 0x0F, 0x01); // Show it

                while (inb(0x60) != LRel); // Wait for key release
                F--; // Decrease branch level

                

            } else {
                gameover();
            
            }
        }


        if (inb(0x60) == Rkey) { // Wait for key press, currently set to D
            if (Branches[1 - F] != 'R') {
                eraceBottom(); // Erace bottom of screen
                for (int i = 0; i < length; i++) { // Loop through branches
                    eraceBranch(Branches[i], F + (1 + i)); // Erace previous branch
                    drawBranch(Branches[i], F + i); // Draw next branch
                }

                rec(-40, -11, 80, 2, 219, 0x02, 0x00); // Draw ground again to not show any eracements
                print(-40, -12, "Copyright Ollie Young, all rights reserved 2025.", 0x00, 0x02); // Show message
                guyOnRight(); // Draw guy on right

                score++; // Increase score
                
                print(-30, 5, "Score: ", 0x0F, 0x01); // Print score

                char scoreText[12];
                itoa(score, scoreText); // Convert score to string
                print(-23, 5, scoreText, 0x0F, 0x01); // Show it

                while (inb(0x60) != RRel); // Wait for key release
                F--; // Decrease branch level

                

            } else {
                gameover();

            }
        }

        if ((1 - F) + 5 == 54) { // If score reaches 54, reset but keep score to make repeating level
            rec(-40, 12, 80, 18, 219, 0x01, 0x00); // Draw top part of sky
            drawLogo(); // Draw logo

            rec(-3, 12, 7, 25, 219, 0x06, 0x00); // Draw trunk
            rec(-40, -11, 80, 2, 219, 0x02, 0x00); // Draw ground
            print(-40, -12, "Copyright Ollie Young, all rights reserved 2025.", 0x00, 0x02); // Show message

            print(-30, 5, "Score: ", 0x0F, 0x01); // Print score

            char scoreText[12];
            itoa(score, scoreText); // Convert score to string
            print(-23, 5, scoreText, 0x0F, 0x01); // Show it



            F = 4; // Branch level starts at 4
        }

    }
}
