//
// Created by itssiem on 3/18/24.
//

#ifndef TIMER_H
#define TIMER_H
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

class Timer {
    std::atomic<bool> active{true};

public:
    void setTimeout(auto function, int delay);
    void setInterval(auto function, int interval);
    void stop();

};

void Timer::setTimeout(auto function, int delay) {
    active = true;
    std::thread t([=, this]() {
        if(!active.load()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(!active.load()) return;
        function();
    });
    t.detach();
}

void Timer::setInterval(auto function, int interval) {
    active = true;
    std::thread t([=, this]() {
        while(active.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(!active.load()) return;
            function();
        }
    });
    t.detach();
}

inline void Timer::stop() {
    active = false;
}
#endif //TIMER_H
