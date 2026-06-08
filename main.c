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

static Bucket *bucket;
static Bucket *bucketNext;

// Cells that have the potential to change, either die or live
static Bucket *potential;
static Bucket *potentialNext;

static char changed = 1;

#define PIXEL_SIZE 2

void addCell(int16_t x, int16_t y)
{
    addToBucket(bucket, x, y);
    addToBucket(bucketNext, x, y);

    int dy;
    for (dy = -1; dy <= 1; dy++)
    {
        int dx;
        for (dx = -1; dx <= 1; dx++)
        {
            addToBucket(potentialNext, x + dx, y + dy);
        }
    }
}

void initCells()
{
    /*addCell(3, 1);
    addCell(4, 2);
    addCell(4, 3);
    addCell(3, 3);
    addCell(2, 3);*/
    int y;
    for (y = 0; y < 500; y++)
    {
        int x;
        for (x = 0; x < 500; x++)
        {
            if(!(rand() % 13))
            {
                addCell(x, y);
            }
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

    bucket = createBucket();
    bucketNext = createBucket();
    potentialNext = createBucket();
    
    initCells();

    return SDL_APP_CONTINUE;
}

void displayCells()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    int i;
    for (i = 0; i < BUCKET_SIZE; i++)
    {
        if (!IS_BIT_PRESENT(bucket->areFilled[BUCKET_FILLED_LIST_INDEX(i)], BUCKET_FILLED_LIST_BIT_SHIFT(i)))
        {
            continue;
        }
        ChainedListNode *currentCell = &bucket->chainedLists[i];
        do
        {
            COORDINATE_TYPE x, y;
            x = currentCell->x;
            y = currentCell->y;

            SDL_FRect rect = {x * PIXEL_SIZE, y * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE};
            SDL_RenderFillRect(renderer, &rect);

            currentCell = currentCell->next;
        } while (currentCell != NULL);
    }

    SDL_RenderPresent(renderer);
}

void NEWnextStep()
{
    // setActive = setActiveNext
    bucket = bucketNext;
    // setActiveNext.clear()
    bucketNext = createBucket();
    // setPotential = setPotentialNext
    potential = potentialNext;
    potentialNext = createBucket();
    // setPotentialNext = setActive
    deepCopy(potentialNext, bucket);

    int i;
    for (i = 0; i < BUCKET_SIZE; i++)
    {
        if (!IS_BIT_PRESENT(potential->areFilled[BUCKET_FILLED_LIST_INDEX(i)], BUCKET_FILLED_LIST_BIT_SHIFT(i)))
        {
            continue;
        }
        ChainedListNode *currentCell = &potential->chainedLists[i];
        do
        {
            COORDINATE_TYPE x, y;
            x = currentCell->x;
            y = currentCell->y;

            char activeNeighbourCount = isInBucket(bucket, x - 1, y - 1) +
                                        isInBucket(bucket, x - 1, y) +
                                        isInBucket(bucket, x - 1, y + 1) +
                                        isInBucket(bucket, x, y - 1) +
                                        isInBucket(bucket, x, y + 1) +
                                        isInBucket(bucket, x + 1, y - 1) +
                                        isInBucket(bucket, x + 1, y) +
                                        isInBucket(bucket, x + 1, y + 1);

            if (isInBucket(bucket, x, y))
            {
                if (activeNeighbourCount == 2 || activeNeighbourCount == 3)
                {
                    addToBucket(bucketNext, x, y);
                }
                else
                {
                    int dy;
                    for (dy = -1; dy <= 1; dy++)
                    {
                        int dx;
                        for (dx = -1; dx <= 1; dx++)
                        {
                            addToBucket(potentialNext, x + dx, y + dy);
                        }
                    }
                }
            }
            else
            {
                if (activeNeighbourCount == 3)
                {
                    addToBucket(bucketNext, x, y);

                    int dy;
                    for (dy = -1; dy <= 1; dy++)
                    {
                        int dx;
                        for (dx = -1; dx <= 1; dx++)
                        {
                            addToBucket(potentialNext, x + dx, y + dy);
                        }
                    }
                }
            }

            currentCell = currentCell->next;
        } while (currentCell != NULL);
    }
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
        NEWnextStep();
        changed = 1;
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    if (changed)
    {
        displayCells();
        changed = 0;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    freeBucket(bucket);
    free(bucket);
    freeBucket(bucketNext);
    free(bucketNext);
    freeBucket(potentialNext);
    free(potentialNext);
}
