//*****************************************************************************************
// Author  	: Tessin K Jose
// File Name 	: HeapMem.c
// Description  : Heap memory management lib
// Dependency 	: None
// Revision 	: Release date    	Simple Remark    	
//	1.0	: 			1-02-2017 		Initial version
//*****************************************************************************************

#include "HeapMem.h"

int InsertBlock(void *pCurrentBlock, void *pNewBlock, int iSize);
int DeleteBlock(void *pBlock, int BlockType);
void* FindUsableFreeBlock(int iSize);
int FindAllocSize(int iNumBytes);
void MergeFreeBlocks(void);

unsigned int uiBlockID=0;
int HeapInitFlag=0;
int HeapSize =0;
int HeapUnitSize =16;
void *pHeapStartAddr;
int BlockHeaderSize =16;

BLOCK_HEADER *psFirstUsedBlock,*psFirstFreeBlock;

//Gets memory heap from the system and initializes it. 
//This memory in turn will be used for all mem_alloc() functions in multiples of chunks.
// Chunk size can be 16,32,64,128
int HeapMemInit(void *pStartAddr, int iHeapSize, int iUnitSize)
{
	if(HeapInitFlag ==0)
	{
		pHeapStartAddr = pStartAddr;
		HeapSize = iHeapSize;
		if(iUnitSize == 16||iUnitSize == 32||iUnitSize == 64||iUnitSize == 128)
			HeapUnitSize = iUnitSize;
		else HeapUnitSize = 16;

		BlockHeaderSize = sizeof(BLOCK_HEADER);

		psFirstUsedBlock = (BLOCK_HEADER *)pHeapStartAddr;
		psFirstUsedBlock->uiBlockID = uiBlockID++;
		psFirstUsedBlock->pParentBlock = NULL;
		psFirstUsedBlock->pChildBlock = NULL;
		psFirstUsedBlock->uiSize = 0;


		psFirstFreeBlock = (BLOCK_HEADER *)(pHeapStartAddr+BlockHeaderSize);
		psFirstFreeBlock->uiBlockID = uiBlockID++;
		psFirstFreeBlock->pParentBlock = NULL;
		psFirstFreeBlock->pChildBlock = NULL;
		psFirstFreeBlock->uiSize = HeapSize - (BlockHeaderSize*2);


		HeapInitFlag =1;
		return HEAP_SUCCESS;
	}
	else return HEAP_ERROR;
}

//Allocate number of chunks from the available memory pool such that 
//allocated memory is greater than or equal to no_of_bytes
void * MemAlloc(int iNumBytes)
{
	BLOCK_HEADER *FreeBlock,*NewFreeBlock,*NewUsedBlock;
	void *pData;
	int iSize;

	iSize = FindAllocSize(iNumBytes);
	FreeBlock =(BLOCK_HEADER *)FindUsableFreeBlock(iSize);
	if(FreeBlock == NULL)
		return NULL;
	else
	{
		if(FreeBlock->uiSize >= (iSize + BlockHeaderSize))
		{
			NewFreeBlock = (BLOCK_HEADER *)(((unsigned int)FreeBlock) + iSize + BlockHeaderSize);
			InsertBlock(FreeBlock,NewFreeBlock,(FreeBlock->uiSize - (iSize + BlockHeaderSize)));
			DeleteBlock(FreeBlock, FREE_BLOCK);
			NewUsedBlock = FreeBlock;
			InsertBlock(psFirstUsedBlock,NewUsedBlock,iSize);
			pData = (void*)((unsigned int)NewUsedBlock + BlockHeaderSize);
			return pData;
		}
		else
		{
			iSize = FreeBlock->uiSize;
			if(HEAP_SUCCESS == DeleteBlock(FreeBlock, FREE_BLOCK))
			{
				NewUsedBlock = FreeBlock;
				InsertBlock(psFirstUsedBlock,NewUsedBlock,iSize);
				pData = (void*)((unsigned int)NewUsedBlock + BlockHeaderSize);
				return pData;
			}
			else
				return NULL;
		}
	}
}

//Frees up the chunks (a block) of allocated memory and makes it available for the future use
int MemFree(void *pData)
{
	BLOCK_HEADER *UsedBlock,*NewFreeBlock;
	int iSize;

	UsedBlock = (BLOCK_HEADER *)((unsigned int)pData - BlockHeaderSize);
	iSize = UsedBlock->uiSize;
	if(HEAP_SUCCESS == DeleteBlock(UsedBlock, USED_BLOCK))
	{
		NewFreeBlock = UsedBlock;
		InsertBlock(psFirstFreeBlock,NewFreeBlock,iSize);
		MergeFreeBlocks();
		return HEAP_SUCCESS;
	}
	else return HEAP_ERROR;
}


