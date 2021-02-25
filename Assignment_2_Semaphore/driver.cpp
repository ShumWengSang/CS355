#include "semaphore.h"
#include <atomic>
#include <chrono>
#include <thread>
#include <random>
#include <iostream>
#include <cstdlib> // atoi

int const max_threads = 5;
std::atomic<int> sleepers( 0 );
std::atomic<int> stop( 0 );
Semaphore sem( max_threads );

void sleeper_thread( int num_ms ) 
{
    sem.wait();
    ++sleepers;
    std::this_thread::sleep_for( std::chrono::milliseconds( num_ms ) );
    --sleepers;
    sem.signal();
}

void watcher() 
{
    while ( !stop ) {
        int sleeper_count = sleepers.load();
        if ( sleeper_count != max_threads  ) {
            if ( sleeper_count > max_threads  ) {
                std::cout << sleeper_count << " too many sleepers - definitely an error\n";
            } else {
//                std::cout << sleeper_count << " too few sleepers - OK\n";
            }
        }
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
    }
}

void test0()
{
    std::random_device rd;
    std::mt19937 gen( rd() );
    std::uniform_int_distribution<> dis(40, 100);

    std::thread tids[100];
    std::thread w( watcher );
    for( int i=0; i<100; ++i ) {
        tids[i] = std::thread( sleeper_thread, dis( gen ) );
    }

    for ( auto & t : tids ) {
        t.join();
    }
    stop = 1;
    w.join();

    std::cout << "Done\n";
}

void test1()
{
    int const times[] = {
        950,390,80,400,990,930,270,450,730,820,100,10,770,950,40,10,900,170,280,240,190,210,450,590,320,
        830,800,480,10,440,980,480,350,70,890,860,20,680,310,260,20,930,790,800,400,830,330,310,10,140
        // total 23260
        // divided by 5 =  4652ms - set timeout 4.9s

        //550,720,350,520,830,200,360,640,680,890,80,180,370,950,250,780,810,780,460,640,560,490,580,350,
        //290,980,190,140,810,720,280,370,440,160,890,280,360,770,920,560,660,520,740,560,470,520,340,
        //800,820,810
    };

    std::thread tids[ sizeof(times)/sizeof(times[0]) ];
    std::thread w( watcher );
    for( unsigned i=0; i<sizeof(times)/sizeof(times[0]); ++i ) {
        tids[i] = std::thread( sleeper_thread, times[i] );
    }

    for ( auto & t : tids ) {
        t.join();
    }
    stop = 1;
    w.join();

    std::cout << "Done\n";
}

void (*pTests[])() = { test0, test1 }; 

int main (int argc, char ** argv) {
	if (argc >1) {
		pTests[atoi( argv[1] )]();
	}
	return 0;
}
