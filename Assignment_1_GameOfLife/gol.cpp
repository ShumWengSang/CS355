#include "gol.h"
#include <iostream>
#include <pthread.h>
#include <algorithm>
#include <semaphore.h>

// #define DEBUG
// #define LOUD
void draw( std::vector< std::tuple<int,int> > & population, int max_x, int max_y );

using Position = std::tuple<int, int>;
using Mutex = pthread_mutex_t;
using Thread = pthread_t;
using Semaphore = sem_t ;

Semaphore writeArraySemaphore;

struct Arguments
{
    std::vector<Position> * positions;
    Thread* threads;
    int x;
    int y;
    int max_x, max_y;
    int iterations;
};

struct Barrier
{
    Semaphore turnStile1;
    Semaphore turnStile2;
    Mutex mutex;
    int count = 0;
    int maxCount;
    Barrier(int maxCount) : maxCount(maxCount)
    {
        sem_init(&turnStile1, 0, 0);
        sem_init(&turnStile2, 0, 0);
        pthread_mutex_init(&mutex, 0);
    }
    ~Barrier()
    {
        sem_destroy(&turnStile1);
        sem_destroy(&turnStile2);
        pthread_mutex_destroy(&mutex);
    }

    void Phase1()
    {
        pthread_mutex_lock(&mutex);
        {
            count++;
            //printf("1: Count is %d and maxCount is %d\n", count, maxCount);
            if(count == maxCount)
            {
                for(int i = 0; i < maxCount; ++i)
                {
                    sem_post(&turnStile1);
                }
            }
        }
        pthread_mutex_unlock(&mutex);
        sem_wait(&turnStile1);
    }

    void Phase2()
    {
        pthread_mutex_lock(&mutex);
        {
            count--;
            if(count == 0)
            {
                for(int i = 0; i < maxCount; ++i)
                {
                    sem_post(&turnStile2);
                }
            }
        }
        pthread_mutex_unlock(&mutex);
        sem_wait(&turnStile2);
    }

    void Wait()
    {
        Phase1();
        Phase2();
    }
};

Barrier* waitForFinishCalc;
Barrier* waitForFinishWrite;

// For debug purposes
std::ostream& operator<<(std::ostream& os, Position const & pos )
{
    os << "[" << std::get<0>(pos) << ", " << std::get<1>(pos) << "]";
    return os;
}



static int countLiveNeighbors(std::vector<Position> const & positions, int curr_x, int curr_y, int max_x, int max_y)
{
    int neighborCount = 0;

    // Count the neighbors around you.
    for(int i = -1; i <= 1; ++i)
    {
        for(int j = -1; j <= 1; ++j)
        {
            const int eval_y = curr_y + i;
            const int eval_x = curr_x + j;
            if(i == 0 && j == 0) // Ignore self
                continue;
            if(eval_y < 0 || eval_y >= max_y) // Y bounds checking
                continue;
            if(eval_x < 0 || eval_x >= max_x) // X bounds checking
                continue;
            // Check if thing exists within vector
            // printf("Evaluating 2 %d %d\n", eval_x, eval_y);
            if(std::find(positions.begin(), positions.end(),std::make_tuple(eval_x, eval_y)) != positions.end())
            {
                // It exists, increment neighbor count
                ++neighborCount;
                #ifdef DEBUG
                #ifdef LOUD
                 printf("    Found neighbor for [%d,%d]. It is [%d,%d]. Incrementing neighbour count to %d \n",
                     curr_x, curr_y, eval_x, eval_y, neighborCount);
                #endif
                #endif
            }
        }
    }
    return neighborCount;
}

// Return true if cell survives, false if cell does not.
static bool ProcessCell(std::vector<Position> & positions, int curr_x, int curr_y, int max_x, int max_y)
{
    // Get if cell is dead or alive
    bool isAlive = (std::find(positions.begin(), positions.end(),std::make_tuple(curr_x, curr_y)) 
                    != positions.end());
    // Get the number of neighbors
    int neighborCount = countLiveNeighbors(positions, curr_x, curr_y, max_x, max_y);

    // if cell is alive with two or three neighbors, survive
    if(isAlive && (neighborCount == 2 || neighborCount == 3))
    {
        // Survive = do nothing to change array.
        return true;
    }
    // if cell is dead with three live neighbors, live
    else if(!isAlive && neighborCount == 3)
    {
        // To change from dead to alive means to write into array
        // positions.emplace_back(std::make_tuple(curr_x, curr_y));
        return true;
    }
    // else if live, die.
    else 
    {
        return false;
    }
}

