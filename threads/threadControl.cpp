#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <queue>

class ThreadManager 
{
private:
    std::mutex mtx;
    std::condition_variable cv;
    int active_thread = 0;
    bool thread3_waiting = false;

public:
    uint count[4] = {0, 0, 0, 0};
    void wait_for_turn(int id) 
    {
        std::unique_lock<std::mutex> lock(mtx);
        if (id == 3) thread3_waiting = true;
        cv.wait(lock, [this, id] 
        { 
            return active_thread == 0 && (id == 3 || !thread3_waiting); 
        }
        );
        active_thread = id;
        if (id == 3) thread3_waiting = false;
    }

    void release_turn() 
    {
        std::unique_lock<std::mutex> lock(mtx);
        active_thread = 0;
        cv.notify_all();
    }
};

ThreadManager manager;

void thread_function(int id) 
{
    manager.wait_for_turn(id);
    std::cout << "Thread-" << id << " started ";
    for (int i = 0; i < 10; i++) {
        std::cout << "." << id << ".";
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    manager.count[id - 1]++;
    std::cout << " Thread-" << id << " finished " << manager.count[id-1] << " times." << std::endl;

    manager.release_turn();
}

void t1() 
{
    while (true) {
        thread_function(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void t2() 
{
    while (true) {
        thread_function(2);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void t3() 
{
    while (true) {
        thread_function(3);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void t4() 
{
    while (true) {
        thread_function(4);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() 
{
    std::thread thread1(t1);
    std::thread thread2(t2);
    std::thread thread3(t3);
    std::thread thread4(t4);
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    return 0;
}