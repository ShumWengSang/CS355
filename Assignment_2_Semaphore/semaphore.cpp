#include "semaphore.h"

Semaphore::Semaphore(int startCount) : count(startCount)
{

}

void Semaphore::signal()
{
    std::unique_lock<std::mutex> lock(mtx);
    count++;
    cv.notify_one();
}

void Semaphore::wait()
{
    std::unique_lock<std::mutex> lock(mtx);
    while(count == 0)
    {
        cv.wait(lock);
    }
    count--;
}