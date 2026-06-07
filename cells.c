#include <stdio.h>
#include <stdlib.h>

#include "cells.h"

/// @brief Hashes the coordinate into a unique hash, composed of the y-coordinate in the most significant digits and the x-coordinate in the least significant bits
/// @param x The x-coordinate of the cell
/// @param y The y-coordinate of the cell
/// @return The hash
static CELL_COORDINATE_TYPE hashCoordinates(COORDINATE_TYPE x, COORDINATE_TYPE y)
{
    return (y << CELL_COORDINATE_Y_BIT_SHIFT) | x;
}

/// @brief Adds the element of coordinates (x;y) to the chained lists of the bucket corresponding to the coordinates' hash
/// @param bucket The address pointing to the bucket
/// @param x The x-coordinate of the cell
/// @param y The y-coordinate of the cell
void addToBucket(Bucket *bucket, COORDINATE_TYPE x, COORDINATE_TYPE y)
{
    CELL_COORDINATE_TYPE hash = hashCoordinates(x, y);
    unsigned int chainedListIndex = hash % BUCKET_SIZE;
    ChainedListNode *currentElement = &bucket->chainedLists[chainedListIndex];
    while (currentElement->next != NULL && currentElement->coordinates != hash)
    {
        currentElement = currentElement->next;
    }

    if (currentElement->coordinates == hash)
    {
        return;
    }
    
    if (IS_BIT_PRESENT(bucket->areFilled[BUCKET_FILLED_LIST_INDEX(chainedListIndex)], BUCKET_FILLED_LIST_BIT_SHIFT(chainedListIndex)))
    {
        // Chained list has already been initialized
        ChainedListNode *newItem = (ChainedListNode *)calloc(1, sizeof(ChainedListNode));
        newItem->coordinates = hash;
        currentElement->next = newItem;
    }
    else
    {
        // Chained list has not been initiliazed
        bucket->areFilled[BUCKET_FILLED_LIST_INDEX(chainedListIndex)] |= 1 << (BUCKET_FILLED_LIST_BIT_SHIFT(chainedListIndex));
        currentElement->coordinates = hash;
    }
    // currentElement now holds the last element of the chained list
    // No need to set the next of newItem to NULL since it was initialised to 0, corresponding to NULL
}

/// @brief Verify if the element of coordinates (x;y) is in the bucket or not
/// @param bucket The address pointing to the bucket
/// @param x The x-coordinate of the cell
/// @param y The y-coordinate of the cell
/// @return 1 if the cell is in the bucket, 0 otherwise
int isInBucket(Bucket *bucket, COORDINATE_TYPE x, COORDINATE_TYPE y)
{
    CELL_COORDINATE_TYPE hash = hashCoordinates(x, y);
    unsigned int chainedListIndex = hash % BUCKET_SIZE;
    if (!IS_BIT_PRESENT(bucket->areFilled[BUCKET_FILLED_LIST_INDEX(chainedListIndex)], BUCKET_FILLED_LIST_BIT_SHIFT(chainedListIndex))) return 0;
    ChainedListNode *currentElement = &bucket->chainedLists[chainedListIndex];
    while (currentElement->next != NULL && currentElement->coordinates != hash)
    {
        currentElement = currentElement->next;
    }

    if (currentElement->coordinates == hash)
    {
        return 1;
    }
    return 0;
}

/// @brief Returns the next initialized chained list starting from the one whose index is startIndex
/// @param bucket A pointer to the bucket
/// @param startIndex A pointer to the index of the index to search from
/// @return A pointer to the next chained list node if it exists, NULL otherwise
ChainedListNode* getNextChainedList(Bucket *bucket, uint64_t *startIndex)
{
    int i = *startIndex;
    while (i < BUCKET_SIZE && !IS_BIT_PRESENT(bucket->areFilled[BUCKET_FILLED_LIST_INDEX(i)], BUCKET_FILLED_LIST_BIT_SHIFT(i)))
    {
        i++;
    }
    *startIndex = i;
    return (i < BUCKET_SIZE ? &bucket->chainedLists[i] : NULL);
}

Bucket* createBucket()
{
    Bucket* bucket = (Bucket*)malloc(sizeof(Bucket));

    int i;
    for (i = 0; i < BUCKET_FILLED_LIST_LENGTH; i++)
    {
        bucket->areFilled[i] = 0;
    }

    return bucket;

}

void deepCopy(Bucket *dst, Bucket *src)
{
    int i;
    for (i = 0; i < BUCKET_FILLED_LIST_LENGTH; i++)
    {
        dst->areFilled[i] = src->areFilled[i];
    }
    
    for (i = 0; i < BUCKET_SIZE; i++)
    {
        if (!IS_BIT_PRESENT(src->areFilled[BUCKET_FILLED_LIST_INDEX(i)], BUCKET_FILLED_LIST_BIT_SHIFT(i)))
        {
            dst->chainedLists[i].coordinates = 0;
            dst->chainedLists[i].next = NULL;
        }
        else
        {
            // Free the current elements
            freeChainedList(dst->chainedLists[i].next);

            // Assign new ones
            ChainedListNode *currentSrcCell = &src->chainedLists[i];
            ChainedListNode *lastDstCell = &dst->chainedLists[i];
            lastDstCell->coordinates = currentSrcCell->coordinates;
            ChainedListNode *currentDstCell;
            while (currentSrcCell != NULL) {
                currentDstCell = (ChainedListNode*)calloc(1, sizeof(ChainedListNode));
                currentDstCell->coordinates = currentSrcCell->coordinates;
                lastDstCell->next = currentDstCell;

                lastDstCell = currentDstCell;
                currentSrcCell = currentSrcCell->next;
            }
        }
    }
}

static void freeChainedList(ChainedListNode* chainedListStart)
{
    if (chainedListStart == NULL) return;
    ChainedListNode *next = chainedListStart->next;
    free(chainedListStart);
    freeChainedList(next);
}

void freeBucket(Bucket* bucket)
{
    ChainedListNode currentChainedList;
    int i;
    for (i = 0; i < BUCKET_SIZE; i++)
    {
        if (!IS_BIT_PRESENT(bucket->areFilled[BUCKET_FILLED_LIST_INDEX(i)], BUCKET_FILLED_LIST_BIT_SHIFT(i)))
            continue;
        currentChainedList = bucket->chainedLists[i];
        // We don't want to free the current element since it is on the stack and not the heap
        freeChainedList(currentChainedList.next);
    }
}
