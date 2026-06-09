#include <stdio.h>
#include <stdlib.h>

#include "cells.h"

static CELL_COORDINATE_TYPE hashCoordinates(COORDINATE_TYPE x, COORDINATE_TYPE y);
static void freeChainedList(ChainedListNode* chainedListStart);

/// @brief Hashes the coordinate into a unique hash, composed of the y-coordinate in the most significant digits and the x-coordinate in the least significant bits
/// @param x The x-coordinate of the cell
/// @param y The y-coordinate of the cell
/// @return The hash
static CELL_COORDINATE_TYPE hashCoordinates(COORDINATE_TYPE x, COORDINATE_TYPE y)
{
    uint32_t result = 0;

    uint16_t ux = (uint16_t)x;
    uint16_t uy = (uint16_t)y;

    for (int i = 0; i < 16; i++) {
        int shift = 15 - i;

        result |= ((uint32_t)((uy >> shift) & 1)) << (31 - 2 * i);
        result |= ((uint32_t)((ux >> shift) & 1)) << (30 - 2 * i);
    }

    return result;
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
    while (currentElement->next != NULL && (currentElement->x != x || currentElement->y != y))
    {
        currentElement = currentElement->next;
    }

    if (currentElement->x == x && currentElement->y == y)
    {
        return;
    }
    
    if (IS_BIT_PRESENT(bucket->areFilled[BUCKET_FILLED_LIST_INDEX(chainedListIndex)], BUCKET_FILLED_LIST_BIT_SHIFT(chainedListIndex)))
    {
        // Chained list has already been initialized
        ChainedListNode *newItem = (ChainedListNode *)calloc(1, sizeof(ChainedListNode));
        newItem->x = x;
        newItem->y = y;
        currentElement->next = newItem;
    }
    else
    {
        // Chained list has not been initiliazed
        bucket->areFilled[BUCKET_FILLED_LIST_INDEX(chainedListIndex)] |= 1 << (BUCKET_FILLED_LIST_BIT_SHIFT(chainedListIndex));
        currentElement->x = x;
        currentElement->y = y;
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
    while (currentElement->next != NULL && (currentElement->x != x || currentElement->y != y))
    {
        currentElement = currentElement->next;
    }

    if (currentElement->x == x && currentElement->y == y)
    {
        return 1;
    }
    return 0;
}

/// @brief Allocates memory for a bucket on the heap
/// @return A pointer to the bucket
Bucket* createBucket()
{
    return (Bucket*)calloc(1, sizeof(Bucket));
}

/// @brief Deep-copies the bucket from src to dst, meaning src and dst are not related to each other.
/// @param dst A pointer to the destination bucket, needsto have allocated memory. If not empty, its content is freed
/// @param src A pointer to the source bucket, stays unchanged
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
            dst->chainedLists[i].x = 0;
            dst->chainedLists[i].y = 0;
            freeChainedList(dst->chainedLists[i].next);
            dst->chainedLists[i].next = NULL;
        }
        else
        {
            // Free the current elements
            freeChainedList(dst->chainedLists[i].next);

            // Assign new ones
            ChainedListNode *currentSrcCell = &src->chainedLists[i];
            ChainedListNode *lastDstCell = &dst->chainedLists[i];
            lastDstCell->x = currentSrcCell->x;
            lastDstCell->y = currentSrcCell->y;
            ChainedListNode *currentDstCell;
            while (currentSrcCell != NULL) {
                currentDstCell = (ChainedListNode*)calloc(1, sizeof(ChainedListNode));
                currentDstCell->x = currentSrcCell->x;
                currentDstCell->y = currentSrcCell->y;
                lastDstCell->next = currentDstCell;

                lastDstCell = currentDstCell;
                currentSrcCell = currentSrcCell->next;
            }
        }
    }
}

/// @brief Frees a chained list from the given element to the end
/// @param chainedListStart A pointer to the starting element of the chained list
static void freeChainedList(ChainedListNode* chainedListStart)
{
    if (chainedListStart == NULL) return;
    ChainedListNode *next = chainedListStart->next;
    free(chainedListStart);
    freeChainedList(next);
}

/// @brief Frees the memory allocated for the bucket and its content
/// @param bucket The pointer to the bucket
void freeBucket(Bucket* bucket)
{
    ChainedListNode currentChainedList;
    int i;
    for (i = 0; i < BUCKET_SIZE; i++)
    {
        if (!IS_BIT_PRESENT(bucket->areFilled[BUCKET_FILLED_LIST_INDEX(i)], BUCKET_FILLED_LIST_BIT_SHIFT(i)))
            continue;
        currentChainedList = bucket->chainedLists[i];
        // We don't want to free the current element since it is directly part of the bucket
        freeChainedList(currentChainedList.next);
    }
    free(bucket);
}
