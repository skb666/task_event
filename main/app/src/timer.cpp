#include "timer.hpp"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

Timer::Timer() : _expired(true), _try_to_expire(false) {}

Timer::Timer(const Timer& timer) {
    _expired = timer._expired.load();
    _try_to_expire = timer._try_to_expire.load();
}

Timer::~Timer() {
    stop();
}

void Timer::start(int interval, std::function<void()> task) {
    // is started, do not start again
    if (_expired == false)
        return;

    // start async timer, launch thread and wait in that thread
    _expired = false;
    std::thread([this, interval, task]() {
        while (!_try_to_expire) {
            // sleep every interval and do the task again and again until times up
            auto time_until = std::chrono::system_clock::now() + std::chrono::milliseconds(interval);
            task();
            std::this_thread::sleep_until(time_until);
        }

        {
            // timer be stopped, update the condition variable expired and wake main thread
            std::lock_guard<std::mutex> locker(_mutex);
            _expired = true;
            _expired_cond.notify_one();
        }
    }).detach();
}

void Timer::startOnce(int delay, std::function<void()> task) {
    std::thread([this, delay, task]() {
        std::cout << "enter start once" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        task();
    }).detach();
}

void Timer::stop() {
    // do not stop again
    if (_expired)
        return;

    if (_try_to_expire)
        return;

    // wait until timer
    _try_to_expire = true;  // change this bool value to make timer while loop stop
    {
        std::unique_lock<std::mutex> locker(_mutex);
        _expired_cond.wait(locker, [this] { return _expired == true; });

        // reset the timer
        if (_expired == true) {
            _try_to_expire = false;
        }
    }
}

bool Timer::is_running() {
    return !_expired;
}
