#ifndef QUICKSORT
#define QUICKSORT
template< typename T>
void quicksort_rec( T* a, unsigned begin, unsigned end );

template< typename T>
void quicksort_iterative( T* a, unsigned begin, unsigned end );

template< typename T>
void quicksort(T* a, unsigned, unsigned, int);

#include "quicksort.cpp"
#endif
