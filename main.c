#include <stdbool.h>
#include <sys/unistd.h>
#include <SDL2/SDL.h>
#include <unistd.h>
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

    // Loop principal
    int running = 1;
    int y = 250;
    TINEC_MainLoop(&running) {
        TINEC_EventQuit(&running);
        TINEC_CanvasDrawRect(&canvas, 350 - 200, y , 100, 100, TINEC_COLOR_RED, TINEC_RECT_MODE1);
        TINEC_CanvasDrawCircle(&canvas, 350, 250, 50, TINEC_COLOR_GREEN);
        TINEC_CanvasUpdate(&canvas);
        TINEC_CanvasFill(&canvas, TINEC_COLOR_BLACK);
        usleep(16777);
    }

    // Limpeza
    TINEC_CanvasDeinit(&canvas);
    return 0;
}