#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define CMD_BUF_SIZE 256
#define VGA ((uint16_t*)0xB8000)
#define HEIGHT 25
#define LENGTH 80
#define inb(p) ({ uint8_t r; __asm__ volatile ("inb %1, %0" : "=a"(r) : "Nd"(p)); r; })

bool complete = false;
char current_text[CMD_BUF_SIZE];
int cmd_len = 0;
int current_line = 0;
int Cinline = 0;
char* username = "Ollie";
char* hostname = "OlzuOS";
char currdir[64] = "home";
char prefix[64];
char sufix[64];
char PlaceholderDir[64];
char memory[4096];
char* Paths[128] = {"home", "home/test", "home/test/test2", NULL};
char* FilePaths[128] = {"home/hello.txt", "home/hello2.txt", "home/test/test4.txt", NULL};
char* PointerLocations[128] = {"0", "1", "33", NULL};
char* Pointers[128] = {"0", "32", "64", NULL};

char* PointerNames[128] = {"home", "home/test", "home/test/test2", NULL};


char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', // 0-14
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',0,  // 15-29
    'a','s','d','f','g','h','j','k','l',';','\'','`',0,'\\','z',  // 30-44
    'x','c','v','b','n','m',',','.','/',0,'*',0,' ',0              // 45-58
    // rest zeros
};



int kstrlen(const char* s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

void display(int X, int Y, char ch, uint8_t fg, uint8_t bg) {
    int y = (HEIGHT / 2) - Y;
    int x = (LENGTH / 2) + X;
    uint8_t color = (bg << 4) | (fg & 0x0F);
    VGA[y * LENGTH + x] = (color << 8) | ch;
}

void fill(char ch, uint8_t fg, uint8_t bg) {
    uint8_t color = (bg << 4) | (fg & 0x0F);
    for (int i = 0; i < LENGTH * HEIGHT; i++) {
        VGA[i] = (color << 8) | ch;
    }
}

void WriteLine(const char* text, uint8_t fg) {
    Cinline = 0;
    for (int i = 0; i < kstrlen(text); i++) {
        display(Cinline - 40, current_line + 12, text[i], fg, 0x00);
        Cinline++;
    }
}

void WriteSameLine(const char* text, uint8_t fg) {
    for (int i = 0; i < kstrlen(text); i++) {
        display(Cinline - 40, current_line + 12, text[i], fg, 0x00);
        Cinline++;
    }
}

void NewLine() {
    current_line--;
    Cinline = 0;
    WriteSameLine(username, 0x2); 
    WriteSameLine("@", 0x2); 
    WriteSameLine(hostname, 0x2); 
    WriteSameLine(":", 0xF); 
    WriteSameLine(currdir, 0x3); 
    WriteSameLine("$", 0xF); 

    cmd_len = 0;
    current_text[0] = 0;
}

uint8_t get_scancode() {
    uint8_t scancode;
    uint8_t status;  // declare here

    do {
        status = inb(0x64);
    } while (!(status & 1));  // now it works

    scancode = inb(0x60);
    return scancode;
}



char get_key() {
    uint8_t scancode = get_scancode();
    if (scancode > 127) return 0; // ignore key release
    return keymap[scancode];
}

int kstrcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

void kstrcpy(char* dest, const char* src) {
    int i = 0;
    
    // 1. Keep looping as long as the current character isn't 0 (the end)
    while (src[i] != '\0') {
        // 2. Copy the character from the source to the destination
        dest[i] = src[i];
        i++;
    }
    
    // 3. Crucial: Put a 0 at the very end so the OS knows the string is over
    dest[i] = '\0';
}

void error(const char* text, const char* Secondtext, const char* Thirdtext) {
    current_line--;
    Cinline = 0;
    WriteLine("Error: ", 0x4);
    WriteSameLine(text, 0x4);
    WriteSameLine(Secondtext, 0x4);
    WriteSameLine(Thirdtext, 0x4);
}

int starts_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*str++ != *prefix++) return 0;
    }
    return 1;
}


