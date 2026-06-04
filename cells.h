#ifndef _CELLS_H
#define _CELLS_H 1

#include <stdint.h>

#define BUCKET_SIZE 4
#define COORDINATE_TYPE int16_t       // Can be negative, describes either x- or y-coordinate
#define CELL_COORDINATE_TYPE uint32_t // Cannot be negative, describes x and y coordinates at once
#define CELL_COORDINATE_BYTE_SIZE sizeof(CELL_COORDINATE_TYPE)
#define CELL_COORDINATE_Y_BIT_SHIFT CELL_COORDINATE_BYTE_SIZE * 4 // (CELL_COORDINAET_BYTE_SIZE / 2) * 8

/// @brief Describes one element of the bucket, which is a chained list node
typedef struct ChainedListNode ChainedListNode;
struct ChainedListNode
{
    CELL_COORDINATE_TYPE coordinates; // y-coordinate occupies the most significant bits, x-coordinate occupies the least significant bits
    ChainedListNode *next;
};

CELL_COORDINATE_TYPE hashCoordinates(COORDINATE_TYPE x, COORDINATE_TYPE y);
void addToBucket(ChainedListNode *bucket, COORDINATE_TYPE x, COORDINATE_TYPE y);
int isInBucket(ChainedListNode *bucket, COORDINATE_TYPE x, COORDINATE_TYPE y);
ChainedListNode* allocateBucket();
void freeChainedList(ChainedListNode* chainedListStart);
void freeBucket(ChainedListNode* bucket);

#endif /* _CELLS_H */