#include <mutex>
#include <condition_variable>

class Semaphore
{
public:
	Semaphore(int startCount);
	void wait();
	void signal();
private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};