void RunCommand(const char* text) {

    if (kstrcmp(text, "") == 0)
    {
        
        return;
    }
    
    // find prefix
    int j = 0;
    for (int i = 0; text[i] != ' ' && text[i] != '\0'; i++)
    {
        prefix[j++] = text[i];
    }
    prefix[j] = 0;

    // find sufix
    j = 0;
    for (int i = kstrlen(prefix + 1); text[i] != '\0'; i++)
    {
        sufix[j++] = text[i];
    }
    sufix[j] = 0;
    
    if (kstrcmp(prefix, "cd") == 0) {

        if (kstrlen(text) == 2)
        {
            kstrcpy(currdir, "home");
            return;
        }
        

        kstrcpy(PlaceholderDir, currdir);
        j = kstrlen(currdir);
        currdir[j++] = '/';
        for (int i = 3; text[i] && j < 63; i++) {
            currdir[j++] = text[i];
        }
        currdir[j] = 0;  // null terminate

        complete = false;
        for (int i = 0; i < kstrlen(currdir); i++)
        {
            if (kstrcmp(Paths[i], currdir) == 0)
            {
                complete = true;
            }
            
        }

        if (!complete)
        {
            kstrcpy(currdir, PlaceholderDir);
            error("No path found", "", "");
        } 

    } else if (kstrcmp(prefix, "clear") == 0) {
        fill(' ', 0x00, 0x00);
        current_line = 1;

    } else if (kstrcmp(prefix, "ls") == 0) {
        current_line--;
        Cinline = 0;

        char temp_str[64];
        char temp_str2[64];
        char full_path[64];
        char seen[128][64];  // Track what we've already shown
        int seen_count = 0;
        
        // List directories from Paths
        for (int i = 0; Paths[i] != NULL; i++)
        {
            // Clear temp_str2 at the start of each iteration
            temp_str2[0] = '\0';
            
            kstrcpy(temp_str, Paths[i]);

            if (starts_with(temp_str, currdir))
            {
                int start_idx = kstrlen(currdir) + 1;  // skip "currdir/"
                int j = 0;
                // Check if there's at least one more character after currdir/
                if (temp_str[start_idx] != '\0')
                {
                    // Copy only the immediate next part (until next "/" if exists)
                    for (int l = start_idx; temp_str[l] != '\0' && temp_str[l] != '/'; l++)
                    {
                        temp_str2[j++] = temp_str[l];
                    }
                    temp_str2[j] = '\0';
                    
                    // Check if we've already shown this
                    int already_shown = 0;
                    for (int k = 0; k < seen_count; k++) {
                        if (kstrcmp(seen[k], temp_str2) == 0) {
                            already_shown = 1;
                            break;
                        }
                    }
                    
                    if (!already_shown) {
                        kstrcpy(seen[seen_count], temp_str2);
                        seen_count++;
                        WriteSameLine(temp_str2, 0x2);  // Green for directories
                        WriteSameLine(" ", 0x2);
                    }
                }
            }
        }
        
        // List files from FilePaths
        for (int i = 0; FilePaths[i] != NULL; i++)
        {
            // Clear temp_str2 at the start of each iteration
            temp_str2[0] = '\0';
            
            kstrcpy(temp_str, FilePaths[i]);

            if (starts_with(temp_str, currdir))
            {
                int start_idx = kstrlen(currdir) + 1;  // skip "currdir/"
                int j = 0;
                // Check if there's at least one more character after currdir/
                if (temp_str[start_idx] != '\0' && temp_str[start_idx + 1] != '\0')
                {
                    // Copy only the filename (until end, no "/" for files)
                    for (int l = start_idx; temp_str[l] != '\0' && temp_str[l] != '/'; l++)
                    {
                        temp_str2[j++] = temp_str[l];
                    }
                    temp_str2[j] = '\0';
                    
                    // Check if we've already shown this
                    int already_shown = 0;
                    for (int k = 0; k < seen_count; k++) {
                        if (kstrcmp(seen[k], temp_str2) == 0) {
                            already_shown = 1;
                            break;
                        }
                    }
                    
                    if (!already_shown) {
                        kstrcpy(seen[seen_count], temp_str2);
                        seen_count++;
                        WriteSameLine(temp_str2, 0xF);  // White for files
                        WriteSameLine(" ", 0xF);
                    }
                }
            }
        }

    } else
    {
        error("No command found named: ", text, "");
    }
    
}


void kernel_main() {
    fill(' ', 0x00, 0x00);   // Clear screen
    WriteLine(" ------     |       ------  |      |         ------    ------", 0x2); 
    current_line--;
    WriteLine("|      |    |            |  |      |        |      |  |      ", 0x2); 
    current_line--;
    WriteLine("|      |    |          |    |      |        |      |  ------ ", 0x2); 
    current_line--;
    WriteLine("|      |    |        |      |      |        |      |        |", 0x2); 
    current_line--;
    WriteLine(" ------     ----    ------   ------          ------   ------ ", 0x2); 
    current_line--;
    current_line--;
    WriteLine("Welcome!", 0x2); 
    current_line--;
    
    NewLine();

    display(Cinline - 40, current_line + 12, ' ', 0xF, 0xF);
    while (1) {
        char key = get_key();
        if (key) {
            
            if (key == '\n') {

                display(Cinline - 40, current_line + 12, ' ', 0x0, 0x0);
                RunCommand(current_text);
                NewLine();
                
            } else if (key == '\b') {

                if (kstrlen(currdir) + kstrlen(username) + kstrlen(hostname) + 3 < Cinline)
                {
                    cmd_len--;
                    current_text[cmd_len] = 0;
                    display(Cinline - 40, current_line + 12, ' ', 0x0, 0x0);
                    Cinline--;
                    
                }
                
            } else {
                if (cmd_len < CMD_BUF_SIZE - 1) {
                    WriteSameLine(&key, 0xF);
                    current_text[cmd_len++] = key;
                    current_text[cmd_len] = 0;  // keep string valid
                }
            }

            display(Cinline - 40, current_line + 12, ' ', 0xF, 0xF);
            
        }

        
    }
}
