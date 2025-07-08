# 32-bit-C-kernel-code
A collection of cool 32 bit protected mode C kernel code that I coded. You can test them out here:

https://github.com/OllieOlzu/Easy-32-bit-C-kernel-to-ISO

I apoligise for my spelling I'm like 13, I write these myself (Not using chatGPT... mostly) apart from the assembely bits that I have no idea how they work or what they do

# NOTES:

If you have used https://github.com/OllieOlzu/Easy-32-bit-C-kernel-to-ISO to run these, the way the restart works is by rebooting the CPU, so if you add another bin file for grub to boot it will land you there instead of restarting the game. If you want to add this to your own scripts, here's how to:

At the top, add this:

    #define outb(port, val) __asm__ volatile ("outb %0, %1" : : "a" ((uint8_t)(val)), "Nd" (port))

Add this function below it somewhere:

    static void reboot() {
        uint8_t good = 0x02;
        while (good & 0x02)
            good = inb(0x64);
        outb(0x64, 0xFE); // Send CPU reset command
    }

And then simply run it with

    reboot();
