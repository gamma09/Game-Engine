#ifndef GLOBAL_H
#define GLOBAL_H

#include "mem.h"

// forward declaration
class Heap;

   // overloaded new/delete
   void * operator new( size_t inSize, Heap *inHeap, Align align, const char * const inName, int lineNum);
   void operator delete( void *p, Heap *inHeap, Align align, const char * const inName, int lineNum );

   void * operator new[]( size_t inSize ,Heap *pHeap, Align align, const char * const inName, int lineNum );
   void operator delete[]( void *p,Heap *pHeap, Align align, const char * const inName, int lineNum );

   // Your new system needs to coexist with existing new/delete()
   // simple new/delete is used in the unit test fixture.
   // welcome to memory system development

   void * operator new( size_t inSize );
   void operator delete( void *p );

   void * operator new[]( size_t inSize );
   void operator delete[]( void *p );

#endif