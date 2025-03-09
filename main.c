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
    
    TINEC_CanvasFill(&canvas, TINEC_COLOR_RED);

    // Loop principal
    int running = 1;
    TINEC_MainLoop(&running) {
        TINEC_EventQuit(&running);
        TINEC_CanvasUpdate(&canvas);

    }

    // Limpeza
    TINEC_CanvasDeinit(&canvas);
    return 0;
}