#ifndef _CELLS_H
#define _CELLS_H 1

#include <stdint.h>

#define IS_BIT_PRESENT(bytes, bit) (!~(bytes | ~(1 << bit)))

#define BUCKET_SIZE 65536

#define BUCKET_FILLED_LIST_LENGTH 4096                              // ceil(BUCKET_SIZE / 16), as many bits as elements count in bucket
#define BUCKET_FILLED_LIST_INDEX(index) ((int)(index / 16))         // Gives the index in the areFilled list given the index of the chained list
#define BUCKET_FILLED_LIST_BIT_SHIFT(index) (15 - index % 16)            // Gives the bit shift to access the bit relative to index, the index of the chained list

#define COORDINATE_TYPE int16_t                                     // Can be negative, describes either x- or y-coordinate
#define CELL_COORDINATE_TYPE uint32_t                               // Cannot be negative, describes x and y coordinates at once
#define CELL_COORDINATE_BYTE_SIZE sizeof(CELL_COORDINATE_TYPE)
#define CELL_COORDINATE_Y_BIT_SHIFT CELL_COORDINATE_BYTE_SIZE * 4   // (CELL_COORDINATE_BYTE_SIZE / 2) * 8

/// @brief Describes one element of the bucket, which is a chained list node
typedef struct ChainedListNode ChainedListNode;
struct ChainedListNode
{
    CELL_COORDINATE_TYPE coordinates;               // y-coordinate occupies the most significant bits, x-coordinate occupies the least significant bits
    ChainedListNode *next;                          
};

typedef struct Bucket Bucket;
struct Bucket {
    uint16_t areFilled[BUCKET_FILLED_LIST_LENGTH];  // List describing which chained lists are filled, 0 when empty, 1 when filled
    ChainedListNode chainedLists[BUCKET_SIZE];      // List of starting elements of chained lists
};

static CELL_COORDINATE_TYPE hashCoordinates(COORDINATE_TYPE x, COORDINATE_TYPE y);
void addToBucket(Bucket *bucket, COORDINATE_TYPE x, COORDINATE_TYPE y);
int isInBucket(Bucket *bucket, COORDINATE_TYPE x, COORDINATE_TYPE y);
ChainedListNode* getNextChainedList(Bucket *bucket, uint64_t *startIndex);
Bucket* createBucket();
void deepCopy(Bucket* dst, Bucket* src);
static void freeChainedList(ChainedListNode* chainedListStart);
void freeBucket(Bucket* bucket);

#endif /* _CELLS_H */