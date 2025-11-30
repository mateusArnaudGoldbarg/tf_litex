#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <uart.h>
#include <console.h>
#include <generated/csr.h>
#include <irq.h>

#include "inference.h"

static char *readstr(void)
{
    char c[2];
    static char s[64];
    static int ptr = 0;

    if(readchar_nonblock()) {
        c[0] = readchar();
        c[1] = 0;
        switch(c[0]) {
            case 0x7f:
            case 0x08:
                if(ptr > 0) {
                    ptr--;
                    putsnonl("\x08 \x08");
                }
                break;
            case 0x07:
                break;
            case '\r':
            case '\n':
                s[ptr] = 0x00;
                putsnonl("\n");
                ptr = 0;
                return s;
            default:
                if(ptr >= (sizeof(s) - 1))
                    break;
                putsnonl(c);
                s[ptr] = c[0];
                ptr++;
                break;
        }
    }
    return NULL;
}

static char *get_token(char **str)
{
    char *c, *d;

    c = (char *)strchr(*str, ' ');
    if(c == NULL) {
        d = *str;
        *str = *str+strlen(*str);
        return d;
    }
    *c = 0;
    d = *str;
    *str = c+1;
    return d;
}

static void prompt(void)
{
    printf("RUNTIME>");
}

static void help(void)
{
    puts("Available commands:");
    puts("help                            - this command");
    puts("reboot                          - reboot CPU");
    puts("led                             - led test");
    puts("sin_inference                   - sin inference");
}

static void reboot(void)
{
    ctrl_reset_write(1);
}

static void toggle_led(void)
{
    int i;
    printf("invertendo led...\n");
    i = leds_out_read();
    leds_out_write(!i);
}

static void delay_cycles(volatile int cycles) {
    for(volatile int i = 0; i < cycles; i++);
}


static void sin_inference(void)
{
    printf("Starting TF LITE MICRO SIN INFERENCE\n");
    printf("press q to stop\n");
    
    
    float x = 0.0f;
    float x_increment = 0.1f;
    int iteration = 0;
    int running = 1;
    while(running) {
        float y_pred = predict(x);
        unsigned char led_pattern = led_choice(y_pred);
        unsigned char led_output = 0;
        int num_leds_on = (led_pattern * 9) / 256;  // Quantos LEDs acender (0-8)
        if (num_leds_on > 8) num_leds_on = 8;
    
        for(int i = 0; i < num_leds_on; i++) {
            led_output |= (1 << i);
        }
        leds_out_write(led_output);
        x += x_increment;
        iteration++;
        delay_cycles(3000);

		if(readchar_nonblock()) {
            char c = readchar();
            if(c == 'q') {
                printf("\n\nStopped.\n");
                running = 0;
            }
        }
    }
}

static void console_service(void) {
    char *str;
    char *token;

    str = readstr();
    if(str == NULL) return;
    token = get_token(&str);
    if(strcmp(token, "help") == 0)
        help();
    else if(strcmp(token, "reboot") == 0)
        reboot();
    else if(strcmp(token, "led") == 0)
        toggle_led();
    else if(strcmp(token, "sin_inference") == 0)
        sin_inference();
    prompt();
}

int main(void) {
#ifdef CONFIG_CPU_HAS_INTERRUPT
    irq_setmask(0);
    irq_setie(1);
#endif
    uart_init();

    printf("Hellorld!\n");
    help();
    prompt();

    while(1) {
        console_service();
    }

    return 0;
}