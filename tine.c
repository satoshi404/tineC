#ifndef TINEC_HEADER
#define TINEC_HEADER

typedef unsigned int  u32; // 32 bits
typedef unsigned char u8;  // 8 bits

#define DEFAULT_CANVAS_COORDS_X(sw) sw / 2
#define DEFAULT_CANVAS_COORDS_Y(sh) sh / 2

// Rect mode
#define TINEC_RECT_MODE0 0
#define TINEC_RECT_MODE1 1

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
void   TINEC_CanvasDrawRect(Canvas* canvas, u32 rect_x, u32 rect_y, u32 rect_w, u32 rect_h, u32 color,u32 mode);
void   TINEC_CanvasDrawPoint(Canvas* canvas, u32 point_x, u32 point_y, u32 color);
void   TINEC_CanvasDrawCircle(Canvas* canvas, u32 lenght, u32 radius, u32 color);
void   TINEC_CanvasDrawLine(Canvas* canvas, u32 x0, u32 y0, u32 x1, u32 y1, u32 color);


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
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {  
        printf("Erro ao inicializar SDL: %s\n", SDL_GetError());
        exit(1);
    }
    
    _window = SDL_CreateWindow(
        (const char*)title, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        width, 
        height, 
        0  
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

void TINEC_CanvasDrawLine(Canvas* canvas, u32 x0, u32 y0, u32 x1, u32 y1, u32 color) {  
    int dx = abs((int)x1 - (int)x0);
    int dy = -abs((int)y1 - (int)y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    int x = x0;
    int y = y0;

    while (1) {
        if (x >= 0 && x < canvas->width && y >= 0 && y < canvas->height) {
            canvas->pixels[y * canvas->width + x] = color;
        }
        if (x == x1 && y == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y += sy;
        }
    }
}

void TINEC_CanvasDrawRect(Canvas* canvas, u32 x, u32 y, u32 w, u32 h, u32 color,u32 mode) {

    if (mode == TINEC_RECT_MODE0) {
        u32 x0 = x;                    
        u32 y0 = y;                   
        u32 x1 = x + w;                
        u32 y1 = y + h;               
    
        if (x0 >= canvas->width) return;
        if (y0 >= canvas->height) return;
        if (x1 > canvas->width) x1 = canvas->width;
        if (y1 > canvas->height) y1 = canvas->height;
    
        for (u32 py = y0; py < y1; py++) {
            for (u32 px = x0; px < x1; px++) {
                canvas->pixels[py * canvas->width + px] = color;
            }
        }
    } else if (mode == TINEC_RECT_MODE1) {
        u32 x0 = x;         // Superior esquerdo
    u32 y0 = y;
    u32 x1 = x + w;     // Superior direito
    u32 y1 = y;
    u32 x2 = x + w;     // Inferior direito
    u32 y2 = y + h;
    u32 x3 = x;         // Inferior esquerdo
    u32 y3 = y + h;
        TINEC_CanvasDrawLine(canvas, x0, y0, x1, y1, color); 
        TINEC_CanvasDrawLine(canvas, x1, y1, x2, y2, color); 
        TINEC_CanvasDrawLine(canvas, x2, y2, x3, y3, color); 
        TINEC_CanvasDrawLine(canvas, x3, y3, x0, y0, color); 
    }
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