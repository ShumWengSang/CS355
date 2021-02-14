#include "gol.h"
#include <iostream>
#include <pthread.h>
#include <algorithm>
#include <semaphore.h>
#include <unordered_map>

//#define DEBUG
//#define LOUD
void draw( std::vector< std::tuple<int,int> > & population, int max_x, int max_y );

using Position = std::tuple<int, int>;
using Mutex = pthread_mutex_t;
using Thread = pthread_t;
using Semaphore = sem_t ;

Semaphore writeArraySemaphore;

struct Arguments
{
    std::vector<std::vector<bool>>* Map;
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
            if(++count == maxCount)
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
            if(--count == 0)
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

//static bool isNeighborOf(int curr_x, int curr_y, int other_x, int other_y)
// {
//     int xDiff = std::abs(curr_x - other_x);
//     int yDiff = std::abs(curr_y - other_y);
//     if(xDiff == 0 && yDiff == 0) // Ignore self
//         return false;
//     if(xDiff <= 1 && yDiff <= 1) // If the difference is only one cell away
//         return true;
//     return false;
// }

static int countLiveNeighbors(std::vector<std::vector<bool>> const & map, int curr_x, int curr_y)
{
    int neighborCount = 0;
    const int max_y = map[0].size();
    const int max_x = map.size(); 

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
            if(map[eval_x][eval_y])
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
static bool ProcessCell(std::vector<std::vector<bool>> const & map, int curr_x, int curr_y)
{
    // Get if cell is dead or alive
    bool isAlive = map[curr_x][curr_y];
    // Get the number of neighbors
    int neighborCount = countLiveNeighbors(map, curr_x, curr_y);

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
    std::vector<std::vector<bool>> & Map = *args->Map;

    int iterations = args->iterations;
    //printf("Here 1\n");
    while(iterations-- > 0)
    {
        // Get current state of cell
        bool isAlive = Map[args->x][args->y];

        // Calculate next step
        bool nextState = ProcessCell(Map, args->x, args->y);

        // Wait for all threads to finish calculations.
        waitForFinishCalc->Wait();

        // Write state to array
        {
            if(nextState != isAlive) // If our state changed
            {
                // If we are alive, add to it.
                if(nextState)
                {
                    #ifdef DEBUG
                    printf("Turning [%d,%d] alive\n", args->x, args->y);
                    #endif
                    Map[args->x][args->y] = true;
                }
                else
                // If we are dead, remove from array.
                {
                    #ifdef DEBUG
                    printf("Killing [%d,%d]\n", args->x, args->y);
                    #endif
                    Map[args->x][args->y] = false;
                }
            }
        }
        
        // Wait for all threads to finish writes
        waitForFinishWrite->Wait();
        #ifdef DEBUG
        if(args->x == 0 && args->y == 0)
        {
            std::vector<Position> result;

            for(unsigned i = 0; i < Map.size(); ++i)
            {
                for(unsigned j = 0; j < Map[i].size(); ++j)
                {
                    if(Map[i][j])
                    {
                        result.emplace_back(std::make_tuple(i,j));
                    }
                }
            }
            draw(result, args->max_x, args->max_y);
        }
        #endif
    }
    return 0;
}


std::vector<Position>
run(std::vector<Position> initial_population, int num_iter, int max_x, int max_y)
{
    (void)max_x;
    (void)max_y;

    // Reserve enough spots so it never reallocates
    initial_population.reserve(max_x * max_y);

    // Get the number of threads we need to make
    const int numOfThreads = max_x * max_y;

    // Initialize mutex and semaphore
    {
        // sem_init(&writeArraySemaphore, 0, 1);
        waitForFinishCalc = new Barrier(numOfThreads);
        waitForFinishWrite = new Barrier(numOfThreads);
    }

    // Preallocate all arguments
    Arguments* args;
    {
        args = new Arguments[numOfThreads];
    }

    // We will create our own representation of the 2D Map
    std::vector<std::vector<bool>> Map;
    {
        Map.resize(max_x);
        for(int i = 0; i < max_x; i++)
        {
            Map[i].resize(max_y);
        }
        // Fill it with the positions
        for(unsigned i = 0; i < initial_population.size(); ++i)
        {
            int element_x = std::get<0>(initial_population[i]);
            int element_y = std::get<1>(initial_population[i]);
            Map[element_x][element_y] = true;
        }
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
                args[index].Map = &Map;
                args[index].x = i;
                args[index].y = j;
                args[index].iterations = num_iter;
                args[index].threads = threadIDs;
                args[index].max_x = max_x;
                args[index].max_y = max_y;
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

    // Get the positions of the data client wants.
    std::vector<Position> result;
    result.reserve(10);

    for(unsigned i = 0; i < Map.size(); ++i)
    {
        for(unsigned j = 0; j < Map[i].size(); ++j)
        {
            if(Map[i][j])
            {
                result.emplace_back(std::make_tuple(i,j));
            }
        }
    }

    // Destroy mutex
    {
        // sem_destroy(&writeArraySemaphore);
        delete waitForFinishCalc;
        delete waitForFinishWrite;
    }
    return result;
}