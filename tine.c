#ifndef TINEC_HEADER
#define TINEC_HEADER

typedef unsigned int  u32; // 32 bits
typedef unsigned char u8;  // 8 bits
typedef float f32;       

#define DEFAULT_CANVAS_COORDS_X(sw) ((f32)sw / 2.0f)
#define DEFAULT_CANVAS_COORDS_Y(sh) ((f32)sh / 2.0f)

// Rect mode
#define TINEC_MODE0 0
#define TINEC_MODE1 << 1

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

Canvas TINEC_CanvasInit(const u8* title, u32 width, u32 height);
void   TINEC_CanvasUpdate(Canvas* canvas);
void   TINEC_CanvasFill(Canvas* canvas, u32 color);
void   TINEC_CanvasDeinit(Canvas* canvas);

// Draw functions (modificadas para f32)
void   TINEC_CanvasDrawRect(Canvas* canvas, f32 x, f32 y, u32 w, u32 h, u32 color, u32 mode);
void   TINEC_CanvasDrawPixel(Canvas* canvas, f32 x, f32 y, u32 color);
void   TINEC_CanvasDrawCircle(Canvas* canvas, f32 x, f32 y, f32 r, u32 color);
void   TINEC_CanvasDrawLine(Canvas* canvas, f32 x0, f32 y0, f32 x1, f32 y1, u32 color);

#endif // ---- TINEC_HEADER ----

#ifdef TINEC_IMPLEMENTATION
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

Canvas TINEC_CanvasInit(const u8* title, u32 width, u32 height) {
    Canvas canvas = {0};
    canvas.pixels = (u32*) malloc(PIXELS_SIZE(width, height)); 

    if (!canvas.pixels) {
        const u8* errn_message = "Failed to allocate canvas memory";
        printf("%s\n", errn_message);
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

void TINEC_CanvasDrawPixel(Canvas* canvas, f32 x, f32 y, u32 color) {
    // Converter coordenadas f32 para u32 (arredondando para baixo)
    u32 px = (u32)floorf(x);
    u32 py = (u32)floorf(y);
    if (px >= 0 && px < canvas->width && py >= 0 && py < canvas->height) {
        canvas->pixels[py * canvas->width + px] = color;
    }
}

void TINEC_CanvasDrawLine(Canvas* canvas, f32 x0, f32 y0, f32 x1, f32 y1, u32 color) {  
    // Converter coordenadas f32 para inteiros para o algoritmo de Bresenham
    int ix0 = (int)floorf(x0);
    int iy0 = (int)floorf(y0);
    int ix1 = (int)floorf(x1);
    int iy1 = (int)floorf(y1);

    int dx = abs(ix1 - ix0);
    int dy = -abs(iy1 - iy0);
    int sx = (ix0 < ix1) ? 1 : -1;
    int sy = (iy0 < iy1) ? 1 : -1;
    int err = dx + dy;
    int x = ix0;
    int y = iy0;

    while (1) {
        if (x >= 0 && x < (int)canvas->width && y >= 0 && y < (int)canvas->height) {
            canvas->pixels[y * canvas->width + x] = color;
        }
        if (x == ix1 && y == iy1) break;
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

void TINEC_CanvasDrawRect(Canvas* canvas, f32 x, f32 y, u32 w, u32 h, u32 color, u32 mode) {
    if (mode == TINEC_MODE0) {
        // Modo preenchido
        f32 x0 = x;                    
        f32 y0 = y;                   
        f32 x1 = x + w;                
        f32 y1 = y + h;               
    
        // Converter para inteiros
        u32 ix0 = (u32)floorf(x0);
        u32 iy0 = (u32)floorf(y0);
        u32 ix1 = (u32)floorf(x1);
        u32 iy1 = (u32)floorf(y1);
    
        if (ix0 >= canvas->width) return;
        if (iy0 >= canvas->height) return;
        if (ix1 > canvas->width) ix1 = canvas->width;
        if (iy1 > canvas->height) iy1 = canvas->height;
    
        for (u32 py = iy0; py < iy1; py++) {
            for (u32 px = ix0; px < ix1; px++) {
                canvas->pixels[py * canvas->width + px] = color;
            }
        }
    } else if (mode == TINEC_MODE1) {
        // Modo contorno
        TINEC_CanvasDrawLine(canvas, x, y, x + w, y, color); 
        TINEC_CanvasDrawLine(canvas, x + w, y, x + w, y + h, color); 
        TINEC_CanvasDrawLine(canvas, x + w, y + h, x, y + h, color); 
        TINEC_CanvasDrawLine(canvas, x, y + h, x, y, color); 
    }
}

void TINEC_CanvasDrawCircle(Canvas* canvas, f32 x, f32 y, f32 r, u32 color, u32 mode) {
    if (mode == TINEC_MODE1) {
    int cx = (int)floorf(x);
    int cy = (int)floorf(y);
    int radius = (int)floorf(r);

    int px = 0;
    int py = radius;
    int d = 3 - 2 * radius;

    // Função auxiliar para desenhar os 8 pontos simétricos do círculo
    while (px <= py) {
        // Desenhar os 8 pontos simétricos
        TINEC_CanvasDrawPixel(canvas, (f32)(cx + px), (f32)(cy + py), color);
        TINEC_CanvasDrawPixel(canvas, (f32)(cx - px), (f32)(cy + py), color);
        TINEC_CanvasDrawPixel(canvas, (f32)(cx + px), (f32)(cy - py), color);
        TINEC_CanvasDrawPixel(canvas, (f32)(cx - px), (f32)(cy - py), color);
        TINEC_CanvasDrawPixel(canvas, (f32)(cx + py), (f32)(cy + px), color);
        TINEC_CanvasDrawPixel(canvas, (f32)(cx - py), (f32)(cy + px), color);
        TINEC_CanvasDrawPixel(canvas, (f32)(cx + py), (f32)(cy - px), color);
        TINEC_CanvasDrawPixel(canvas, (f32)(cx - py), (f32)(cy - px), color);

        if (d < 0) {
            d = d + 4 * px + 6;
        } else {
            d = d + 4 * (px - py) + 10;
            py--;
        }
        px++;
    }
  } else if (mode == TINEC_MODE0) {

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