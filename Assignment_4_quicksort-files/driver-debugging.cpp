#include <iostream> //cout, endl
#include <cstdlib>  //srand
#include <algorithm>//copy, random_shuffle
#include <iterator> //ostream_iterator
#include <ctime>    //std::time (NULL) to seed srand
#include <cstdio>    /* sscanf */
#include <chrono>    /* C++11 timer */
#include "quicksort.h"
#include "ratio.h"

#define xREC
#define xITER
#define ITER_PAR
#define xUSER_QUEUE
#define xSTD

int main( int argc, char ** argv ) {
	int n = 2000;
    int num_threads = 2;

    Ratio *a = new Ratio[n];
	
    if ( argc > 1 ) {
        std::sscanf( argv[1], "%i", &num_threads );
    }
//    std::copy(a, a + n, std::ostream_iterator<int>(std::cout, " ")); 
//    std::cout << std::endl;
	
    // master array
	for ( int i=0; i<n; ++i ) { Ratio r( 1, i+1 ); a[i] = r; }
    std::srand( static_cast<unsigned int>(std::time (NULL)) );
    //std::srand( static_cast<unsigned int>(2) );
    std::random_shuffle( a, a+n );

    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed_seconds = end-start;

#ifdef REC
    Ratio *a1 = new Ratio[n];
	for ( int i=0; i<n; ++i ) { Ratio r( 1, i+1 ); a1[i] = a[i]; }
    std::cout << "Recursive" << std::endl;
    start = std::chrono::system_clock::now();
    quicksort_rec( a1, 0, n );
    //std::cout << "Number of comparisons " << Ratio::GetLessCounter() << std::endl;
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    for ( int i=0; i<n-1; ++i ) {
        if ( ! (a1[i] < a1[i+1] ) ) {
            std::cout << "A1 - Error at index " << i << ": " << a1[i] << " is not < " << a1[i+1] << std::endl;
            break;
        }
    }
    delete [] a1;
#endif

#ifdef ITER
    Ratio *a2 = new Ratio[n];
	for ( int i=0; i<n; ++i ) { Ratio r( 1, i+1 ); a2[i] = a[i]; }
    std::cout << "Iterative" << std::endl;
    Ratio::ZeroLessCounter();
    start = std::chrono::system_clock::now();
    quicksort_iterative( a2, 0, n );
    //std::cout << "Number of comparisons " << Ratio::GetLessCounter() << std::endl;
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    for ( int i=0; i<n-1; ++i ) {
        if ( ! (a2[i] < a2[i+1] ) ) {
            std::cout << "A2 - Error at index " << i << ": " << a2[i] << " is not < " << a2[i+1] << std::endl;
            break;
        }
    }
    delete [] a2;
#endif

#ifdef ITER_PAR
    Ratio *a3 = new Ratio[n];
	for ( int i=0; i<n; ++i ) { Ratio r( 1, i+1 ); a3[i] = a[i]; }
    std::cout << "Iterative parallel" << std::endl;
    Ratio::ZeroLessCounter();
    start = std::chrono::system_clock::now();
    quicksort( a3, 0, n, num_threads );
    std::cout << "Number of comparisons " << Ratio::GetLessCounter() << std::endl;
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    for ( int i=0; i<n-1; ++i ) {
        if ( ! (a3[i] < a3[i+1] ) ) {
            std::cout << "A3 - Error at index " << i << ": " << a3[i] << " is not < " << a3[i+1] << std::endl;
            break;
        }
    }
    delete [] a3;
#endif

#ifdef STD
    Ratio *a4 = new Ratio[n];
	for ( int i=0; i<n; ++i ) { Ratio r( 1, i+1 ); a4[i] = a[i]; }
    std::cout << "Standard" << std::endl;
    Ratio::ZeroLessCounter();
    start = std::chrono::system_clock::now();
    std::sort( a4, a4+n );
    std::cout << "Number of comparisons " << Ratio::GetLessCounter() << std::endl;
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    for ( int i=0; i<n-1; ++i ) {
        if ( ! (a4[i] < a4[i+1] ) ) {
            std::cout << "A4 - Error at index " << i << ": " << a4[i] << " is not < " << a4[i+1] << std::endl;
            break;
        }
    }
    delete [] a4;
#endif

#ifdef USER_QUEUE
    Ratio *a5 = new Ratio[n];
	for ( int i=0; i<n; ++i ) { Ratio r( 1, i+1 ); a5[i] = a[i]; }
    std::cout << "User lockless queue" << std::endl;
    Ratio::ZeroLessCounter();
    start = std::chrono::system_clock::now();
    quicksort2( a5, 0, n, num_threads );
    //std::cout << "Number of comparisons " << Ratio::GetLessCounter() << std::endl;
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    for ( int i=0; i<n-1; ++i ) {
        if ( ! (a5[i] < a5[i+1] ) ) {
            std::cout << "A5 - Error at index " << i << ": " << a5[i] << " is not < " << a5[i+1] << std::endl;
            break;
        }
    }
    delete [] a5;
#endif

    delete [] a;
    return 0;
}
