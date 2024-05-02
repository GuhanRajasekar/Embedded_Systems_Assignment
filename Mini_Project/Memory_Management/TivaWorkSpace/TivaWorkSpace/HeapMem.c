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

// Gets memory heap from the system and initializes it. 
// This memory in turn will be used for all mem_alloc() functions in multiples of chunks.
// Chunk size can be 16,32,64,128

// DOUBT: Here is it upto us to decide the pStartAddr, iHeapSize and iUnitSize? If so what are the values that we need to initialize it with ? 
int HeapMemInit(void *pStartAddr, int iHeapSize, int iUnitSize)
{
	if(HeapInitFlag ==0)  // The flag check makes sure that heap memory initialization is done only once 
	{
		pHeapStartAddr = pStartAddr;   // The starting address of the heap memory section is mentioned in "pStartAddr"
		HeapSize = iHeapSize;          // Total size of the requested memory is mentioned in "iHeapSize"

		//iUnitSize indicates the units in which the requested memory chunk will be allocated.
		// For example if iHeapSize is 127 and iUnitsize is 64, then 2 blocks of 64 bytes will be allocated 
		if(iUnitSize == 16||iUnitSize == 32||iUnitSize == 64||iUnitSize == 128)
			HeapUnitSize = iUnitSize;  
		else HeapUnitSize = 16;        // Default value of HeapUnitSize if iUnitSize is not 16,32,64 or 128

		BlockHeaderSize = sizeof(BLOCK_HEADER); // BlockHeaderSize is the size of the structure BLOCK_HEADER (32 bytes when compiled with VSCode)
       
        // Need to maintain two lists : "Used Block" and "Free Block"
		// These blocks are initialized as follows
		psFirstUsedBlock = (BLOCK_HEADER *)pHeapStartAddr; //psFirstUsedBlock stores the starting address of the heap memory section
		psFirstUsedBlock->uiBlockID = uiBlockID++; // psFirstUsedBlock->uiBlockID = 0. uiBlockID is unique for each block of memory
		psFirstUsedBlock->pParentBlock = NULL; // First used block of memory has no parent block
		psFirstUsedBlock->pChildBlock = NULL;  // First used block of memory is initialized with no child
		psFirstUsedBlock->uiSize = 0;          // 

        // another way of saying this would be psFirstFreeBlock = psFirstUsedBlock + 1
		psFirstFreeBlock = (BLOCK_HEADER *)(pHeapStartAddr+BlockHeaderSize); //psFirstFreeBlock will point to the location after psFirstUsedBlock
		psFirstFreeBlock->uiBlockID = uiBlockID++; // psFirstFreeBlock->uiBlockID = 1
		psFirstFreeBlock->pParentBlock = NULL; // First Free block of memory has no parent block
		psFirstFreeBlock->pChildBlock = NULL;  // First Free block of memory is initialized with no child

		// uiSize of psFirstFreeBlock indicates the total available memory after the initialization of the first free block and the first used block
		psFirstFreeBlock->uiSize = HeapSize - (BlockHeaderSize*2); 


		HeapInitFlag =1;     // To make sure that heap memory intialization is done only once
		return HEAP_SUCCESS; // HEAP_SUCCESS is a macro with value 1
	}
	else return HEAP_ERROR;  // HEAP_ERROR   is a macro with value 0  
}

