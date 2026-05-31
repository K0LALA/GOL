#include <stdio.h>

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define WIDTH 20
#define HEIGHT 20
#define PIXEL_SIZE 16

static const SDL_Color BLACK = { 0, 0, 0, SDL_ALPHA_OPAQUE };
static const SDL_Color WHITE = { 255, 255, 255, SDL_ALPHA_OPAQUE };

static SDL_Surface *deadCell = NULL;
static SDL_Surface *aliveCell = NULL;

static uint8_t cells[HEIGHT][WIDTH];
static SDL_Texture *cellsTexture = NULL;

void initSurfaces() {
    SDL_FillSurfaceRect(deadCell, NULL, 0x000000FF);
    SDL_FillSurfaceRect(aliveCell, NULL, 0xFFFFFFFF);
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("SDL3 Draw", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Create the surface for the cells */
    cellsTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE);

    SDL_SetRenderTarget(renderer, cellsTexture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL);

    /* Create the two constant surfaces for dead/alive cells */
    deadCell = SDL_CreateSurface(PIXEL_SIZE, PIXEL_SIZE, SDL_PIXELFORMAT_RGBA32);
    aliveCell = SDL_CreateSurface(PIXEL_SIZE, PIXEL_SIZE, SDL_PIXELFORMAT_RGBA32);
    initSurfaces();

    return SDL_APP_CONTINUE;
}

void displayCells() {
    int y;
    for (y = 0; y < HEIGHT; y++) {
        int x;
        for (x = 0; x < WIDTH; x++) {
            SDL_Rect rect = {x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE};
            SDL_UpdateTexture(cellsTexture, &rect, (*(*(cells + y) + x) ? aliveCell->pixels : deadCell->pixels), deadCell->pitch);
        }
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_FRect target = {(int)((800 - WIDTH * PIXEL_SIZE) / 2), (int)((600 - HEIGHT * PIXEL_SIZE) / 2), WIDTH * PIXEL_SIZE, HEIGHT * PIXEL_SIZE};
    SDL_RenderTexture(renderer, cellsTexture, NULL, &target);
    SDL_RenderPresent(renderer);
}

int i = 0;
int changed = 1;

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    else if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_ESCAPE) {
            return SDL_APP_SUCCESS;
        }
        *(*(cells + i % WIDTH) + (int)(i / HEIGHT)) = 1;
        i++;
        changed=1;
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    if (changed) {
        changed = 0;
        displayCells();
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
