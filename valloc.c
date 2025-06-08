/**
 *
 *
 */

#include "valloc.h"
#include <stdio.h>
#include <stddef.h>

uint8_t VALLOC_BUFFER [VALLOC_BUFFERSIZE];
VallocMemBlock_TypeDef VALLOC_MEMBLOCKS [VALLOC_MAXALLOC +1]; // First One is Dummy, it shows the VallocBuffer's head, Length calculates the free memory of buffer

static uint32_t valloc_free_blocks = VALLOC_MAXALLOC;
static uint32_t valloc_free_memory_size = VALLOC_BUFFERSIZE;

static VallocMemBlock_TypeDef * _memblock_add (size_t Size);
static int32_t _memblock_del (VallocMemBlock_TypeDef * pMemBlock);

int32_t v_alloc_init ()
{
  size_t i;

  VALLOC_MEMBLOCKS [0].pHead = (void *)& VALLOC_BUFFER[0];
  VALLOC_MEMBLOCKS [0].Length = 0;
  VALLOC_MEMBLOCKS [0].valloc_memlock_chain_s.pLink = (void *)0x00;

  for (i = 0; i < VALLOC_BUFFERSIZE; ++i)
  {
    VALLOC_BUFFER[i] = 0x00;
  }

  for (i = 1; i < VALLOC_MAXALLOC; ++i)
  {
    VALLOC_MEMBLOCKS[i].pHead = (void *)0x00;
    VALLOC_MEMBLOCKS[i].Length = 0x00;
    VALLOC_MEMBLOCKS[i].valloc_memlock_chain_s.pLink = (void *)0x00;
  }

  return 0;
}

void * v_alloc (size_t Size)
{
  VallocMemBlock_TypeDef * pTemp;
  pTemp = _memblock_add (Size);

  if (pTemp == NULL) {
    return NULL;
  }

  return pTemp->pHead;
}

void v_alloc_free (void * pPointer)
{
  size_t i = 0;

  if (pPointer == NULL) {
      return;
  }

  for (i = 0; i < VALLOC_MAXALLOC; ++i)
  {
    if (VALLOC_MEMBLOCKS[i].pHead == pPointer && VALLOC_MEMBLOCKS[i].Length != 0)
    {
      break;
    }
  }

  if (i < VALLOC_MAXALLOC) {
    _memblock_del ((VallocMemBlock_TypeDef *)& VALLOC_MEMBLOCKS[i]);
  }
}

static VallocMemBlock_TypeDef * _memblock_add (size_t Size)
{
  size_t i;

  // check if block is available
  if (valloc_free_blocks == 0)
  {
    return (void*)0x00;
  }

  // seek for new empty memblock in the array
  VallocMemBlock_TypeDef *pNew;
  for (i = 0, pNew = & VALLOC_MEMBLOCKS [i]; 
      ( i < VALLOC_MAXALLOC ) && ( pNew->pHead != (void *)0x00 );
      ++i, pNew = & VALLOC_MEMBLOCKS [i]);


  uint32_t spaceBetweenBlocks, spaceBetweenBlocksMemory = -1;
  VallocMemBlock_TypeDef * pLeftOptimal, * pLeft, * pRight;

  pLeft = & VALLOC_MEMBLOCKS [0]; // head of mem blocks
  pLeftOptimal = pLeft;
  pRight = (VallocMemBlock_TypeDef *) pLeft->valloc_memlock_chain_s.pLink;

  // seek for memory block which is free
  while (pRight != (void *)0x00 && pRight->pHead != (void*)0x00)
  {
    // find memspace between two blocks
    spaceBetweenBlocks = __VALLOC_ABS( ((uint8_t *)pLeft->pHead + pLeft->Length) - ((uint8_t *)pRight->pHead) );
    if( spaceBetweenBlocks >= (uint32_t)Size)
    {
      if (spaceBetweenBlocksMemory > spaceBetweenBlocks)
      {
        pLeftOptimal = pLeft;
        spaceBetweenBlocksMemory = spaceBetweenBlocks;
      }
    }
    pLeft   = pRight;//(VallocMemBlock_TypeDef *) pLeft->valloc_memlock_chain_s.pLink;
    pRight = (VallocMemBlock_TypeDef *) pLeft->valloc_memlock_chain_s.pLink;
  }

  if (spaceBetweenBlocksMemory != -1U)
  {
    pLeft = pLeftOptimal;
  }
  pRight  = (VallocMemBlock_TypeDef *) pLeft->valloc_memlock_chain_s.pLink;

  pNew->pHead = ((uint8_t *)pLeft->pHead + pLeft->Length);
  pNew->Length = Size;
  pNew->valloc_memlock_chain_s.pLink = pRight;
  pLeft->valloc_memlock_chain_s.pLink = pNew;

  valloc_free_blocks -= 1;
  valloc_free_memory_size -= Size;
  return pNew;
}

static int32_t _memblock_del (VallocMemBlock_TypeDef * pMemBlock)
{
  VallocMemBlock_TypeDef * pLeft;
  
  pLeft = & VALLOC_MEMBLOCKS [0];

  while ((void *) pLeft != (void *) 0x00 && pLeft->valloc_memlock_chain_s.pLink != pMemBlock)
  {
    pLeft = pLeft->valloc_memlock_chain_s.pLink;
  }

  if ((void *)0x00 == pLeft)
  {
    return -1;
  }

  // Unlink the block
  pLeft->valloc_memlock_chain_s.pLink = pMemBlock->valloc_memlock_chain_s.pLink;

  valloc_free_memory_size += pMemBlock->Length;
  valloc_free_blocks += 1;

  // Clear the freed block's data
  pMemBlock->pHead = (void *)0x00;
  pMemBlock->Length = 0x00;
  pMemBlock->valloc_memlock_chain_s.pLink = (void *) 0x00;

  return 0;
}
