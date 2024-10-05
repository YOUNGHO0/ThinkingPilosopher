#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>

std::vector<bool> fork(5, false);
std::vector<std::mutex> forkMutex(5);
std::condition_variable cv;
std::mutex coutMutex;  // 콘솔 출력을 위한 뮤텍스 추가

void eat(int personNumber) {
    // 첫 번째 포크 시도
    if (!forkMutex[personNumber].try_lock()) {
        std::lock_guard<std::mutex> coutLock(coutMutex);
        std::cout << personNumber << " failed to lock fork " << personNumber << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 대기 후 다시 시도
        return; // 현재 식사 중단
    }

    // 두 번째 포크 시도
    int next = (personNumber + 1) % 5;
    if (!forkMutex[next].try_lock()) {
        std::lock_guard<std::mutex> coutLock(coutMutex);
        std::cout << personNumber << " failed to lock fork " << next << std::endl;
        forkMutex[personNumber].unlock(); // 첫 번째 포크를 내려놓음
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 대기 후 다시 시도
        return; // 현재 식사 중단
    }

    // 콘솔 출력 시 동기화
    {
        std::lock_guard<std::mutex> coutLock(coutMutex);
        std::cout << personNumber << " eats with fork " << personNumber << " and fork " << next << std::endl;
    }

    // 식사 후 포크를 내려놓음
    forkMutex[next].unlock();
    forkMutex[personNumber].unlock();
}

int main() {
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(eat, i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
