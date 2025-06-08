/**
 *
 *
 *
 */

#ifndef VALLOC_H
#define VALLOC_H

#ifdef __cplusplus
extern "C" 
{
#endif

#define UNUSED(X) (void)X

#include <stdint.h>
#include <stddef.h>
#ifndef VALLOC_BUFFERSIZE
#define 	VALLOC_BUFFERSIZE (10240U)
#endif // VALLOC_BUFFERSIZE

#ifndef VALLOC_MAXALLOC
#define VALLOC_MAXALLOC (128U) // VALLOC_BUFFERSIZE 
#endif // VALLOC_MAXALLOC

#define __VALLOC_ABS(__X) ( ((__X) < 0) ? (-1*(__X)) :(__X) )

  typedef struct valloc_memblock_s
  {
    void * pHead;
    size_t Length;

    struct     
    {
      void * pLink;
    } valloc_memlock_chain_s;

  } VallocMemBlock_TypeDef;


  int32_t v_alloc_init ();

  void * v_alloc (size_t Size);
  void v_alloc_free (void * pPointer);


#ifdef __cplusplus
}
#endif

#endif // VALLOC_H
