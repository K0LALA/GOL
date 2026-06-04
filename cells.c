#include <stdio.h>
#include <stdlib.h>

#include "cells.h"

/// @brief Hashes the coordinate into a unique hash, composed of the y-coordinate in the most significant digits and the x-coordinate in the least significant bits
/// @param x The x-coordinate of the cell
/// @param y The y-coordinate of the cell
/// @return The hash
CELL_COORDINATE_TYPE hashCoordinates(COORDINATE_TYPE x, COORDINATE_TYPE y)
{
    return (y << CELL_COORDINATE_Y_BIT_SHIFT) | x;
}

// TODO: Add a list for each bucket, saying if the bucket is initialized or not, used in order to not alloc / free the same segments twice for the first chained lists elements

/// @brief Adds the element of coordinates (x;y) to the chained lists of the bucket corresponding to the coordinates' hash
/// @param bucket The address pointing to the start of the bucket
/// @param x The x-coordinate of the cell
/// @param y The y-coordinate of the cell
void addToBucket(ChainedListNode *bucket, COORDINATE_TYPE x, COORDINATE_TYPE y)
{
    CELL_COORDINATE_TYPE hash = hashCoordinates(x, y);
    unsigned int addressOffset = hash % BUCKET_SIZE;
    printf("Hash created");
    ChainedListNode currentElement = *(bucket + addressOffset);
    while (currentElement.next != NULL)
    {
        currentElement = *currentElement.next;
    }
    printf("Found last item");

    // currentElement now holds the last element of the chained list
    ChainedListNode *newItem = (ChainedListNode *)calloc(1, sizeof(ChainedListNode));
    newItem->coordinates = hash;
    // No need to set the next of newItem to NULL since it was initialised to 0, corresponding to NULL
    currentElement.next = newItem;
    printf("Initialized newItem");
}

/// @brief Verify if the element of coordinates (x;y) is in the bucket or not
/// @param bucket The address pointing to the start of the bucket
/// @param x The x-coordinate of the cell
/// @param y The y-coordinate of the cell
/// @return 1 if the cell is in the bucket, 0 otherwise
int isInBucket(ChainedListNode *bucket, COORDINATE_TYPE x, COORDINATE_TYPE y)
{
    CELL_COORDINATE_TYPE hash = hashCoordinates(x, y);
    unsigned int addressOffset = hash % BUCKET_SIZE;
    printf("Hash created");
    ChainedListNode currentElement = *(bucket + addressOffset);
    while (currentElement.next != NULL && currentElement.coordinates != hash)
    {
        currentElement = *currentElement.next;
    }

    printf("Found element or last");

    if (currentElement.coordinates == hash)
    {
        return 1;
    }
    return 0;
}

ChainedListNode* allocateBucket()
{
    printf("Allocating bucket");
    return (ChainedListNode*)calloc(BUCKET_SIZE, sizeof(ChainedListNode*));
}

void freeChainedList(ChainedListNode* chainedListStart)
{
    if (chainedListStart == NULL) return;
    ChainedListNode *next = chainedListStart->next;
    free(chainedListStart);
    freeChainedList(next);
}

void freeBucket(ChainedListNode* bucket)
{
    ChainedListNode *currentChainedList = bucket;
    int i;
    for (i = 0; i < BUCKET_SIZE; i++)
    {
        freeChainedList(currentChainedList++);
    }
    free(bucket);
}
