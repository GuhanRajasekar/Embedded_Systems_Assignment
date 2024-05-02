//*****************************************************************************************
// Author   : Tessin K Jose
// File Name    : HeapMem.h
// Description  : prtototypes for heap memory manipulation functions
// Dependency   : None
// Revision     : Release date      Simple Remark
//  1.0 :        1-02-2017         Initial version
//*****************************************************************************************

#ifndef HEAPMEM_H
#define HEAPMEM_H

#ifndef NULL
#define NULL 0
#endif

#define HEAP_ERROR  0
#define HEAP_SUCCESS    1

#define FREE_BLOCK  0
#define USED_BLOCK  1

typedef struct BlockHeaderType
{
    unsigned int uiBlockID;   // 4 bytes (in VSCode)
    void * pParentBlock;      // pointer to previous node in the linked list [8 bytes in VSCode]
    void * pChildBlock;       // pointer to enxt node in the linked list [8 bytes in VSCode]
    unsigned int uiSize;      // 4 bytes (in VSCode)
}BLOCK_HEADER;

//Gets memory heap from the system and initializes it. 
//This memory in turn will be used for all mem_alloc() functions in multiples of chunks.
// Chunk size can be 16,32,64,128
int HeapMemInit(void *pStartAddr, int iHeapSize, int iUnitSize);

//Allocate number of chunks from the available memory pool such that 
//allocated memory is greater than or equal to no_of_bytes
void * MemAlloc(int iNumBytes);

//Frees up the chunks (a block) of allocated memory and makes it available for the future use
int MemFree(void *pData);

//Output snapshot of memory usauge. Prints for each block: size in chunks and status (allocated/free)   
void HeapMemDump(void);

//Returns error if used_list is not NULL
//otherwise de-allocates the heap.
int HeapMemExit(void);

#endif // HEAPMEM_H








