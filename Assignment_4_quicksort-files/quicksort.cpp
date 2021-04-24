#include <iostream>
#include "quicksort.h"
#include "sort_small_arrays.h"

template< typename T>
unsigned partition( T* a, unsigned begin, unsigned end) {
	unsigned i = begin, last = end-1;
	T pivot = a[last];

	for (unsigned j=begin; j<last; ++j) {
		if ( a[j]<pivot ) {
			std::swap( a[j], a[i] );
			++i;
		}
	}
	std::swap( a[i], a[last] );
	return i;
}

template< typename T>
unsigned partition_new( T* a, unsigned begin, unsigned end) {
    if ( end-begin > 8 ) return partition_old( a, begin, end );

	unsigned i = begin, last = end-1, step = (end-begin)/4;

    T* pivots[5] = { a+begin, a+begin+step, a+begin+2*step, a+begin+3*step, a+last };
    quicksort_base_5_pointers( pivots );

	std::swap( a[last], a[begin+2*step] );
	T pivot = a[last];
    
    for (unsigned j=begin; j<last; ++j) {
		if ( a[j]<pivot /*|| a[j]==pivot*/ ) {
			std::swap( a[j], a[i] );
			++i;
		}
	}
	std::swap( a[i], a[last] );
	return i;
}

/* recursive */
template< typename T>
void quicksort_rec( T* a, unsigned begin, unsigned end )
{
    if ( end-begin<6 ) {
        switch ( end-begin ) {
            case 5: quicksort_base_5( a+begin ); break;
            case 4: quicksort_base_4( a+begin ); break;
            case 3: quicksort_base_3( a+begin ); break;
            case 2: quicksort_base_2( a+begin ); break;
        }
        return;
    }

	unsigned q = partition(a,begin,end);
 	
	quicksort_rec(a,begin,q);
	quicksort_rec(a,q,end);
}

/* iterative */
#define STACK
#define xVECTOR
#define xPRIORITY_QUEUE 

#include <utility> // std::pair

template <typename T>
using triple = typename std::pair< T*, std::pair<unsigned,unsigned>>;

template< typename T>
struct compare_triples {
    bool operator() ( triple<T> const& op1, triple<T> const& op2 ) const {
        return op1.second.first > op2.second.first;
    }
};

#ifdef STACK
#include <stack>
template< typename T>
using Container = std::stack< triple<T>>;
#define PUSH push
#define TOP  top
#define POP  pop
#endif

#ifdef VECTOR
#include <vector>
template< typename T>
using Container = std::vector< triple<T>>;
#define PUSH push_back
#define TOP  back
#define POP  pop_back
#endif

#ifdef PRIORITY_QUEUE
#include <queue>
template< typename T>
using Container = std::priority_queue< triple<T>, std::vector<triple<T>>, compare_triples<T> >;
#define PUSH push
#define TOP  top
#define POP  pop
#endif

template< typename T>
void quicksort_iterative_aux( Container<T> & ranges );

template< typename T>
void quicksort_iterative( T* a, unsigned begin, unsigned end )
{
    Container<T> ranges;
    ranges.PUSH( std::make_pair( a, std::make_pair( begin,end ) ) );
    quicksort_iterative_aux( ranges );
}

template< typename T>
void quicksort_iterative_aux( Container<T> & ranges )
{
    while ( ! ranges.empty() ) {
        triple<T> r = ranges.TOP();
        ranges.POP();
        
        T*       a = r.first;
        unsigned b = r.second.first;
        unsigned e = r.second.second;
        
        //base case
        if (e-b<6) {
            switch ( e-b ) {
                case 5: quicksort_base_5( a+b ); break;
                case 4: quicksort_base_4( a+b ); break;
                case 3: quicksort_base_3( a+b ); break;
                case 2: quicksort_base_2( a+b ); break;
            }
            continue;
        }

        unsigned q = partition(a,b,e);

        ranges.PUSH( std::make_pair( a, std::make_pair( b,q ) ) );
        ranges.PUSH( std::make_pair( a, std::make_pair( q+1,e ) ) );
    }
}
