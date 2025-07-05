//Uses VGA text mode to display HELLO on a centerd 7x3 blue square. You can run these on Linux at https://github.com/OllieOlzu/Easy-32-bit-C-kernel-to-ISO.

#include <stdint.h>
#define V ((uint16_t*)0xB8000)
#define HIGHT 24 // Hight of the screen
#define LENTH 80 // Lenth of the screen

void display(X,Y,L,fg,bg) { // Display somthing at a spisific cordinate and color
    int y = (HIGHT / 2) - Y;
    int x = (LENTH / 2) + X;
    int fullc = (bg << 4) | (fg & 0x0F);
    V[LENTH*y+x] = (fullc << 8) | L;
}

void fill(L,fg,bg) { // Fill the screen with a charector and color
    int fullc = (bg << 4) | (fg & 0x0F);
    for (int i = 0; i < LENTH * HIGHT; i++) {
        V[i] = (fullc << 8) | L;
    }
}

void kernel_main() {
    // Fill screen with black, so clearing the screen to remove any IPXE stuff
    fill(219, 0x00, 0x00);

    // Make upper blue bourder
     display(-3, 1, 219, 0x01, 0x00);
     display(-2, 1, 219, 0x01, 0x00);
     display(-1, 1, 219, 0x01, 0x00);
     display(0, 1, 219, 0x01, 0x00);
     display(1, 1, 219, 0x01, 0x00);
     display(2, 1, 219, 0x01, 0x00);
     display(3, 1, 219, 0x01, 0x00);

     // Display "HELLO" in the middle of the screen, along with the blue sides
     display(-3, 0, 219, 0x01, 0x01);
     display(-2, 0, 'H', 0x02, 0x01);
     display(-1, 0, 'E', 0x02, 0x01);
     display(0, 0, 'L', 0x02, 0x01);
     display(1, 0, 'L', 0x02, 0x01);
     display(2, 0, 'O', 0x02, 0x01);
     display(3, 0, 219, 0x01, 0x01);

     // Make lower blue bourder
     display(-3, -1, 219, 0x01, 0x00);
     display(-2, -1, 219, 0x01, 0x00);
     display(-1, -1, 219, 0x01, 0x00);
     display(0, -1, 219, 0x01, 0x00);
     display(1, -1, 219, 0x01, 0x00);
     display(2, -1, 219, 0x01, 0x00);
     display(3, -1, 219, 0x01, 0x00);

     //Stop the kernel from exiting
     while (1); 


}