//Allocate number of chunks from the available memory pool such that 
//allocated memory is greater than or equal to no_of_bytes that is requested by the user
void * MemAlloc(int iNumBytes)
{
	BLOCK_HEADER *FreeBlock,*NewFreeBlock,*NewUsedBlock; // These are all pointers to structure BLOCK_HEADER
	void *pData; // void pointer
	int iSize;   
    
    // iNumBytes is the size of the requested memory in bytes
	// iSize is the allocated size of memory which is in multiples of " HeapUnitSize "
	// iSize will always be greater than or equal to iNumBytes 
	// Because allocating memory greater than or equal to the requested amount is OK but allocating less than the requested amount is NOT OK
	iSize = FindAllocSize(iNumBytes);

	FreeBlock =(BLOCK_HEADER *)FindUsableFreeBlock(iSize);
	if(FreeBlock == NULL)
		return NULL;
	else
	{
		// Available memory space >= (allocated memory + BlockHeaderSize)
		// The allocated memory must have a label (BlockHeaderSize) attached to it. 
		// Hence while allocating memory, we check if we have enough memory for "user requested space(iSize)" and a label(BlockHeaderSize) for the user requested space
		if(FreeBlock->uiSize >= (iSize + BlockHeaderSize))
		{
			//iSize + BlockHeaderSize will be allocated to the user (user requested space + label for the user requested space)
			// the NewFreeBlock is the location after that , hence NewFreeBlock = (Old Free Block) + (User Requested Space) + (Label for the user requested space)
			NewFreeBlock = (BLOCK_HEADER *)(((unsigned int)FreeBlock) + iSize + BlockHeaderSize); // NewFreeBlock points to the location of (Intial free block + requested memory space)
			
			// Here we set up the connection between the FreeBlock and NewFreeBlock. NewFreeBlock is made the child node of FreeBlock
			// Available free size is now updated as (old available size) - ((user requested space) + (label for the user requested space)) => Third argument in the InsertBlock Function
			InsertBlock(FreeBlock,NewFreeBlock,(FreeBlock->uiSize - (iSize + BlockHeaderSize)));  
			DeleteBlock(FreeBlock, FREE_BLOCK); // Since a section has been used, it must be removed from "free" block section
			NewUsedBlock = FreeBlock; // What was originally FreeBlock now becomes the "NewUsedBlock". 

			// Now we need to make the connection between the first used block and the newused block
			// NewUsedBlock becomes the child node of the psFirstUsedBlock (Recall that originally child node of psFirstUsedBlock was set to NULL in HeapMemInit() function ) 
			InsertBlock(psFirstUsedBlock,NewUsedBlock,iSize);

	        // We return NewUsedBlock + BlockHeaderSize because every allocated chunk of memory will have a label (BlockHeaderSize)
			// The user can use the portion that is present after the label (BlockHeaderSize)
			// Hence we return NewUsedBlock(Starting point) + BlockHeaderSize
			pData = (void*)((unsigned int)NewUsedBlock + BlockHeaderSize); 
			return pData;
		}

		// Available memory space < (allocated memory + BlockHeaderSize)
		else
		{
			iSize = FreeBlock->uiSize; // Set iSize to the available size
			if(HEAP_SUCCESS == DeleteBlock(FreeBlock, FREE_BLOCK))
			{ 
				NewUsedBlock = FreeBlock; // // What was originally FreeBlock now becomes the "NewUsedBlock". 

			    // Now we need to make the connection between the first used block and the newused block
			    // NewUsedBlock becomes the child node of the psFirstUsedBlock				
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


// Output snapshot of memory usauge. Prints for each block: size in chunks and status (allocated/free)  
// This function prints the chunks of memory present in used block and free blocks 
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

	NewBlock->uiBlockID = uiBlockID++;      // incrementing the block ID for the new block of memory
	NewBlock->pParentBlock = CurrentBlock;  // Current block becomes the parent block for the new block
	NewBlock->pChildBlock = CurrentBlock->pChildBlock;

	// Similar to inserting a node in a linked list
	if(CurrentBlock->pChildBlock != NULL)
	{
		ChildBlock = (BLOCK_HEADER *)CurrentBlock->pChildBlock;
		ChildBlock->pParentBlock = NewBlock;
	}
	NewBlock->uiSize = iSize;

	CurrentBlock->pChildBlock = NewBlock ;

	// This function changes " CurrentBlock -> ChildBlock"  to " CurrentBlock -> NewBlock -> ChildBlock" 

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
		// Similar to deleting the first node of a linkedlist
		if(Block->pParentBlock == NULL) // if it was the first block (first block has no parent block)
		{
			ChildBlock = (BLOCK_HEADER *)(Block->pChildBlock);
			ChildBlock->pParentBlock = NULL;
			if(BlockType == FREE_BLOCK)
				 psFirstFreeBlock = ChildBlock;
			else if(BlockType == USED_BLOCK)
				 psFirstUsedBlock = ChildBlock;
		}
		
		// Similar to deleting the last node of a linked list
		else if (Block->pChildBlock == NULL)// if it was the last block (last block has no child block)
		{
			ParentBlock = (BLOCK_HEADER *)(Block->pParentBlock);
			ParentBlock->pChildBlock = NULL;
		}

		// Similar to deleting an intermediate node of the linked list
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
	BLOCK_HEADER *CurrentBlock;  // Current Block is a pointer pointing to a vriable of structure type BLOCK_HEADER
    
	// The "psFirstFreeBlock" was initialized with the location of the first free memory section in the function HeapMemInit()
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
    
	// HeapUnitSize is initialized in the function HeapMemInit()
	Count = iNumBytes /HeapUnitSize;

	// Assume iNumBytes = 127 and HeapUnitSize = 64. In this case Count = 127/64 = 1 as Count is "int" type variable
	// But for 127 bytes of memory , we need two units of "64" bytes of memory (extra space is OK but less than what is required is NOT OK)
	// Hence, in this case iNumBytes > (Count*HeapUnitSize) => 127 > (1*64) => Count is made 2
	// And we return (Count*HeapUnitSize) => (2*64) => 128 bytes of memory
	// So this function makes sure that we always return a chunk of memory that is greater than or equal to what is required, but never below it.  
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