static void * RunCell(void *p)
{
    Arguments* args = reinterpret_cast<Arguments*>(p);

    // Retrieve all arguments
    std::vector<Position> & pos = *args->positions;

    int iterations = args->iterations;
    //printf("Here 1\n");
    while(iterations-- > 0)
    {
        // Get current state of cell
        auto cellVectorIter = std::find(pos.begin(), pos.end(), std::make_tuple(args->x, args->y));
        bool isAlive = (cellVectorIter != pos.end());

        // Calculate next step
        bool nextState = ProcessCell(pos, args->x, args->y, args->max_x, args->max_y);

        // Wait for all threads to finish calculations.
        waitForFinishCalc->Wait();

        // Write state to array
        sem_wait(&writeArraySemaphore);
        {
            if(nextState != isAlive) // If our state changed
            {
                // If we are alive, add to it.
                if(nextState)
                {
                    #ifdef DEBUG
                    printf("Turning [%d,%d] alive\n", args->x, args->y);
                    #endif
                    pos.emplace_back(std::make_tuple(args->x, args->y));
                }
                else
                // If we are dead, remove from array.
                {
                    #ifdef DEBUG
                    printf("Killing [%d,%d]\n", args->x, args->y);
                    #endif
                    pos.erase(std::find(pos.begin(), pos.end(), std::make_tuple(args->x, args->y)));
                }
            }
        }
        sem_post(&writeArraySemaphore);
        // Wait for all threads to finish writes
        waitForFinishWrite->Wait();
        #ifdef DEBUG
        if(args->x == 0 && args->y == 0)
            draw(pos, args->max_x, args->max_y);
        #endif
    }
    return 0;
}


std::vector<Position>
run(std::vector<Position> initial_population, int num_iter, int max_x, int max_y)
{
    // Reserve enough spots so it never reallocates
    initial_population.reserve(max_x * max_y);

    // Get the number of threads we need to make
    const int numOfThreads = max_x * max_y;

    // Initialize mutex and semaphore
    {
        sem_init(&writeArraySemaphore, 0, 1);
        waitForFinishCalc = new Barrier(numOfThreads);
        waitForFinishWrite = new Barrier(numOfThreads);
    }

    // Preallocate all arguments
    Arguments* args;
    {
        args = new Arguments[numOfThreads];
    }
#ifdef DEBUG
    draw(initial_population, max_x, max_y);
#endif
    // Initialize all the threads with appropriate arguments
    Thread* threadIDs; // Array of pthread IDs
    {
        threadIDs = new Thread[numOfThreads];
        for(int i = 0; i < max_x; ++i ) 
        {
            for(int j = 0; j < max_y; ++j)
            {
                int index = i * max_x + j;
                // Prepare the arguments
                args[index].positions = &initial_population;
                args[index].max_x = max_x;
                args[index].max_y = max_y;
                args[index].x = i;
                args[index].y = j;
                args[index].iterations = num_iter;
                args[index].threads = threadIDs;
                // Create each thread.
                if(pthread_create( &threadIDs[index], 0, RunCell, &args[index]) != 0)
                    std::cerr << "Error! PThread creation error!" << std::endl;
            }
        }
    }

    // Wait for all threads to finish and delete
    {
        for(int i = 0; i < numOfThreads; i++ ) 
        {
            // Wait for each thread
            pthread_join( threadIDs[i], 0);
        }
        // Free memory
        delete[] threadIDs;
        delete[] args;
    }

    // Destroy mutex
    {
        sem_destroy(&writeArraySemaphore);
        delete waitForFinishCalc;
        delete waitForFinishWrite;
    }
    return initial_population;
}