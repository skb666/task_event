#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>

class Timer {
   public:
    Timer();
    Timer(const Timer& timer);
    ~Timer();

    void start(int interval, std::function<void()> task);
    void startOnce(int delay, std::function<void()> task);
    void stop();
    bool is_running();

   private:
    std::atomic<bool> _expired;        // timer stopped status
    std::atomic<bool> _try_to_expire;  // timer is in stop process
    std::mutex _mutex;
    std::condition_variable _expired_cond;
};


/*
int main() {
    using namespace std;

    Timer timer;

    // execute task every timer interval
    cout << "--- start period timer ----" << endl;
    timer.start(1000, bind(func2, 3));
    cout << "sleep_for 10000 ms" << endl;
    this_thread::sleep_for(chrono::milliseconds(10000));
    timer.stop();
    cout << "--- stop period timer ----" << endl;

    // execute task once after delay

    cout << "----------------------------\n"
         << endl;
    cout << "--- start one shot timer ----" << endl;
    timer.startOnce(5000, bind(func1));
    this_thread::sleep_for(chrono::milliseconds(5000));
    cout << "--- stop one shot timer ----" << endl;

    return 0;
}
*/