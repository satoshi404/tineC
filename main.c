#include <stdbool.h>
#include <sys/unistd.h>
#include <SDL2/SDL.h>
#define TINEC_IMPLEMENTATION
#include "tine.c"

#include <stdio.h>

int main(int argc, char* argv[]) {
    // Inicializa o canvas
    Canvas canvas = TINEC_CanvasInit((const u8*)"Test Window", 800, 600);
    if (!canvas.pixels) {
        printf("Falha ao inicializar o canvas\n");
        return 1;
    }

    printf("abs: %i", abs(100 - 300));
    
    // Loop principal
    int running = 1;
    int y = 100;
    TINEC_MainLoop(&running) {
        TINEC_EventQuit(&running);
        TINEC_CanvasDrawRect(&canvas, 100, y , 90, 80, TINEC_COLOR_RED, TINEC_RECT_MODE0);
        TINEC_CanvasUpdate(&canvas);
        TINEC_CanvasFill(&canvas, TINEC_COLOR_BLACK);
        y++;

    }

    // Limpeza
    TINEC_CanvasDeinit(&canvas);
    return 0;
}