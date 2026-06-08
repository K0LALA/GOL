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

static char stepping = 0;

static unsigned long long iterationCount = 0;

#define PIXEL_SIZE 1

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

void initPattern(char* fileName, int offsetX, int offsetY)
{
    FILE* patternFile = fopen(fileName, "r");
    if (patternFile == NULL)
    {
        printf("Couldn't read file.\n");
        return;
    }

    char character;
    char finishLine = 0;
    unsigned int run_count = 0;
    unsigned int x = 0;
    unsigned int y = 0;
    while (character != EOF)
    {
        character = fgetc(patternFile);
        if (finishLine)
        {
            if (character == '\n')
            {
                finishLine = 0;
            }
        }
        // Comment
        else if (character == '#')
        {
            finishLine = 1;
        }
        // Cell count
        else if ((int)'0' <= character && character <= (int)'9')
        {
            run_count *= 10;
            run_count += character - '0';
        }
        // Dead cell
        else if (character == 'b')
        {
            x += run_count ? run_count : 1;
            run_count = 0;
        }
        // Alive cell
        else if (character == 'o')
        {
            run_count = run_count ? run_count : 1;
            int dx;
            for (dx = 0; dx < run_count; dx++)
            {
                addCell(offsetX + x + dx, offsetY + y);
            }
            x += run_count;
            run_count = 0;
        }
        // New line
        else if (character == '$')
        {
            x = 0;
            y += run_count ? run_count : 1;
            run_count = 0;
        }
        // End of pattern
        else if (character == '!')
        {
            fseek(patternFile, 0, SEEK_END);
            break;
        }
    }
    fclose(patternFile);
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
    

    if (argc == 4)
    {
        initPattern(argv[1], atoi(argv[2]), atoi(argv[3]));
    }
    else if (argc == 2)
    {
        initPattern(argv[1], 0, 0);
    }
    else
    {
        initCells();
    }

    displayCells();

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

void nextStep()
{
    iterationCount++;
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
        switch (event->key.key)
        {
        case SDLK_ESCAPE:
            return SDL_APP_SUCCESS;

        case SDLK_SPACE:
            stepping = !stepping;
            break;

        case SDLK_S:
            if (!stepping)
            {
                nextStep();
                displayCells();
            }
            break;
        
        default:
            break;
        }
    }
    else if (event->type == SDL_EVENT_WINDOW_RESIZED)
    {
        displayCells();
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    if (stepping)
    {
        nextStep();
        displayCells();
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    printf("%llu\n", iterationCount);
    freeBucket(bucket);
    free(bucket);
    freeBucket(bucketNext);
    free(bucketNext);
    freeBucket(potentialNext);
    free(potentialNext);
}
