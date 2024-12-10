#include "task.h"
#include "timer.h"
#include "timer.hpp"

void main_init(void) {
    task_init();
}

void main_loop(void) {
    task_loop();
}

int main() {
    Timer systick;
    systick.start(1, [] {
        timer_increase();
        task_time_loop();
    });

    main_init();
    while (1) {
        main_loop();
    }

    return 0;
}
