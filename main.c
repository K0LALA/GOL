#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "main.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define WIDTH 750
#define HEIGHT 250
#define PIXEL_SIZE 4

static uint8_t cells[HEIGHT][WIDTH];

void showCells()
{
    int y;
    for (y = 0; y < HEIGHT; y++)
    {
        int x;
        for (x = 0; x < WIDTH; x++)
        {
            printf("%d ", *(*(cells + y) + x));
        }
        printf("\n");
    }
    printf("\n");
}

void initCells()
{
    /* *(*(cells + 1) + 3) = 1;
     *(*(cells + 2) + 4) = 1;
     *(*(cells + 3) + 4) = 1;
     *(*(cells + 3) + 3) = 1;
     *(*(cells + 3) + 2) = 1; */
    int y;
    for (y = 0; y < HEIGHT; y++)
    {
        int x;
        for (x = 0; x < WIDTH; x++)
        {
            *(*(cells + y) + x) = (rand() % 3 == 0);
        }
    }
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    /* Initialize random */
    srand((int)time(NULL));

    /* Initialize SDL */
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("SDL3 Draw", 800, 600, SDL_WINDOW_RESIZABLE, &window, &renderer))
    {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    initCells();

    Bucket bucket = createBucket();
    
    addToBucket(&bucket, 1, 2);
    addToBucket(&bucket, 3, 4);
    addToBucket(&bucket, 9, 5);
    addToBucket(&bucket, -6214, -6789);
    addToBucket(&bucket, 6214, -6789);
    printf("%d\n", isInBucket(&bucket, 1, 2));
    printf("%d\n", isInBucket(&bucket, 3, 4));
    printf("%d\n", isInBucket(&bucket, 9, 5));
    printf("%d\n", isInBucket(&bucket, -6214, -6789));
    printf("%d\n", isInBucket(&bucket, 6214, -6789));
    printf("%d\n", isInBucket(&bucket, 3, 49));
    addToBucket(&bucket, 3, 49);
    printf("%d\n", isInBucket(&bucket, 3, 49));
    addToBucket(&bucket, 3, 49);
    printf("%d\n", isInBucket(&bucket, 3, 49));
    
    freeBucket(&bucket);

    return SDL_APP_CONTINUE;
}

void displayCells()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int y;
    for (y = 0; y < HEIGHT; y++)
    {
        int x;
        for (x = 0; x < WIDTH; x++)
        {
            if (*(*(cells + y) + x))
            {
                SDL_FRect rect = {x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void nextStep()
{
    uint8_t *aboveLine = (uint8_t *)malloc(WIDTH);
    uint8_t *line = (uint8_t *)malloc(WIDTH);
    int y;
    for (y = 0; y < HEIGHT; y++)
    {
        int x;
        for (x = 0; x < WIDTH; x++)
        {
            uint8_t cellCount = 0;
            int dx;
            for (dx = -1; dx <= 1; dx++)
            {
                int dy;
                for (dy = -1; dy <= 1; dy++)
                {
                    if (cellCount > 3)
                        break;
                    if ((dx == 0 && dy == 0) ||
                        x + dx < 0 || x + dx >= WIDTH ||
                        y + dy < 0 || y + dy >= HEIGHT)
                        continue;
                    if (*(*(cells + y + dy) + x + dx))
                        cellCount++;
                }
            }
            if (*(*(cells + y) + x) && cellCount == 2)
                *(line + x) = 1;
            else if (cellCount == 3)
                *(line + x) = 1;
            else
                *(line + x) = 0;
        }
        if (y > 0)
        {
            memcpy(*(cells + y - 1), aboveLine, WIDTH);
        }
        memcpy(aboveLine, line, WIDTH);
    }
    free(aboveLine);
    free(line);
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
    }
    else if (event->type == SDL_EVENT_KEY_DOWN)
    {
        if (event->key.key == SDLK_ESCAPE)
        {
            return SDL_APP_SUCCESS;
        }
        nextStep();
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    displayCells();

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