//Output snapshot of memory usauge. Prints for each block: size in chunks and status (allocated/free)   
void HeapMemDump(void)
{
	BLOCK_HEADER *UsedBlock,*FreeBlock;

	if(HeapInitFlag ==0)
		UARTprintf("\n No Heap mem exists \n");
	else
	{
		UsedBlock = psFirstUsedBlock;
		UARTprintf("\n Used Blocks \n");
		do
		{
			UARTprintf("Block ID = %d, Start Addr = %08x , Size = %d\n",UsedBlock->uiBlockID,UsedBlock,UsedBlock->uiSize);
			UsedBlock = (BLOCK_HEADER *)(UsedBlock->pChildBlock);
		}
		while (UsedBlock != NULL);

		FreeBlock = psFirstFreeBlock;
		UARTprintf("\n Free Blocks \n");

		do
		{
			UARTprintf("Block ID = %d, Start Addr = %08x , Size = %d\n",FreeBlock->uiBlockID,FreeBlock,FreeBlock->uiSize);
			FreeBlock = (BLOCK_HEADER *)(FreeBlock->pChildBlock);
		}
		while (FreeBlock != NULL);
	}

}

//Returns error if used_list is not NULL
//otherwise de-allocates the heap.
int HeapMemExit(void)
{
	if((psFirstUsedBlock->pParentBlock == NULL) && (psFirstUsedBlock->pChildBlock == NULL))
	{
		HeapInitFlag =0;
		return HEAP_SUCCESS;
	}
	else return HEAP_ERROR;

}

int InsertBlock(void *pCurrentBlock, void *pNewBlock, int iSize)
{
	BLOCK_HEADER *CurrentBlock,*NewBlock,*ChildBlock;

	CurrentBlock = (BLOCK_HEADER *)pCurrentBlock;
	NewBlock = (BLOCK_HEADER *)pNewBlock;

	NewBlock->uiBlockID = uiBlockID++;
	NewBlock->pParentBlock = CurrentBlock;
	NewBlock->pChildBlock = CurrentBlock->pChildBlock;
	if(CurrentBlock->pChildBlock != NULL)
	{
		ChildBlock = (BLOCK_HEADER *)CurrentBlock->pChildBlock;
		ChildBlock->pParentBlock = NewBlock;
	}
	NewBlock->uiSize = iSize;

	CurrentBlock->pChildBlock = NewBlock ;

	return HEAP_SUCCESS;
}

int DeleteBlock(void *pBlock,  int BlockType)
{
	BLOCK_HEADER *Block,*ParentBlock,*ChildBlock;

	Block = (BLOCK_HEADER *)pBlock;

	if((Block->pParentBlock == NULL) && (Block->pChildBlock == NULL)) //Pivot Block Cannot be deleted
	{
		return HEAP_ERROR;
	}
	else
	{
		if(Block->pParentBlock == NULL) // if it was the first block
		{
			ChildBlock = (BLOCK_HEADER *)(Block->pChildBlock);
			ChildBlock->pParentBlock = NULL;
			if(BlockType == FREE_BLOCK)
				 psFirstFreeBlock = ChildBlock;
			else if(BlockType == USED_BLOCK)
				 psFirstUsedBlock = ChildBlock;
		}
		else if (Block->pChildBlock == NULL)// if it was the last block
		{
			ParentBlock = (BLOCK_HEADER *)(Block->pParentBlock);
			ParentBlock->pChildBlock = NULL;
		}
		else
		{
			ChildBlock = (BLOCK_HEADER *)(Block->pChildBlock);
			ParentBlock = (BLOCK_HEADER *)(Block->pParentBlock);
			ChildBlock->pParentBlock = ParentBlock;
			ParentBlock->pChildBlock = ChildBlock;
		}
		return HEAP_SUCCESS;
	}
}

void* FindUsableFreeBlock(int iSize)
{
	BLOCK_HEADER *CurrentBlock;

	CurrentBlock = psFirstFreeBlock;
	while (CurrentBlock->uiSize < iSize)
	{
		if(CurrentBlock->pChildBlock == NULL)
		{
			return NULL;
		}
		else
		{
			CurrentBlock = (BLOCK_HEADER *)(CurrentBlock->pChildBlock);
		}
	}
	return CurrentBlock;
}

int FindAllocSize(int iNumBytes)
{
	int Count;

	Count = iNumBytes /HeapUnitSize;
	if(iNumBytes > (Count*HeapUnitSize))Count += 1;

	return (Count*HeapUnitSize);
}

void MergeFreeBlocks(void)
{
	BLOCK_HEADER *CurrentBlock, *NextBlock;
	unsigned int CurrentBlockEnd;

	CurrentBlock = psFirstFreeBlock;

	while (CurrentBlock != NULL)
	{
		CurrentBlockEnd = (((unsigned int)CurrentBlock) + BlockHeaderSize + CurrentBlock->uiSize);
		NextBlock = psFirstFreeBlock;
		while (NextBlock != NULL)
		{
			if((unsigned int)NextBlock == CurrentBlockEnd)
			{
				CurrentBlock->uiSize += (NextBlock->uiSize + BlockHeaderSize);
				DeleteBlock(NextBlock, FREE_BLOCK);
				break;
			}
			NextBlock = (BLOCK_HEADER *)(NextBlock->pChildBlock);
		}
		CurrentBlock = (BLOCK_HEADER *)(CurrentBlock->pChildBlock);
	}
}
