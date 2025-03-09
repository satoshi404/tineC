#ifndef TINEC_HEADER
#define TINEC_HEADER

typedef unsigned int  u32; // 32 bits
typedef unsigned char u8;  // 8 bits

#define DEFAULT_CANVAS_COORDS_X(sw) sw / 2
#define DEFAULT_CANVAS_COORDS_Y(sh) sh / 2

// Format color -> 0xRGBA
#define TINEC_COLOR_RED     0xff0000ff
#define TINEC_COLOR_GREEN   0x00ff00ff
#define TINEC_COLOR_BLUE    0x0000ffff
#define TINEC_COLOR_BLACK   0x000000ff
#define TINEC_COLOR_WHITE   0xffffffff

typedef struct {
    u32* pixels;
    u32 width;
    u32 height;
} Canvas;

Canvas TINEC_CanvasInit(const u8* title, u32 width, u32 height );
void   TINEC_CanvasUpdate(Canvas* canvas);
void   TINEC_CanvasFill(Canvas* canvas, u32 color);
void   TINEC_CanvasDeinit(Canvas* canvas);

// Draw functions
void   TINEC_CanvasDrawRect(Canvas* canvas, u32 rect_x, u32 rect_y, u32 rect_w, u32 rect_h, u32 color);
void   TINEC_CanvasDrawPoint(Canvas* canvas, u32 point_x, u32 point_y, u32 color);
void   TINEC_CanvasDrawCircle(Canvas* canvas, u32 lenght, u32 radius, u32 color);

#endif // ---- TINEC_HEADER ----

#ifdef TINEC_IMPLEMENTATION
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#define PIXELS_SIZE(sw, sh) (sw * sh) * sizeof(u32)
#define CANVAS_FOREACH(sw, sh, index) for (*(index); *(index) < (sw * sh); (*(index))++)
#define DEBUG_LINE_FEED 30
#define DEBUG_PIXELS(sw, sh, px) \
        u32 i = 0;\
        CANVAS_FOREACH(canvas.width, canvas.height, &i) { \
        printf("%i", canvas.pixels[i]);\
        if (i % DEBUG_LINE_FEED == 0) {\
            printf("\n");}}\

static SDL_Event event = {0};

#define TINEC_MainLoop(state) while (*state)
#define TINEC_EventQuit(state) \
    while (SDL_PollEvent(&event) != 0) {\
        switch (event.type) {\
            case SDL_QUIT:\
                *state = 0;break;}}\

static SDL_Window* _window = NULL;
static SDL_Renderer* _renderer = NULL;
static SDL_Texture* _texture = NULL; 

static void TINEC_InitSDL(const u8* title, u32 width, u32 height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {  // Corrigido: SDL_Init retorna < 0 em erro
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        exit(1);
    }
    
    _window = SDL_CreateWindow(
        (const char*)title, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        width, 
        height, 
        0  // Garante que a janela seja visível
    );
    
    if (!_window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    
    if (!_renderer) {
        printf("Erro ao criar renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(_window);
        SDL_Quit();
        exit(1);
    }
    
    _texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                width, height);
    if (!_texture) {
        printf("Erro ao criar textura: %s\n", SDL_GetError());
        SDL_DestroyRenderer(_renderer);
        SDL_DestroyWindow(_window);
        SDL_Quit();
        exit(1);
    }
}

Canvas TINEC_CanvasInit(const u8* title, u32 width, u32 height ) {
    Canvas canvas = {0};
    canvas.pixels = (u32*) malloc(PIXELS_SIZE(width, height)); 

    if (!canvas.pixels) {
        const u8* errn_message = "Failed to allocate canvas memory";
        printf(errn_message);
        exit(13);
    }
    
    canvas.width  = width;
    canvas.height = height;

    TINEC_InitSDL(title, width, height);

    return canvas;
}

void TINEC_CanvasUpdate(Canvas* canvas) {
    SDL_UpdateTexture(_texture, NULL, canvas->pixels, canvas->width * sizeof(u32));
    SDL_RenderClear(_renderer);
    SDL_RenderCopy(_renderer, _texture, NULL, NULL);
    SDL_RenderPresent(_renderer);
}

void TINEC_CanvasFill(Canvas* canvas, u32 color) {
    u32 i = 0;
    CANVAS_FOREACH(canvas->width, canvas->height, &i) {
        canvas->pixels[i] = color;
    }
}

void   TINEC_CanvasDrawRect(Canvas* canvas, u32 rect_x, u32 rect_y, u32 rect_w, u32 rect_h, u32 color) {

}

void TINEC_CanvasDeinit(Canvas* canvas) {
    if (canvas && canvas->pixels) {
        free(canvas->pixels);
        canvas->pixels = NULL;
    }
    if (_texture) {
        SDL_DestroyTexture(_texture);
        _texture = NULL;
    }
    if (_renderer) {
        SDL_DestroyRenderer(_renderer);
        _renderer = NULL;
    }
    if (_window) {
        SDL_DestroyWindow(_window);
        _window = NULL;
    }
    SDL_Quit();
}

#endif // ---- TINEC_IMPLEMENTATION ----