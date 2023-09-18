#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

#include "task.h"
#include "timer.h"
#include "timer.hpp"

int main() {
    using namespace std;

    task_init();

    Timer timer;
    timer.start(1, [=] {
        timer_increase();
        timer_loop();
    });

    while (1) {
        task_loop();
    }

    return 0;
}
